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

#include "hardware.h"
#include "formats.h"


#include "../Inc/usbh_hub.h"
#include "../../HID/Inc/usbh_hid.h"

//#include "usbh_msc.h"
//#include "stm32f4xx_hal.h"
//#include "stm32f4xx_hal_hcd.h"

static USBH_StatusTypeDef get_hub_descriptor(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef set_hub_port_power(USBH_HandleTypeDef *phost, uint8_t hub_port);
static USBH_StatusTypeDef hub_request(USBH_HandleTypeDef *phost, uint8_t request, uint8_t feature, uint8_t dataDirection, uint8_t porta, uint8_t *buffer, uint16_t size);
static USBH_StatusTypeDef set_hub_request(USBH_HandleTypeDef *phost, uint8_t request, uint8_t feature, uint8_t port);
static USBH_StatusTypeDef get_hub_request(USBH_HandleTypeDef *phost, uint8_t request, uint8_t feature, uint8_t porta, uint8_t *buffer, uint16_t size);
static USBH_StatusTypeDef clear_port_feature(USBH_HandleTypeDef *phost, uint8_t feature, uint8_t port);
static USBH_StatusTypeDef set_port_feature(USBH_HandleTypeDef *phost, uint8_t feature, uint8_t port);
static void clear_port_changed(uint8_t port);
static uint8_t get_port_changed(void);
static uint8_t port_changed(uint8_t *b);
static void detach(USBH_HandleTypeDef *phost, uint16_t idx);
static void attach(USBH_HandleTypeDef *phost, uint16_t idx, uint8_t lowspeed);
static void debug_port(uint8_t *buff, __IO USB_HUB_PORT_STATUS *info);

static USBH_StatusTypeDef USBH_HUB_InterfaceInit  (USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_HUB_InterfaceDeInit  (USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_HUB_ClassRequest(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_HUB_Process(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_HUB_SOFProcess(USBH_HandleTypeDef *phost);

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

static __IO USB_PORT_CHANGE HUB_Change;
static __IO uint8_t HUB_CurPort = 0;
static __IO USB_HUB_PORT_STATUS *HUB_ChangeInfo = 0;

static uint8_t  HUB_NumPorts = 0;
static uint16_t HUB_PwrGood  = 0;

static USBH_StatusTypeDef USBH_HUB_InterfaceInit (USBH_HandleTypeDef *phost)
{
	uint8_t interface;

	USBH_StatusTypeDef status = USBH_FAIL ;
	HUB_HandleTypeDef *HUB_Handle;

	HUB_NumPorts = 0;
	HUB_PwrGood = 0;
	HUB_ChangeInfo = 0;
	HUB_CurPort = 0;

	int h = 1;
	for(; h < ARRAY_SIZE(hUSBHost); ++h)
	{
		memset(&hUSBHost[h], 0, sizeof(USBH_HandleTypeDef));
	}

	interface = USBH_FindInterface(phost, phost->pActiveClass->ClassCode, 0x00, 0x00);

	if(interface == 0xFF) /* No Valid Interface */
	{
		status = USBH_FAIL;
		USBH_DbgLog ("Cannot Find the interface for %s class.", phost->pActiveClass->Name);
	}
	else
	{
		phost->USBH_ClassTypeDef_pData[0] = (HUB_HandleTypeDef *)USBH_malloc(sizeof(HUB_HandleTypeDef));
		HUB_Handle = phost->USBH_ClassTypeDef_pData[0];
		memset(HUB_Handle, 0, sizeof(HUB_HandleTypeDef));

		phost->interfaces = 1;

		USBH_SelectInterface (phost, interface);

	    HUB_Handle->state     = HUB_IDLE;
	    HUB_Handle->ctl_state = HUB_REQ_IDLE;
	    HUB_Handle->ep_addr   = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bEndpointAddress;
	    HUB_Handle->length    = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].wMaxPacketSize;
	    HUB_Handle->poll      = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bInterval;

	    if (HUB_Handle->poll  < HUB_MIN_POLL)
	    	HUB_Handle->poll = HUB_MIN_POLL;

USBH_UsrLog ("device POLL %d, LEN %d", HUB_Handle->poll, HUB_Handle->length);

	    if(phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bEndpointAddress & 0x80)
	    {
	    	HUB_Handle->InEp = (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bEndpointAddress);
	    	HUB_Handle->InPipe  = USBH_AllocPipe(phost, HUB_Handle->InEp);

	    	// Open pipe for IN endpoint
	    	USBH_OpenPipe  (phost, HUB_Handle->InPipe, HUB_Handle->InEp, phost->device.address, phost->device.speed, USB_EP_TYPE_INTR, HUB_Handle->length);

	    	USBH_LL_SetToggle (phost, HUB_Handle->InPipe, 0);
	    }

	    phost->hub = 1;

	    status = USBH_OK;
	}

	return status;
}

static USBH_StatusTypeDef USBH_HUB_InterfaceDeInit (USBH_HandleTypeDef *phost )
{
	USBH_UsrLog("USBH_HUB_InterfaceDeInit %d", phost->address);

	HUB_HandleTypeDef *HUB_Handle = phost->USBH_ClassTypeDef_pData[0];

	if(HUB_Handle->InPipe != 0x00)
	{
		USBH_ClosePipe (phost, HUB_Handle->InPipe);
		USBH_FreePipe  (phost, HUB_Handle->InPipe);
		HUB_Handle->InPipe = 0;     // Reset the pipe as Free
	}

	if(phost->USBH_ClassTypeDef_pData[0])
		USBH_free (phost->USBH_ClassTypeDef_pData[0]);

	return USBH_OK;
}

static USBH_StatusTypeDef USBH_HUB_ClassRequest(USBH_HandleTypeDef *phost)
{
	USBH_StatusTypeDef status = USBH_BUSY;
	HUB_HandleTypeDef *HUB_Handle = phost->USBH_ClassTypeDef_pData[0];

    static uint8_t  port = 1;

    switch (HUB_Handle->ctl_state)
    {
    	case HUB_REQ_IDLE:
    	case HUB_REQ_GET_DESCRIPTOR:
    		port = 1;
      		if(get_hub_descriptor(phost) == USBH_OK)
    			HUB_Handle->ctl_state = HUB_REQ_SET_POWER;
    		break;

    	case HUB_REQ_SET_POWER:
    		// Turn on power for each hub port...
    		if(set_hub_port_power(phost, port) == USBH_OK)
    		{
    			// Reach last port
    			if(HUB_NumPorts == port)
    				HUB_Handle->ctl_state = HUB_WAIT_PWRGOOD;
    			else
    				port++;
    		}
    		break;

    	case HUB_WAIT_PWRGOOD:
    		USBH_Delay(HUB_PwrGood);
    		HUB_Handle->ctl_state = HUB_REQ_DONE;
    		break;

    	case HUB_REQ_DONE:
    		phost->busy = 0;
    		USBH_UsrLog("%d HUB PORTS ENABLED", HUB_NumPorts);
    		USBH_UsrLog("=============================================");
    		status = USBH_OK;
    		break;
    }

    return status;
}


static USBH_StatusTypeDef USBH_HUB_Process(USBH_HandleTypeDef *phost)
{
	USBH_StatusTypeDef status = USBH_OK;
	HUB_HandleTypeDef *HUB_Handle =  (HUB_HandleTypeDef *)phost->USBH_ClassTypeDef_pData[0];

    switch(HUB_Handle->state)
    {
     	case HUB_IDLE:
     		HUB_CurPort = 0;
     		HUB_Handle->state = HUB_SYNC;
     		break;

     	case HUB_SYNC:
     	    /* Sync with start of Even Frame */
     	    if ((phost->Timer & 0x01) != 0)
     	    {
     	    	HUB_Handle->state = HUB_GET_DATA;
     	    }
     		break;

    	case HUB_GET_DATA:
			if(hUSBHost[1].busy)
				break;

    	    USBH_InterruptReceiveData(phost, HUB_Handle->buffer, HUB_Handle->length, HUB_Handle->InPipe);
    	    HUB_Handle->state = HUB_POLL;
    	    HUB_Handle->timer = phost->Timer;
    	    HUB_Handle->DataReady = 0;
    		break;

    	case HUB_POLL:
    	    if(USBH_LL_GetURBState(phost, HUB_Handle->InPipe) == USBH_URB_DONE)
    	    {
//LOG1("^");
    	    	if(HUB_Handle->DataReady == 0)
    	    	{
    	    		HUB_Handle->DataReady = 1;
    	    		if(port_changed(HUB_Handle->buffer))
    		        	HUB_Handle->state = HUB_LOOP_PORT_CHANGED;
    		        else
    		        	HUB_Handle->state = HUB_GET_DATA;
    	    	}
    	    }
    		else if(USBH_LL_GetURBState(phost , HUB_Handle->InPipe) == USBH_URB_STALL) /* IN Endpoint Stalled */
    		{
    			// Issue Clear Feature on interrupt IN endpoint
    			if( (USBH_ClrFeature(phost, HUB_Handle->ep_addr)) == USBH_OK)
    			{
    				// Change state to issue next IN token
    				HUB_Handle->state = HUB_GET_DATA;
    			}
    		}
    		else if(USBH_LL_GetURBState(phost , HUB_Handle->InPipe) == USBH_URB_ERROR)
    		{
//LOG1("=");
    		}
    		break;

    	case HUB_LOOP_PORT_CHANGED:
        	HUB_CurPort = get_port_changed();
        	if(HUB_CurPort > 0)
        	{
//USBH_UsrLog("HUB_CurPort %d", HUB_CurPort);

phost->busy = 1;
				clear_port_changed(HUB_CurPort);
        		HUB_Handle->state = HUB_PORT_CHANGED;
        	}
        	else
        		HUB_Handle->state = HUB_IDLE;

        	break;

    	case HUB_PORT_CHANGED:

    		// uses EP0
    		if(get_hub_request(phost, USB_REQUEST_GET_STATUS, HUB_FEAT_SEL_PORT_CONN, HUB_CurPort, HUB_Handle->buffer, sizeof(USB_HUB_PORT_STATUS)) == USBH_OK)
    		{
    			HUB_ChangeInfo = (USB_HUB_PORT_STATUS *)HUB_Handle->buffer;

debug_port(HUB_Handle->buffer, HUB_ChangeInfo);

                if(HUB_ChangeInfo->wPortStatus.PORT_POWER)
                {
					if(HUB_ChangeInfo->wPortChange.C_PORT_OVER_CURRENT)
					{
						HUB_Handle->state = HUB_C_PORT_OVER_CURRENT;
						break;
					}

					if(HUB_ChangeInfo->wPortChange.C_PORT_SUSPEND)
					{
						HUB_Handle->state = HUB_C_PORT_SUSPEND;
						break;
					}


                    if(HUB_ChangeInfo->wPortChange.C_PORT_CONNECTION)
                    {
                    	HUB_Handle->state = HUB_C_PORT_CONNECTION;
                    }
                    else
                    {
                    	if(HUB_ChangeInfo->wPortStatus.PORT_CONNECTION)
                    	{
                    		if(HUB_ChangeInfo->wPortStatus.PORT_RESET)
                    		{
                    			HUB_Handle->state = HUB_PORT_CHANGED;
                    		}
							else if(HUB_ChangeInfo->wPortChange.C_PORT_RESET)
							{
								HUB_Handle->state = HUB_C_PORT_RESET;
							}
							else if(HUB_ChangeInfo->wPortStatus.PORT_ENABLE)
							{
								// Device Attached
/*								if(HUB_ChangeInfo->wPortStatus.PORT_LOW_SPEED)
								{
									phost->busy = 0;
									HUB_Handle->state = HUB_LOOP_PORT_CHANGED;
								}
								else*/
									HUB_Handle->state = HUB_DEV_ATTACHED;
							}
							else
							{
								HUB_Handle->state = HUB_RESET_DEVICE;
							}
                    	}
                    	else
                    	{
                    		// Device Detached
                    		HUB_Handle->state = HUB_DEV_DETACHED;
                    	}
                    }
                }
    		}
    		break;

    	case HUB_C_PORT_SUSPEND:
    		if(clear_port_feature(phost, HUB_FEAT_SEL_C_PORT_SUSPEND, HUB_CurPort) == USBH_OK)
            {
            	HUB_Handle->state = HUB_PORT_CHANGED;
            }

    		break;

    	case HUB_C_PORT_OVER_CURRENT:
            if(clear_port_feature(phost, HUB_FEAT_SEL_C_PORT_OVER_CURRENT, HUB_CurPort) == USBH_OK)
            {
            	HUB_Handle->state = HUB_PORT_CHANGED;
            }
            break;

    	case HUB_C_PORT_CONNECTION:
            if(clear_port_feature(phost, HUB_FEAT_SEL_C_PORT_CONNECTION, HUB_CurPort) == USBH_OK)
            {
            	HUB_Handle->state = HUB_PORT_CHANGED;
            }
    		break;

    	case HUB_C_PORT_RESET:
            if(clear_port_feature(phost, HUB_FEAT_SEL_C_PORT_RESET, HUB_CurPort) == USBH_OK)
            {
            	HUB_Handle->state = HUB_PORT_CHANGED;
            }
    		break;

    	case HUB_RESET_DEVICE:
            if(set_port_feature(phost, HUB_FEAT_SEL_PORT_RESET, HUB_CurPort) == USBH_OK)
            {
            	USBH_Delay(150);
            	HUB_Handle->state = HUB_PORT_CHANGED;
            }
    		break;

    	case HUB_DEV_ATTACHED:
USBH_UsrLog("HUB_DEV_ATTACHED %d, lowspeed? %d", HUB_CurPort, HUB_ChangeInfo->wPortStatus.PORT_LOW_SPEED);

			HUB_Handle->state = HUB_LOOP_PORT_WAIT;
			attach(phost, HUB_CurPort, HUB_ChangeInfo->wPortStatus.PORT_LOW_SPEED);
			phost->busy = 0;
    		break;

    	case HUB_DEV_DETACHED:
USBH_UsrLog("HUB_DEV_DETACHED %d", HUB_CurPort);

			HUB_Handle->state = HUB_LOOP_PORT_WAIT;
			detach(phost, HUB_CurPort);
			phost->busy = 0;
    		break;

    	case HUB_LOOP_PORT_WAIT:
    		USBH_Delay(10);
    		HUB_Handle->state = HUB_LOOP_PORT_CHANGED;
    		break;

    	case HUB_ERROR:
    	default:
    		break;
    }

	return status;
}

static USBH_StatusTypeDef USBH_HUB_SOFProcess(USBH_HandleTypeDef *phost)
{
/*if(!phost->hub)
{
USBH_UsrLog("EEEERRRRRRROOORRRRRRR");
return USBH_OK;
}*/

	HUB_HandleTypeDef *HUB_Handle = (HUB_HandleTypeDef *)phost->USBH_ClassTypeDef_pData[0];

//if(HUB_Handle->poll != 255)
//USBH_UsrLog("ERR %d %d", HUB_Handle->poll, HUB_Handle->length);

	if(HUB_Handle->state == HUB_POLL)
	{
		if((phost->Timer - HUB_Handle->timer) >= HUB_Handle->poll)
	    {
			HUB_Handle->state = HUB_GET_DATA;
	    }
	}

	return USBH_OK;
}

static USBH_StatusTypeDef get_hub_descriptor(USBH_HandleTypeDef *phost)
{
	USBH_StatusTypeDef status = USBH_BUSY;
	static uint8_t state = 0;

	HUB_HandleTypeDef *HUB_Handle = phost->USBH_ClassTypeDef_pData[0];

	switch(state)
	{
	case 0:
		phost->Control.setup.b.bmRequestType = USB_D2H|USB_REQ_RECIPIENT_DEVICE|USB_REQ_TYPE_CLASS;
		phost->Control.setup.b.bRequest  	 = USB_REQ_GET_DESCRIPTOR;
		phost->Control.setup.b.wValue.bw.msb = 0;
		phost->Control.setup.b.wValue.bw.lsb = USB_DESCRIPTOR_HUB;
		phost->Control.setup.b.wIndex.w  	 = 0;
		phost->Control.setup.b.wLength.w 	 = sizeof(USB_HUB_DESCRIPTOR);

		if(USBH_CtlReq(phost, HUB_Handle->buffer, sizeof(USB_HUB_DESCRIPTOR)) == USBH_OK)
			state = 1;
		break;

	case 1:
		{
			USB_HUB_DESCRIPTOR  *HUB_Desc = (USB_HUB_DESCRIPTOR *) HUB_Handle->buffer;
			HUB_NumPorts = (HUB_Desc->bNbrPorts > MAX_HUB_PORTS) ? MAX_HUB_PORTS : HUB_Desc->bNbrPorts;
			HUB_PwrGood  = (HUB_Desc->bPwrOn2PwrGood * 2);
			state = 0;
			status = USBH_OK;
		}
		break;
	}

	return status;
}

static USBH_StatusTypeDef hub_request(USBH_HandleTypeDef *phost, uint8_t request, uint8_t feature, uint8_t dataDirection, uint8_t porta, uint8_t *buffer, uint16_t size)
{
	uint8_t bmRequestType = (dataDirection == USB_DEVICE_REQUEST_GET) ? USB_D2H : USB_H2D; // 0x23

	phost->Control.setup.b.bmRequestType = bmRequestType|USB_REQ_RECIPIENT_OTHER|USB_REQ_TYPE_CLASS;
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

static USBH_StatusTypeDef set_hub_port_power(USBH_HandleTypeDef *phost, uint8_t hub_port)
{
	return hub_request(phost, USB_REQ_SET_FEATURE, HUB_FEATURE_SEL_PORT_POWER, USB_DEVICE_REQUEST_SET, hub_port, 0, 0);
}

static USBH_StatusTypeDef get_hub_request(USBH_HandleTypeDef *phost, uint8_t request, uint8_t feature, uint8_t porta, uint8_t *buffer, uint16_t size)
{
    return hub_request(phost, request, feature, USB_DEVICE_REQUEST_GET, porta, buffer, size);
}

static USBH_StatusTypeDef set_hub_request(USBH_HandleTypeDef *phost, uint8_t request, uint8_t feature, uint8_t port)
{
    return hub_request(phost, request, feature, USB_DEVICE_REQUEST_SET, port, 0, 0);
}

static USBH_StatusTypeDef clear_port_feature(USBH_HandleTypeDef *phost, uint8_t feature, uint8_t port)
{
    return set_hub_request(phost, USB_REQUEST_CLEAR_FEATURE, feature, port);
}

static USBH_StatusTypeDef set_port_feature(USBH_HandleTypeDef *phost, uint8_t feature, uint8_t port)
{
    return set_hub_request(phost, USB_REQUEST_SET_FEATURE, feature, port);
}

static void clear_port_changed(uint8_t port)
{
	switch(port)
	{
		case 1:	HUB_Change.bPorts.PORT_1 = 0; break;
		case 2: HUB_Change.bPorts.PORT_2 = 0; break;
		case 3: HUB_Change.bPorts.PORT_3 = 0; break;
		case 4: HUB_Change.bPorts.PORT_4 = 0; break;
	}
}

static uint8_t get_port_changed()
{
	if(HUB_Change.bPorts.PORT_1)	return 1;
	if(HUB_Change.bPorts.PORT_2)	return 2;
	if(HUB_Change.bPorts.PORT_3)	return 3;
	if(HUB_Change.bPorts.PORT_4)	return 4;

	return 0;
}

static uint8_t port_changed(uint8_t *b)
{
    if(b[0] != 0x00)
    {
    	HUB_Change.val = 0x00;

        if(b[0] & (1<<1)) { HUB_Change.bPorts.PORT_1 = 1; }
        if(b[0] & (1<<2)) { HUB_Change.bPorts.PORT_2 = 1; }
        if(b[0] & (1<<3)) { HUB_Change.bPorts.PORT_3 = 1; }
        if(b[0] & (1<<4)) { HUB_Change.bPorts.PORT_4 = 1; }

//USBH_UsrLog("PORT STATUS CHANGE [0x%02X] [%d %d %d %d]", b[0], HUB_Change.bPorts.PORT_1, HUB_Change.bPorts.PORT_2, HUB_Change.bPorts.PORT_3, HUB_Change.bPorts.PORT_4);

        return HUB_Change.val > 0;
    }

    return 0;
}

void detach(USBH_HandleTypeDef *_phost, uint16_t idx)
{
	USBH_HandleTypeDef *pphost = &hUSBHost[idx];
	if(pphost->valid)
	{
USBH_UsrLog("detach %d", pphost->address);

		if(pphost->pUser != NULL)
		{
			pphost->pUser(pphost, HOST_USER_DISCONNECTION);
		}

	    if(pphost->pActiveClass != NULL)
	    {
	    	pphost->pActiveClass->DeInit(pphost);
	    	pphost->pActiveClass = NULL;
	    }

		pphost->address	    = 0;
		pphost->busy  	    = 0;
		pphost->ClassNumber = 0;
		pphost->valid 	    = 0;

		uint32_t i;
		for(i = 0; i< USBH_MAX_DATA_BUFFER; i++)
		{
			pphost->device.Data[i] = 0;
		}

		for(i = 0; i < pphost->interfaces; ++i)
		{
			USBH_free(pphost->USBH_ClassTypeDef_pData[i]);
			pphost->USBH_ClassTypeDef_pData[i] = NULL;
		}

		pphost->interfaces = 0;
		pphost->device.is_connected = 0;
	}
}

static void attach(USBH_HandleTypeDef *phost, uint16_t idx, uint8_t lowspeed)
{
	USBH_HandleTypeDef *pphost = &hUSBHost[idx];
USBH_UsrLog("attach %d", idx);

	if(pphost->valid)
	{
		USBH_UsrLog("ATTACH ERROR, ALREADY ATTACHED");
		detach(pphost, idx);
	}

	pphost->id 					= hUSBHost[0].id;
	pphost->address 			= idx;
	pphost->hub 				= 0;
	pphost->pActiveClass 		= NULL;
	pphost->ClassNumber 		= 0;

#warning Then use HUB class. investigane Pipes usage.
// Taken from https://github.com/mori-br/STM32F4HUB
	//pphost->Pipes 				= phost->Pipes;
	memcpy(pphost->Pipes, phost->Pipes, sizeof pphost->Pipes);

    pphost->pUser 				= phost->pUser;
	pphost->EnumState 			= ENUM_IDLE;
	pphost->RequestState 		= CMD_SEND;
	pphost->Timer 				= 0;
	pphost->Control.errorcount 	= 0;
	pphost->Control.state 		= CTRL_SETUP;
	pphost->Control.pipe_size 	= lowspeed ? USBH_MPS_LOWSPEED: USBH_MPS_DEFAULT;
	pphost->device.address 		= USBH_ADDRESS_DEFAULT;
	pphost->device.speed   		= lowspeed ? USBH_SPEED_LOW : USBH_SPEED_FULL;
	pphost->device.is_connected = 1;

	HCD_HandleTypeDef *phHCD =  &_hHCD[pphost->id];
	USBH_LL_SetTimer (pphost, HAL_HCD_GetCurrentFrame(phHCD));

	/* link the class tgo the USB Host handle */
    pphost->pClass[pphost->ClassNumber++] = USBH_HID_CLASS;
//    pphost->pClass[pphost->ClassNumber++] = USBH_MSC_CLASS;

    pphost->gState = HOST_ENUMERATION;

	pphost->Control.pipe_out = phost->Control.pipe_out;
	pphost->Control.pipe_in  = phost->Control.pipe_in;

	uint8_t i = 0;
	for(;i < USBH_MAX_NUM_INTERFACES; ++i)
		pphost->USBH_ClassTypeDef_pData[i] = NULL;

	pphost->interfaces = 0;
	pphost->busy  = 0;
	pphost->valid = 3;

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
