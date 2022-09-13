/**
  ******************************************************************************
  * @file    usbh_hub.c
  * @author  Mori
  * @version
  * @date
  * @brief   Definitions used in the USB Hub support
  ******************************************************************************
  * Copyright (c) 2016 Mori
  *
  * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
  * associated documentation files (the "Software"), to deal in the Software without restriction,
  * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
  * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
  * subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in all copies or substantial
  * portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
  * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  *
  ******************************************************************************
*/

#include "../Inc/usbh_hub.h"

static USBH_StatusTypeDef set_hub_port_power(USBH_HandleTypeDef *phost, uint8_t hub_port);
static USBH_StatusTypeDef hub_request(USBH_HandleTypeDef *phost, uint8_t request, uint8_t feature, uint8_t dataDirection, uint8_t porta, uint8_t *buffer, uint16_t size);
static USBH_StatusTypeDef set_hub_request(USBH_HandleTypeDef *phost, uint8_t request, uint8_t feature, uint8_t port);
static USBH_StatusTypeDef get_hub_request(USBH_HandleTypeDef *phost, uint8_t request, uint8_t feature, uint8_t porta, uint8_t *buffer, uint16_t size);
static USBH_StatusTypeDef clear_port_feature(USBH_HandleTypeDef *phost, uint8_t feature, uint8_t port);
static USBH_StatusTypeDef set_port_feature(USBH_HandleTypeDef *phost, uint8_t feature, uint8_t port);

static void clear_port_changed(HUB_HandleTypeDef *HUB_Handle, uint8_t port);
static uint8_t get_port_changed(HUB_HandleTypeDef *HUB_Handle);
static uint8_t port_changed(HUB_HandleTypeDef *HUB_Handle, const uint8_t *b, unsigned len);

static void detach(USBH_HandleTypeDef *phost, uint16_t idx);
static void attach(USBH_HandleTypeDef *phost, uint16_t idx, uint8_t lowspeed);
static void debug_port(uint8_t *buff, __IO USB_HUB_PORT_STATUS *info);

static USBH_StatusTypeDef hub_request(USBH_HandleTypeDef *phost, uint8_t request, uint8_t feature, uint8_t dataDirection, uint8_t porta,
		uint8_t *buffer, uint16_t size)
{
	uint8_t bmRequestType = (dataDirection == USB_DEVICE_REQUEST_GET) ? USB_D2H : USB_H2D; // 0x23

	phost->Control.setup.b.bmRequestType = bmRequestType | USB_REQ_RECIPIENT_OTHER | USB_REQ_TYPE_CLASS;
	phost->Control.setup.b.bRequest  	 = request;
	phost->Control.setup.b.wValue.bw.msb = feature;
	phost->Control.setup.b.wValue.bw.lsb = 0;
	phost->Control.setup.b.wIndex.bw.msb = porta;
	phost->Control.setup.b.wIndex.bw.lsb = 0;
	phost->Control.setup.b.wLength.w     = size;

	return USBH_CtlReq(phost, buffer, size);

//	USBH_StatusTypeDef res = USBH_FAIL;
//	do { res = USBH_CtlReq(phost, buffer, size); } while(res != USBH_OK && res != USBH_FAIL);
//	return res;
}

// porta: 1..n
static USBH_StatusTypeDef set_hub_port_power(USBH_HandleTypeDef *phost, uint8_t porta)
{
	return hub_request(phost, USB_REQ_SET_FEATURE, HUB_FEATURE_SEL_PORT_POWER, USB_DEVICE_REQUEST_SET, porta, 0, 0);
}

// porta: 1..n
static USBH_StatusTypeDef get_hub_request(USBH_HandleTypeDef *phost, uint8_t request, uint8_t feature, uint8_t porta, uint8_t *buffer, uint16_t size)
{
    return hub_request(phost, request, feature, USB_DEVICE_REQUEST_GET, porta, buffer, size);
}

// porta: 1..n
static USBH_StatusTypeDef set_hub_request(USBH_HandleTypeDef *phost, uint8_t request, uint8_t feature, uint8_t porta)
{
    return hub_request(phost, request, feature, USB_DEVICE_REQUEST_SET, porta, 0, 0);
}

// porta: 1..n
static USBH_StatusTypeDef clear_port_feature(USBH_HandleTypeDef *phost, uint8_t feature, uint8_t porta)
{
    return set_hub_request(phost, USB_REQUEST_CLEAR_FEATURE, feature, porta);
}

// porta: 1..n
static USBH_StatusTypeDef set_port_feature(USBH_HandleTypeDef *phost, uint8_t feature, uint8_t porta)
{
    return set_hub_request(phost, USB_REQUEST_SET_FEATURE, feature, porta);
}
//
//// porta: 1..n
//static void clear_port_changed(HUB_HandleTypeDef *HUB_Handle, uint8_t porta)
//{
//	switch (porta)
//	{
//	case 1:
//		HUB_Handle->HUB_Change.bPorts.PORT_1 = 0;
//		break;
//	case 2:
//		HUB_Handle->HUB_Change.bPorts.PORT_2 = 0;
//		break;
//	case 3:
//		HUB_Handle->HUB_Change.bPorts.PORT_3 = 0;
//		break;
//	case 4:
//		HUB_Handle->HUB_Change.bPorts.PORT_4 = 0;
//		break;
//	case 5:
//		HUB_Handle->HUB_Change.bPorts.PORT_5 = 0;
//		break;
//	case 6:
//		HUB_Handle->HUB_Change.bPorts.PORT_6 = 0;
//		break;
//	case 7:
//		HUB_Handle->HUB_Change.bPorts.PORT_7 = 0;
//		break;
//	case 8:
//		HUB_Handle->HUB_Change.bPorts.PORT_8 = 0;
//		break;
//	}
//}
//
//static uint8_t get_port_changed(HUB_HandleTypeDef *HUB_Handle)
//{
//	if (HUB_Handle->HUB_Change.bPorts.PORT_1)
//		return 1;
//	if (HUB_Handle->HUB_Change.bPorts.PORT_2)
//		return 2;
//	if (HUB_Handle->HUB_Change.bPorts.PORT_3)
//		return 3;
//	if (HUB_Handle->HUB_Change.bPorts.PORT_4)
//		return 4;
//	if (HUB_Handle->HUB_Change.bPorts.PORT_5)
//		return 5;
//	if (HUB_Handle->HUB_Change.bPorts.PORT_6)
//		return 6;
//	if (HUB_Handle->HUB_Change.bPorts.PORT_7)
//		return 7;
//	if (HUB_Handle->HUB_Change.bPorts.PORT_8)
//		return 8;
//
//	return 0;
//}
//
//static uint8_t port_changed(HUB_HandleTypeDef *HUB_Handle, const uint8_t *b, unsigned len)
//{
//	HUB_Handle->HUB_Change.val = 0x00;
//	if (b [0] != 0x00)
//	{
//
//		if (b [0] & (0x01 << 1))
//		{
//			HUB_Handle->HUB_Change.bPorts.PORT_1 = 1;
//		}
//		if (b [0] & (0x01 << 2))
//		{
//			HUB_Handle->HUB_Change.bPorts.PORT_2 = 1;
//		}
//		if (b [0] & (0x01 << 3))
//		{
//			HUB_Handle->HUB_Change.bPorts.PORT_3 = 1;
//		}
//		if (b [0] & (0x01 << 4))
//		{
//			HUB_Handle->HUB_Change.bPorts.PORT_4 = 1;
//		}
//		if (b [0] & (0x01 << 5))
//		{
//			HUB_Handle->HUB_Change.bPorts.PORT_5 = 1;
//		}
//		if (b [0] & (0x01 << 6))
//		{
//			HUB_Handle->HUB_Change.bPorts.PORT_6 = 1;
//		}
//		if (b [0] & (0x01 << 7))
//		{
//			HUB_Handle->HUB_Change.bPorts.PORT_7 = 1;
//		}
//		USBH_UsrLog("PORT STATUS CHANGE [0x%02X] [%d %d %d %d]",
//				b [0],
//				HUB_Handle->HUB_Change.bPorts.PORT_1,
//				HUB_Handle->HUB_Change.bPorts.PORT_2,
//				HUB_Handle->HUB_Change.bPorts.PORT_3,
//				HUB_Handle->HUB_Change.bPorts.PORT_4
//				);
//
//	}
//	return HUB_Handle->HUB_Change.val > 0;
//}

void detach(USBH_HandleTypeDef *_phost, uint16_t idx)
{
	USBH_UsrLog("detach %d", (int) idx);
	return;
	//USBH_HandleTypeDef *pphost = &hUSBHost[idx];
	USBH_HandleTypeDef *pphost = & hUsbHostHS;
	//if (pphost->hubValid)
	{
			//USBH_UsrLog("detach %d", pphost->hubAddress [idx]);
//
//		if(pphost->pUser != NULL)
//		{
//			pphost->pUser(pphost, HOST_USER_DISCONNECTION);
//		}
//
//	    if(pphost->pActiveClass != NULL)
//	    {
//	    	pphost->pActiveClass->DeInit(pphost);
//	    	pphost->pActiveClass = NULL;
//	    }

		//pphost->hubAddress	    = 0;
		//pphost->hubBusy  	    = 0;
		//host->ClassNumber = 0;
		//pphost->hubValid 	    = 0;
//
//		uint32_t i;
//		for(i = 0; i< USBH_MAX_DATA_BUFFER; i++)
//		{
//			pphost->device.Data[i] = 0;
//		}
//
//		for(i = 0; i < pphost->hubInstances; ++i)
//		{
//			USBH_free(pphost->hubDatas[i]);
//			pphost->hubDatas[i] = NULL;
//		}
//
//		pphost->hubInstances = 0;
		//pphost->device.is_connected = 0;
	}
}

static void attach(USBH_HandleTypeDef *phost,
		uint16_t idx, 	// Index of port on HUB
		uint8_t lowspeed)
{
	USBH_UsrLog("attach %d", idx);
	return;
//	//USBH_HandleTypeDef *pphost = &hUSBHost[idx];
//	USBH_HandleTypeDef *pphost = &hUsbHostHS;
//
////	if (pphost->hubValid)
////	{
////		USBH_UsrLog("ATTACH ERROR, ALREADY ATTACHED");
////		detach(pphost, idx);
////	}
//
//	//pphost->id 					= 0;//hUSBHost[0].id;
//	//pphost->hubAddress [idx]			= idx;	// ????
//	//pphost->hubHub				= 0;
////	pphost->pActiveClass 		= NULL;
////	pphost->ClassNumber 		= 0;
//
//#warning Then use HUB class. investigane Pipes usage.
//// Taken from https://github.com/mori-br/STM32F4HUB
//	//pphost->Pipes 				= phost->Pipes;
//	//(void)USBH_memcpy(pphost->Pipes, phost->Pipes, sizeof pphost->Pipes);
//
//    pphost->pUser 				= phost->pUser;
//	pphost->EnumState 			= ENUM_IDLE;
//	pphost->RequestState 		= CMD_SEND;
//	pphost->Timer 				= 0;
//	pphost->Control.errorcount 	= 0;
//	pphost->Control.state 		= CTRL_SETUP;
//	pphost->Control.pipe_size 	= lowspeed ? USBH_MPS_LOWSPEED: USBH_MPS_DEFAULT;
//	pphost->Target.dev_address 		= USBH_ADDRESS_DEFAULT;
//	pphost->Target.speed   		= lowspeed ? USBH_SPEED_LOW : USBH_SPEED_FULL;
//	pphost->device.is_connected = 1;
//
////	HCD_HandleTypeDef *phHCD =  &_hHCD[pphost->id];
////	USBH_LL_SetTimer (pphost, HAL_HCD_GetCurrentFrame(phHCD));
//
//	/* link the class tgo the USB Host handle */
////    pphost->pClass[pphost->ClassNumber++] = USBH_HID_CLASS;
////    pphost->pClass[pphost->ClassNumber++] = USBH_MSC_CLASS;
//
//    pphost->gState = HOST_ENUMERATION;
//
//	pphost->Control.pipe_out = phost->Control.pipe_out;
//	pphost->Control.pipe_in  = phost->Control.pipe_in;
//
//	uint8_t i = 0;
//	for(;i < USBH_MAX_NUM_INTERFACES; ++i)
//		pphost->hubDatas[i] = NULL;
//
//	pphost->hubInstances = 0;
//	//pphost->hubBusy  = 0;
//	//pphost->hubValid = 3;

//USBH_UsrLog("HUB stuff ok");
}

static void debug_port(uint8_t *buff, __IO USB_HUB_PORT_STATUS *info)
{
#if 0
    LOG1("PORT STATUS [%02X %02X %02X %02X] ", buff[0], buff[1], buff[2], buff[3]);

    if(info->wPortStatus.PORT_CONNECTION)       LOG1("PORT_CONNECTION ");
    if(info->wPortStatus.PORT_ENABLE)           LOG1("PORT_ENABLE ");
    if(info->wPortStatus.PORT_SUSPEND)          LOG1("PORT_SUSPEND ");
    if(info->wPortStatus.PORT_OVER_CURRENT)     LOG1("PORT_OVER_CURRENT ");
    if(info->wPortStatus.PORT_RESET)            LOG1("PORT_RESET ");
    if(info->wPortStatus.PORT_POWER)            LOG1("PORT_POWER ");
    if(info->wPortStatus.PORT_LOW_SPEED)        LOG1("PORT_LOW_SPEED ");
    if(info->wPortStatus.PORT_HIGH_SPEED)       LOG1("PORT_HIGH_SPEED ");
    if(info->wPortStatus.PORT_TEST)             LOG1("PORT_TEST ");
    if(info->wPortStatus.PORT_INDICATOR)        LOG1("PORT_INDICATOR ");

    if(info->wPortChange.C_PORT_CONNECTION)     LOG1("C_PORT_CONNECTION ");
    if(info->wPortChange.C_PORT_ENABLE)         LOG1("C_PORT_ENABLE ");
    if(info->wPortChange.C_PORT_SUSPEND)        LOG1("C_PORT_SUSPEND ");
    if(info->wPortChange.C_PORT_OVER_CURRENT)   LOG1("C_PORT_OVER_CURRENT ");
    if(info->wPortChange.C_PORT_RESET)          LOG1("C_PORT_RESET ");
    LOG1("\r\n");
#endif
}



static USBH_StatusTypeDef USBH_HUB_InterfaceInit (USBH_HandleTypeDef *phost, const USBH_TargetTypeDef * target)
{
	HUB_HandleTypeDef *HUB_Handle;
	//USBH_DbgLog ("USBH_HUB_InterfaceInit.");
	uint8_t interface;

	USBH_StatusTypeDef status = USBH_FAIL ;

	interface = USBH_FindInterface(phost, phost->pActiveClass->ClassCode, 0x00, 0xFF);

	if (interface == 0xFF) /* No Valid Interface */
	{
		status = USBH_FAIL;
		USBH_DbgLog ("Cannot Find the interface for %s class.", phost->pActiveClass->Name);
	}
	if (phost->hubInstances >= USBH_MAX_NUM_INTERFACES)
	{
		status = USBH_FAIL;
		USBH_DbgLog ("Too many hubs in chain.");
	}
	else
	{
		  // check USBH_free
		static HUB_HandleTypeDef staticHUB_Handle;
		phost->pActiveClass->pData = & staticHUB_Handle;
		//phost->hubDatas [phost->hubInstances] = (HUB_HandleTypeDef *) USBH_malloc(sizeof (HUB_HandleTypeDef));

		HUB_Handle = phost->pActiveClass->pData;
		phost->hubDatas [phost->hubInstances] = HUB_Handle;
		phost->hubInstances += 1;

		USBH_memset(HUB_Handle, 0, sizeof (HUB_HandleTypeDef));

		(void)USBH_memcpy(& HUB_Handle->target, target, sizeof HUB_Handle->target);

		HUB_Handle->parrent = phost->hubInstances == 1 ? NULL : phost->hubDatas [phost->hubInstances - 1];	/* todo: fix for chans */

		HUB_Handle->NumPorts = 0;
		HUB_Handle->pwrGoodDelay = 0;

		HUB_Handle->pChangeInfo = NULL;
		HUB_Handle->HUB_CurPort = 0;
		HUB_Handle->HUB_Change.val = 0;

		HUB_Handle->detectedPorts = 0;

		USBH_SelectInterface (phost, interface);

	    HUB_Handle->state     = HUB_IDLE;
	    HUB_Handle->ctl_state = HUB_REQ_IDLE;
	    HUB_Handle->ep_addr   = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bEndpointAddress;
	    HUB_Handle->length    = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].wMaxPacketSize;
	    HUB_Handle->poll      = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bInterval;

	    if (HUB_Handle->poll  < HUB_MIN_POLL)
	    	HUB_Handle->poll = HUB_MIN_POLL;

	    USBH_UsrLog ("USBH_HUB_InterfaceInit: device poll=%d, length=%d", HUB_Handle->poll, HUB_Handle->length);

	    if (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bEndpointAddress & 0x80)
	    {
	    	HUB_Handle->InEp = (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bEndpointAddress);
	    	HUB_Handle->InPipe  = USBH_AllocPipe(phost, HUB_Handle->InEp);

	    	// Open pipe for IN endpoint
	    	USBH_OpenPipe(phost, HUB_Handle->InPipe, HUB_Handle->InEp, & HUB_Handle->target, USB_EP_TYPE_INTR, HUB_Handle->length);
	    	USBH_LL_SetToggle (phost, HUB_Handle->InPipe, 0);
	    }

	    status = USBH_OK;
	}

	return status;
}

static USBH_StatusTypeDef USBH_HUB_InterfaceDeInit (USBH_HandleTypeDef *phost )
{
	HUB_HandleTypeDef * const HUB_Handle = phost->hubDatas [0];
	USBH_UsrLog("USBH_HUB_InterfaceDeInit");
//
//	if(HUB_Handle->InPipe != 0x00)
//	{
//		USBH_ClosePipe (phost, HUB_Handle->InPipe);
//		USBH_FreePipe  (phost, HUB_Handle->InPipe);
//		HUB_Handle->InPipe = 0;     // Reset the pipe as Free
//	}
//
	return USBH_OK;
}



static void USBH_HUB_ProcessDelay(
		HUB_HandleTypeDef *HUB_Handle,
		HUB_CtlStateTypeDef state,
	unsigned delayMS
	)
{
	HUB_Handle->tickstart = sys_now();
	HUB_Handle->wait = ulmax32(delayMS, 1000 / TICKS_FREQUENCY);
	HUB_Handle->ctl_state = HUB_DELAY;
	HUB_Handle->ctl_state_push = state;
}

// state machine - for each hub port...
static USBH_StatusTypeDef USBH_HUB_ClassRequest(USBH_HandleTypeDef *phost)
{
	USBH_StatusTypeDef status = USBH_BUSY;
	HUB_HandleTypeDef * const HUB_Handle = phost->hubDatas [0];

	switch (HUB_Handle->ctl_state)
	{
	case HUB_REQ_IDLE:
		phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_CLASS;
		phost->Control.setup.b.bRequest = USB_REQ_GET_DESCRIPTOR;
		phost->Control.setup.b.wValue.bw.msb = 0;
		phost->Control.setup.b.wValue.bw.lsb = USB_DESCRIPTOR_HUB;
		phost->Control.setup.b.wIndex.w = 0;
		phost->Control.setup.b.wLength.w = sizeof(USB_HUB_DESCRIPTOR);

		status = USBH_CtlReq(phost, HUB_Handle->buffer, sizeof(USB_HUB_DESCRIPTOR));
		if (status == USBH_OK)
		{
			USB_HUB_DESCRIPTOR *HUB_Desc = (USB_HUB_DESCRIPTOR*) HUB_Handle->buffer;
			HUB_Handle->NumPorts = (HUB_Desc->bNbrPorts > MAX_HUB_PORTS) ? MAX_HUB_PORTS : HUB_Desc->bNbrPorts;
			HUB_Handle->pwrGoodDelay = (HUB_Desc->bPwrOn2PwrGood * 2);
			//USBH_UsrLog("USBH_HUB_ClassRequest: NumPorts=%d, pwrGoodDelay=%d", HUB_Handle->NumPorts, HUB_Handle->pwrGoodDelay);

			HUB_Handle->hubClassRequestPort = 1;
			HUB_Handle->ctl_state = HUB_REQ_SET_POWER;
			status = USBH_BUSY;
		}
		break;

	case HUB_REQ_SET_POWER:
		// Turn on power for each hub port...
		status = set_hub_port_power(phost, HUB_Handle->hubClassRequestPort);
		if (status == USBH_OK)
		{
			// Reach last port
			if (HUB_Handle->NumPorts <= HUB_Handle->hubClassRequestPort)
				HUB_Handle->ctl_state = HUB_WAIT_PWRGOOD;
			else
				HUB_Handle->hubClassRequestPort ++;
			status = USBH_BUSY;
		}
		break;

	case HUB_WAIT_PWRGOOD:
		//HAL_Delay(HUB_Handle->pwrGoodDelay);
		//HUB_Handle->ctl_state = HUB_WAIT_PWRGOOD_DONE;
		USBH_HUB_ProcessDelay(HUB_Handle, HUB_WAIT_PWRGOOD_DONE, HUB_Handle->pwrGoodDelay);
		status = USBH_BUSY;
		break;

	case HUB_WAIT_PWRGOOD_DONE:
		//phost->hubBusy = 0;
		USBH_UsrLog("USBH_HUB_ClassRequest done: NumPorts=%d, pwrGoodDelay=%d", HUB_Handle->NumPorts, HUB_Handle->pwrGoodDelay);
		USBH_UsrLog("=============================================");
		HUB_Handle->hubClassRequestPort = 1;
		HUB_Handle->ctl_state = HUB_REQ_RESETS;
		break;

	case HUB_REQ_RESETS:
		// Выполняем сброс всех портов
		status = set_port_feature(phost, HUB_FEAT_SEL_PORT_RESET, HUB_Handle->hubClassRequestPort);
		if (status == USBH_OK)
		{
			// Reach last port
			if (HUB_Handle->NumPorts <= HUB_Handle->hubClassRequestPort)
			{
				HUB_Handle->ctl_state = HUB_REQ_RESETS_DONE;
				//USBH_HUB_ProcessDelay(HUB_Handle, HUB_REQ_RESETS_DONE, 25);	/* HS устройства не сразу становятся подключенными */
			}
			else
			{
				HUB_Handle->hubClassRequestPort ++;
				HUB_Handle->ctl_state = HUB_REQ_RESETS;
				//USBH_HUB_ProcessDelay(HUB_Handle, HUB_REQ_RESETS, 25);	/* HS устройства не сразу становятся подключенными */
			}
			status = USBH_BUSY;
		}
		else
		{
			USBH_HUB_ProcessDelay(HUB_Handle, HUB_REQ_RESETS, 25);	/* HS устройства не сразу становятся подключенными */
		}
		break;

	case HUB_REQ_RESETS_DONE:
		// Строим карту подключенных портов
		HUB_Handle->hubClassRequestPort = 1;
		HUB_Handle->ctl_state = HUB_REQ_SCAN_STATUSES;
		status = USBH_BUSY;
		break;

	case HUB_REQ_SCAN_STATUSES:
		ASSERT(HUB_Handle->hubClassRequestPort >= 1 && HUB_Handle->hubClassRequestPort <= HUB_Handle->hubClassRequestPort);
		status = get_hub_request(phost, USB_REQUEST_GET_STATUS, HUB_FEAT_SEL_PORT_CONN, HUB_Handle->hubClassRequestPort,
				HUB_Handle->buffer, sizeof(USB_HUB_PORT_STATUS));
		if (status == USBH_OK)
		{
			USBH_TargetTypeDef   * const tg = & HUB_Handle->Targets [HUB_Handle->hubClassRequestPort - 1];	/* Enumeration target */
			ASSERT(HUB_Handle->hubClassRequestPort >= 1 && HUB_Handle->hubClassRequestPort <= HUB_Handle->hubClassRequestPort);

			//printhex(HUB_Handle->buffer, HUB_Handle->buffer, sizeof (USB_HUB_PORT_STATUS));
			USB_HUB_PORT_STATUS * const st = (USB_HUB_PORT_STATUS*) HUB_Handle->buffer;
			// ИНтерпретируем результаты
			//debug_port(HUB_Handle->buffer, st);
			// TODO: если выбрана енумерация LOW SPEED устройста, при установленной HIGH SPEED flash не проходит енумерация.
			if (st->wPortStatus.PORT_ENABLE /* && HUB_Handle->hubClassRequestPort > 1 */)
			{
				if (st->wPortStatus.PORT_LOW_SPEED && 1)
				{
					// LOW SPEED, мышка - нашлась.
					HUB_Handle->detectedPorts += 1;

					tg->tt_hubaddr = phost->currentTarget->dev_address;
					tg->dev_address = USBH_ADDRESS_DEFAULT;
					tg->speed = USBH_SPEED_LOW;
					tg->tt_prtaddr = HUB_Handle->hubClassRequestPort;

					USBH_UsrLog("port %d status val=%04X: conn=%d, ena=%d, pwr=%d, hs=%d, ls=%d - LS device", HUB_Handle->hubClassRequestPort,
							st->wPortStatus.val, st->wPortStatus.PORT_CONNECTION, st->wPortStatus.PORT_ENABLE,
							st->wPortStatus.PORT_POWER, st->wPortStatus.PORT_HIGH_SPEED, st->wPortStatus.PORT_LOW_SPEED);

					phost->currentTarget = tg;
				}
				else if (st->wPortStatus.PORT_HIGH_SPEED && 1)
				{
					// HIGH SPEED, флешка - нашлась.
					HUB_Handle->detectedPorts += 1;

					tg->tt_hubaddr = phost->currentTarget->dev_address;
					tg->dev_address = USBH_ADDRESS_DEFAULT;
					tg->speed = USBH_SPEED_HIGH;
					tg->tt_prtaddr = HUB_Handle->hubClassRequestPort;

					USBH_UsrLog("port %d status val=%04X: conn=%d, ena=%d, pwr=%d, hs=%d, ls=%d - HS device", HUB_Handle->hubClassRequestPort,
							st->wPortStatus.val, st->wPortStatus.PORT_CONNECTION, st->wPortStatus.PORT_ENABLE,
							st->wPortStatus.PORT_POWER, st->wPortStatus.PORT_HIGH_SPEED, st->wPortStatus.PORT_LOW_SPEED);

					phost->currentTarget = tg;
				}
				else if (1 && 1)
				{
					// FULL SPEED
					HUB_Handle->detectedPorts += 1;

					tg->tt_hubaddr = phost->currentTarget->dev_address;
					tg->dev_address = USBH_ADDRESS_DEFAULT;
					tg->speed = USBH_SPEED_FULL;
					tg->tt_prtaddr = HUB_Handle->hubClassRequestPort;

					USBH_UsrLog("port %d status val=%04X: conn=%d, ena=%d, pwr=%d, hs=%d, ls=%d - FS device", HUB_Handle->hubClassRequestPort,
							st->wPortStatus.val, st->wPortStatus.PORT_CONNECTION, st->wPortStatus.PORT_ENABLE,
							st->wPortStatus.PORT_POWER, st->wPortStatus.PORT_HIGH_SPEED, st->wPortStatus.PORT_LOW_SPEED);

					phost->currentTarget = tg;
				}
			}
			else
			{
				USBH_memset(tg, 0, sizeof * tg);

				USBH_UsrLog("port %d status val=%04X: conn=%d, ena=%d, pwr=%d, hs=%d, ls=%d", HUB_Handle->hubClassRequestPort,
						st->wPortStatus.val, st->wPortStatus.PORT_CONNECTION, st->wPortStatus.PORT_ENABLE,
						st->wPortStatus.PORT_POWER, st->wPortStatus.PORT_HIGH_SPEED, st->wPortStatus.PORT_LOW_SPEED);
			}

			// Reach last port
			if (HUB_Handle->NumPorts <= HUB_Handle->hubClassRequestPort)
			{
				// выходим из цикла
				HUB_Handle->ctl_state = HUB_REQ_SCAN_STATUSES_DONE;
				status = USBH_BUSY;

			}
			else
			{
				HUB_Handle->hubClassRequestPort ++;
				status = USBH_BUSY;
			}

		}
		else if (status == USBH_BUSY)
		{

		}
		else
		{
			// выходим по ошибке
			HUB_Handle->ctl_state = HUB_REQ_IDLE;
			status = USBH_OK;
		}
		break;

	case HUB_REQ_SCAN_STATUSES_DONE:
		USBH_UsrLog("=============================================");

		if (HUB_Handle->detectedPorts != 1)
		{
			USBH_UsrLog("Wrong count (%d) USB devices on HUB. Only one supported", (int) HUB_Handle->detectedPorts);
			return USBH_OK;
		}

        /* free Interrupt pipe */
		if (HUB_Handle->InPipe != 0x00)
		{
			USBH_ClosePipe (phost, HUB_Handle->InPipe);
			USBH_FreePipe  (phost, HUB_Handle->InPipe);
			HUB_Handle->InPipe = 0;     // Reset the pipe as Free
		}

        /* free control pipes */
		USBH_ClosePipe (phost, phost->Control.pipe_out);
		(void)USBH_FreePipe(phost, phost->Control.pipe_out);
		USBH_ClosePipe (phost, phost->Control.pipe_in);
		(void)USBH_FreePipe(phost, phost->Control.pipe_in);

		HUB_Handle->ctl_state = HUB_ALREADY_INITED;
		status = USBH_HUB_REQ_REENUMERATE;
		break;

	case HUB_DELAY:
		if  ((sys_now() - HUB_Handle->tickstart) >= HUB_Handle->wait)
			HUB_Handle->ctl_state = HUB_Handle->ctl_state_push;
		status = USBH_BUSY;
		break;

	default:
		status = USBH_OK;
		break;
	}

	return status;
}


static USBH_StatusTypeDef USBH_HUB_Process(USBH_HandleTypeDef *phost)
{
	USBH_StatusTypeDef status = USBH_BUSY;	/* не требуется, но по стилю = чтобы продолжались вызовы */
	HUB_HandleTypeDef * const HUB_Handle = phost->hubDatas [0];
	//ASSERT(HUB_Handle != NULL);
//
//    switch (HUB_Handle->state)
//    {
//     	case HUB_IDLE:
//     		HUB_Handle->HUB_CurPort = 0;
//     		HUB_Handle->state = HUB_SYNC;
//     		break;
//
//     	case HUB_SYNC:
//     	    /* Sync with start of Even Frame */
//     	    if ((phost->Timer & 0x01) != 0)
//     	    {
//     	    	HUB_Handle->state = HUB_GET_DATA;
//     	    }
//     		break;
//
//    	case HUB_GET_DATA:
////			if(hUSBHost[1].busy)
////				break;
//
//    	    (void) USBH_InterruptReceiveData(phost, HUB_Handle->buffer, HUB_Handle->length, HUB_Handle->InPipe);
//    	    HUB_Handle->state = HUB_POLL;
//    	    //HUB_Handle->state = HUB_PORT_CHANGED;
//    	    HUB_Handle->timer = phost->Timer;
//    	    HUB_Handle->DataReady = 0;
//    		break;
//
//    	case HUB_POLL:
//			{
//	    		const USBH_URBStateTypeDef urbState = USBH_LL_GetURBState(phost, HUB_Handle->InPipe);
//	    	    if (urbState == USBH_URB_DONE)
//	    	    {
//	    			USBH_UsrLog("USBH_HUB_Process: HUB_POLL, HUB_CurPort=%d, answer=%02X,%02X (len=%d)", (int) HUB_Handle->HUB_CurPort, HUB_Handle->buffer [0], HUB_Handle->buffer [1], HUB_Handle->length);
//	    	    	if(HUB_Handle->DataReady == 0)
//	    	    	{
//	    	    		HUB_Handle->DataReady = 1;
//	    	    		if (port_changed(HUB_Handle, HUB_Handle->buffer, HUB_Handle->length))
//	    		        	HUB_Handle->state = HUB_LOOP_PORT_CHANGED;
//	    		        else
//	    		        	HUB_Handle->state = HUB_GET_DATA;
//	    	    	}
//	    	    }
//	    		else if (urbState == USBH_URB_STALL) /* IN Endpoint Stalled */
//	    		{
//	    			// Issue Clear Feature on interrupt IN endpoint
//	    			if( (USBH_ClrFeature(phost, HUB_Handle->ep_addr)) == USBH_OK)
//	    			{
//	    				// Change state to issue next IN token
//	    				HUB_Handle->state = HUB_GET_DATA;
//	    			}
//	    		}
//	    		else if(urbState == USBH_URB_ERROR)
//	    		{
//	//LOG1("=");
//	    		}
//	    		else
//	    		{
//	    			// URB_IDLE
//	    		}
//
//			}
//    		break;
//
//    	case HUB_LOOP_PORT_CHANGED:
//    		HUB_Handle->HUB_CurPort = get_port_changed(HUB_Handle);
//        	if (HUB_Handle->HUB_CurPort > 0)
//        	{
////USBH_UsrLog("HUB_CurPort %d", HUB_CurPort);
//
////phost->hubBusy = 1;
//				clear_port_changed(HUB_Handle, HUB_Handle->HUB_CurPort);
//        		HUB_Handle->state = HUB_PORT_CHANGED;
//        	}
//        	else
//        		HUB_Handle->state = HUB_IDLE;
//
//        	break;
//
//    	case HUB_PORT_CHANGED:
//     		// uses EP0
//    		if (get_hub_request(phost, USB_REQUEST_GET_STATUS, HUB_FEAT_SEL_PORT_CONN, HUB_Handle->HUB_CurPort, HUB_Handle->buffer, sizeof (USB_HUB_PORT_STATUS)) == USBH_OK)
//    		{
//     			HUB_Handle->pChangeInfo = (USB_HUB_PORT_STATUS *) HUB_Handle->buffer;
//
//    			debug_port(HUB_Handle->buffer, HUB_Handle->pChangeInfo);
//
//                if (HUB_Handle->pChangeInfo->wPortStatus.PORT_POWER)
//                {
//					if (HUB_Handle->pChangeInfo->wPortChange.C_PORT_OVER_CURRENT)
//					{
//						HUB_Handle->state = HUB_C_PORT_OVER_CURRENT;
//						break;
//					}
//
//					if (HUB_Handle->pChangeInfo->wPortChange.C_PORT_SUSPEND)
//					{
//						HUB_Handle->state = HUB_C_PORT_SUSPEND;
//						break;
//					}
//
//
//                    if (HUB_Handle->pChangeInfo->wPortChange.C_PORT_CONNECTION)
//                    {
//                    	HUB_Handle->state = HUB_C_PORT_CONNECTION;
//                    }
//                    else
//                    {
//                    	if (HUB_Handle->pChangeInfo->wPortStatus.PORT_CONNECTION)
//                    	{
//                    		if (HUB_Handle->pChangeInfo->wPortStatus.PORT_RESET)
//                    		{
//                    			HUB_Handle->state = HUB_PORT_CHANGED;
//                    		}
//							else if (HUB_Handle->pChangeInfo->wPortChange.C_PORT_RESET)
//							{
//								HUB_Handle->state = HUB_C_PORT_RESET;
//							}
//							else if (HUB_Handle->pChangeInfo->wPortStatus.PORT_ENABLE)
//							{
//								// Device Attached
///*								if(pChangeInfo->wPortStatus.PORT_LOW_SPEED)
//								{
//									//phost->hubBusy = 0;
//									HUB_Handle->state = HUB_LOOP_PORT_CHANGED;
//								}
//								else*/
//									HUB_Handle->state = HUB_DEV_ATTACHED;
//							}
//							else
//							{
//								HUB_Handle->state = HUB_RESET_DEVICE;
//							}
//                    	}
//                    	else
//                    	{
//                    		// Device Detached
//                    		HUB_Handle->state = HUB_DEV_DETACHED;
//                    	}
//                    }
//                }
//    		}
//    		break;
//
//    	case HUB_C_PORT_SUSPEND:
//    		if(clear_port_feature(phost, HUB_FEAT_SEL_C_PORT_SUSPEND, HUB_Handle->HUB_CurPort) == USBH_OK)
//            {
//            	HUB_Handle->state = HUB_PORT_CHANGED;
//            }
//
//    		break;
//
//    	case HUB_C_PORT_OVER_CURRENT:
//            if(clear_port_feature(phost, HUB_FEAT_SEL_C_PORT_OVER_CURRENT, HUB_Handle->HUB_CurPort) == USBH_OK)
//            {
//            	HUB_Handle->state = HUB_PORT_CHANGED;
//            }
//            break;
//
//    	case HUB_C_PORT_CONNECTION:
//            if(clear_port_feature(phost, HUB_FEAT_SEL_C_PORT_CONNECTION, HUB_Handle->HUB_CurPort) == USBH_OK)
//            {
//            	HUB_Handle->state = HUB_PORT_CHANGED;
//            }
//    		break;
//
//    	case HUB_C_PORT_RESET:
//            if(clear_port_feature(phost, HUB_FEAT_SEL_C_PORT_RESET, HUB_Handle->HUB_CurPort) == USBH_OK)
//            {
//            	HUB_Handle->state = HUB_PORT_CHANGED;
//            }
//    		break;
//
//    	case HUB_RESET_DEVICE:
//            if(set_port_feature(phost, HUB_FEAT_SEL_PORT_RESET, HUB_Handle->HUB_CurPort) == USBH_OK)
//            {
//            	USBH_Delay(150);
//            	HUB_Handle->state = HUB_PORT_CHANGED;
//            }
//    		break;
//
//    	case HUB_DEV_ATTACHED:
//    		USBH_UsrLog("HUB_DEV_ATTACHED %d, lowspeed? %d", HUB_Handle->HUB_CurPort, HUB_Handle->pChangeInfo->wPortStatus.PORT_LOW_SPEED);
//
//			HUB_Handle->state = HUB_LOOP_PORT_WAIT;
//			attach(phost, HUB_Handle->HUB_CurPort, HUB_Handle->pChangeInfo->wPortStatus.PORT_LOW_SPEED);
//			//phost->hubBusy = 0;
//    		break;
//
//    	case HUB_DEV_DETACHED:
//    		USBH_UsrLog("HUB_DEV_DETACHED %d", HUB_Handle->HUB_CurPort);
//
//			HUB_Handle->state = HUB_LOOP_PORT_WAIT;
//			detach(phost, HUB_Handle->HUB_CurPort);
//			//phost->hubBusy = 0;
//    		break;
//
//    	case HUB_LOOP_PORT_WAIT:
//    		USBH_Delay(10);
//    		HUB_Handle->state = HUB_LOOP_PORT_CHANGED;
//    		break;
//
//    	case HUB_ERROR:
//    	default:
//    		break;
//    }

	return status;
}

static USBH_StatusTypeDef USBH_HUB_SOFProcess(USBH_HandleTypeDef *phost)
{
	HUB_HandleTypeDef * const HUB_Handle = phost->hubDatas [0];
/*if(!phost->hub)
{
USBH_UsrLog("EEEERRRRRRROOORRRRRRR");
return USBH_OK;
}*/

//if(HUB_Handle->poll != 255)
//USBH_UsrLog("ERR %d %d", HUB_Handle->poll, HUB_Handle->length);
//
//	if(HUB_Handle->state == HUB_POLL)
//	{
//		if((phost->Timer - HUB_Handle->timer) >= HUB_Handle->poll)
//	    {
//			HUB_Handle->state = HUB_GET_DATA;
//	    }
//	}

	return USBH_OK;
}

USBH_ClassTypeDef  HUB_Class =
{
	"HUB",
	USB_HUB_CLASS,
	USBH_HUB_InterfaceInit,
	USBH_HUB_InterfaceDeInit,
	USBH_HUB_ClassRequest,
	USBH_HUB_Process,
	USBH_HUB_SOFProcess,
	NULL,
};
