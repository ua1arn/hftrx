/**
  ******************************************************************************
  * @file    usbh_core.c
  * @author  MCD Application Team
  * @brief   This file implements the functions for the core state machine process
  *          the enumeration and the control transfer process
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"


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
#define USBH_ADDRESS_DEFAULT                     0x00U
#define USBH_ADDRESS_ASSIGNED                    0x01U
#define USBH_MPS_DEFAULT                         0x40U
/**
  * @}
  */

/** @defgroup USBH_CORE_Private_Macros
  * @{
  */
/**
  * @}
  */


/** @defgroup USBH_CORE_Private_Variables
  * @{
  */
#if (USBH_USE_OS == 1U)
#if (osCMSIS >= 0x20000U)
osThreadAttr_t USBH_Thread_Atrr;
#endif
#endif


/**
  * @}
  */


/** @defgroup USBH_CORE_Private_Functions
  * @{
  */
static USBH_StatusTypeDef USBH_HandleEnum(USBH_HandleTypeDef *phost);
static void USBH_HandleSof(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef DeInitStateMachine(USBH_HandleTypeDef *phost);

#if (USBH_USE_OS == 1U)
#if (osCMSIS < 0x20000U)
static void USBH_Process_OS(void const *argument);
#else
static void USBH_Process_OS(void *argument);
#endif
#endif


/**
  * @brief  HCD_Init
  *         Initialize the HOST Core.
  * @param  phost: Host Handle
  * @param  pUsrFunc: User Callback
  * @retval USBH Status
  */
USBH_StatusTypeDef  USBH_Init(USBH_HandleTypeDef *phost,
                              void (*pUsrFunc)(USBH_HandleTypeDef *phost,
                                               uint8_t id), uint8_t id)
{
  /* Check whether the USB Host handle is valid */
  if (phost == NULL)
  {
    USBH_ErrLog("Invalid Host handle");
    return USBH_FAIL;
  }

  /* Set DRiver ID */
  phost->id = id;

  /* Unlink class*/
  phost->pActiveClass = NULL;
  phost->ClassNumber = 0U;

  /* Restore default states and prepare EP0 */
  (void)DeInitStateMachine(phost);

  /* Restore default Device connection states */
  phost->device.PortEnabled = 0U;
  phost->device.is_connected = 0U;
  phost->device.is_disconnected = 0U;
  phost->device.is_ReEnumerated = 0U;

  /* Assign User process */
  if (pUsrFunc != NULL)
  {
    phost->pUser = pUsrFunc;
  }

#if (USBH_USE_OS == 1U)
#if (osCMSIS < 0x20000U)

  /* Create USB Host Queue */
  osMessageQDef(USBH_Queue, MSGQUEUE_OBJECTS, uint16_t);
  phost->os_event = osMessageCreate(osMessageQ(USBH_Queue), NULL);

  /* Create USB Host Task */
#if defined (USBH_PROCESS_STACK_SIZE)
  osThreadDef(USBH_Thread, USBH_Process_OS, USBH_PROCESS_PRIO, 0U, USBH_PROCESS_STACK_SIZE);
#else
  osThreadDef(USBH_Thread, USBH_Process_OS, USBH_PROCESS_PRIO, 0U, 8U * configMINIMAL_STACK_SIZE);
#endif /* defined (USBH_PROCESS_STACK_SIZE) */

  phost->thread = osThreadCreate(osThread(USBH_Thread), phost);

#else

  /* Create USB Host Queue */
  phost->os_event = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(uint32_t), NULL);

  /* Create USB Host Task */
  USBH_Thread_Atrr.name = "USBH_Queue";

#if defined (USBH_PROCESS_STACK_SIZE)
  USBH_Thread_Atrr.stack_size = USBH_PROCESS_STACK_SIZE;
#else
  USBH_Thread_Atrr.stack_size = (8U * configMINIMAL_STACK_SIZE);
#endif /* defined (USBH_PROCESS_STACK_SIZE) */

  USBH_Thread_Atrr.priority = USBH_PROCESS_PRIO;
  phost->thread = osThreadNew(USBH_Process_OS, phost, &USBH_Thread_Atrr);

#endif /* (osCMSIS < 0x20000U) */
#endif /* (USBH_USE_OS == 1U) */

  /* Initialize low level driver */
  (void)USBH_LL_Init(phost);

  return USBH_OK;
}


/**
  * @brief  HCD_Init
  *         De-Initialize the Host portion of the driver.
  * @param  phost: Host Handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_DeInit(USBH_HandleTypeDef *phost)
{
  (void)DeInitStateMachine(phost);

  /* Restore default Device connection states */
  phost->device.PortEnabled = 0U;
  phost->device.is_connected = 0U;
  phost->device.is_disconnected = 0U;
  phost->device.is_ReEnumerated = 0U;
  phost->device.RstCnt = 0U;
  phost->device.EnumCnt = 0U;

  if (phost->pData != NULL)
  {
    (void)USBH_LL_Stop(phost);
  }

#if (USBH_USE_OS == 1U)
#if (osCMSIS < 0x20000U)

  /* Free allocated resource for USBH process */
  (void)osThreadTerminate(phost->thread);
  (void)osMessageDelete(phost->os_event);

#else

  /* Free allocated resource for USBH process */
  (void)osThreadTerminate(phost->thread);
  (void)osMessageQueueDelete(phost->os_event);

#endif /* (osCMSIS < 0x20000U) */
#endif /* (USBH_USE_OS == 1U) */

  return USBH_OK;
}


/**
  * @brief  DeInitStateMachine
  *         De-Initialize the Host state machine.
  * @param  phost: Host Handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef DeInitStateMachine(USBH_HandleTypeDef *phost)
{
  uint32_t i = 0U;

  /* Clear Pipes flags*/
  for (i = 0U; i < USBH_MAX_PIPES_NBR; i++)
  {
    phost->Pipes[i] = 0U;
  }

  for (i = 0U; i < USBH_MAX_DATA_BUFFER; i++)
  {
    phost->device.Data[i] = 0U;
  }

  phost->gState = HOST_IDLE;
  phost->EnumState = ENUM_IDLE;
  phost->RequestState = CMD_SEND;
  phost->Timer = 0U;

  phost->Control.state = CTRL_SETUP;
  phost->Control.pipe_size = USBH_MPS_DEFAULT;
  phost->Control.errorcount = 0U;

  phost->device.address = USBH_ADDRESS_DEFAULT;
  phost->device.speed = (uint8_t)USBH_SPEED_FULL;
  phost->device.RstCnt = 0U;
  phost->device.EnumCnt = 0U;

  return USBH_OK;
}


/**
  * @brief  USBH_RegisterClass
  *         Link class driver to Host Core.
  * @param  phost : Host Handle
  * @param  pclass: Class handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_RegisterClass(USBH_HandleTypeDef *phost, USBH_ClassTypeDef *pclass)
{
  USBH_StatusTypeDef status = USBH_OK;

  if (pclass != NULL)
  {
    if (phost->ClassNumber < USBH_MAX_NUM_SUPPORTED_CLASS)
    {
      /* link the class to the USB Host handle */
      phost->pClass[phost->ClassNumber++] = pclass;
      status = USBH_OK;
    }
    else
    {
      USBH_ErrLog("Max Class Number reached");
      status = USBH_FAIL;
    }
  }
  else
  {
    USBH_ErrLog("Invalid Class handle");
    status = USBH_FAIL;
  }

  return status;
}


/**
  * @brief  USBH_SelectInterface
  *         Select current interface.
  * @param  phost: Host Handle
  * @param  interface: Interface number
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_SelectInterface(USBH_HandleTypeDef *phost, uint8_t interface)
{
  USBH_StatusTypeDef status = USBH_OK;

  if (interface < phost->device.CfgDesc.bNumInterfaces)
  {
    phost->device.current_interface = interface;
    USBH_UsrLog("Switching to Interface (#%d)", interface);
    USBH_UsrLog("Class    : %xh", phost->device.CfgDesc.Itf_Desc[interface].bInterfaceClass);
    USBH_UsrLog("SubClass : %xh", phost->device.CfgDesc.Itf_Desc[interface].bInterfaceSubClass);
    USBH_UsrLog("Protocol : %xh", phost->device.CfgDesc.Itf_Desc[interface].bInterfaceProtocol);
  }
  else
  {
    USBH_ErrLog("Cannot Select This Interface.");
    status = USBH_FAIL;
  }

  return status;
}


/**
  * @brief  USBH_GetActiveClass
  *         Return Device Class.
  * @param  phost: Host Handle
  * @param  interface: Interface index
  * @retval Class Code
  */
uint8_t USBH_GetActiveClass(USBH_HandleTypeDef *phost)
{
  return (phost->device.CfgDesc.Itf_Desc[0].bInterfaceClass);
}


/**
  * @brief  USBH_FindInterface
  *         Find the interface index for a specific class.
  * @param  phost: Host Handle
  * @param  Class: Class code
  * @param  SubClass: SubClass code
  * @param  Protocol: Protocol code
  * @retval interface index in the configuration structure
  * @note : (1)interface index 0xFF means interface index not found
  */
uint8_t  USBH_FindInterface(USBH_HandleTypeDef *phost, uint8_t Class, uint8_t SubClass, uint8_t Protocol)
{
  USBH_InterfaceDescTypeDef *pif;
  USBH_CfgDescTypeDef *pcfg;
  uint8_t if_ix = 0U;

  pif = (USBH_InterfaceDescTypeDef *)NULL;
  pcfg = &phost->device.CfgDesc;

  while (if_ix < USBH_MAX_NUM_INTERFACES)
  {
    pif = &pcfg->Itf_Desc[if_ix];
    if (((pif->bInterfaceClass == Class) || (Class == 0xFFU)) &&
        ((pif->bInterfaceSubClass == SubClass) || (SubClass == 0xFFU)) &&
        ((pif->bInterfaceProtocol == Protocol) || (Protocol == 0xFFU)))
    {
      return  if_ix;
    }
    if_ix++;
  }
  return 0xFFU;
}


/**
  * @brief  USBH_FindInterfaceIndex
  *         Find the interface index for a specific class interface and alternate setting number.
  * @param  phost: Host Handle
  * @param  interface_number: interface number
  * @param  alt_settings    : alternate setting number
  * @retval interface index in the configuration structure
  * @note : (1)interface index 0xFF means interface index not found
  */
uint8_t  USBH_FindInterfaceIndex(USBH_HandleTypeDef *phost, uint8_t interface_number, uint8_t alt_settings)
{
  USBH_InterfaceDescTypeDef *pif;
  USBH_CfgDescTypeDef *pcfg;
  uint8_t if_ix = 0U;

  pif = (USBH_InterfaceDescTypeDef *)NULL;
  pcfg = &phost->device.CfgDesc;

  while (if_ix < USBH_MAX_NUM_INTERFACES)
  {
    pif = &pcfg->Itf_Desc[if_ix];
    if ((pif->bInterfaceNumber == interface_number) && (pif->bAlternateSetting == alt_settings))
    {
      return  if_ix;
    }
    if_ix++;
  }
  return 0xFFU;
}


/**
  * @brief  USBH_Start
  *         Start the USB Host Core.
  * @param  phost: Host Handle
  * @retval USBH Status
  */
USBH_StatusTypeDef  USBH_Start(USBH_HandleTypeDef *phost)
{
  /* Start the low level driver  */
  (void)USBH_LL_Start(phost);

  /* Activate VBUS on the port */
  (void)USBH_LL_DriverVBUS(phost, TRUE);

  return USBH_OK;
}


/**
  * @brief  USBH_Stop
  *         Stop the USB Host Core.
  * @param  phost: Host Handle
  * @retval USBH Status
  */
USBH_StatusTypeDef  USBH_Stop(USBH_HandleTypeDef *phost)
{
  /* DeActivate VBUS on the port */
  (void)USBH_LL_DriverVBUS(phost, FALSE);

  /* Stop and cleanup the low level driver  */
  (void)USBH_LL_Stop(phost);

  /* Free Control Pipes */
  (void)USBH_FreePipe(phost, phost->Control.pipe_in);
  (void)USBH_FreePipe(phost, phost->Control.pipe_out);

  return USBH_OK;
}


/**
  * @brief  HCD_ReEnumerate
  *         Perform a new Enumeration phase.
  * @param  phost: Host Handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_ReEnumerate(USBH_HandleTypeDef *phost)
{
  if (USBH_IsPortEnabled(phost) != 0U)
  {
    phost->device.is_ReEnumerated = 1U;

    /* Stop Host */
    (void)USBH_Stop(phost);

    phost->device.is_disconnected = 1U;
  }

#if (USBH_USE_OS == 1U)
  phost->os_msg = (uint32_t)USBH_PORT_EVENT;
#if (osCMSIS < 0x20000U)
  (void)osMessagePut(phost->os_event, phost->os_msg, 0U);
#else
  (void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, 0U);
#endif
#endif

  return USBH_OK;
}


static void USBH_ProcessDelay(
	USBH_HandleTypeDef *phost,
	HOST_StateTypeDef state,
	unsigned delayMS
	)
{
	const uint_fast16_t nticks = NTICKS(delayMS);
	if (nticks > 1)
	{
		phost->gPushTicks = nticks;
		phost->gPushState = state;
		phost->gState = HOST_DELAY;
	}
	else
	{
		phost->gState = state;
	}
}


/**
  * @brief  USBH_Process
  *         Background process of the USB Core.
  * @param  phost: Host Handle
  * @retval USBH Status
  */
USBH_StatusTypeDef  USBH_Process(USBH_HandleTypeDef *phost)
{
	USBH_StatusTypeDef status = USBH_FAIL;
	switch (phost->gState)
	{
	case HOST_IDLE:
		if (phost->device.is_connected)
		{
			//USBH_UsrLog(PSTR("USBH_Process: phost->device.is_connected"));
			/* Wait for 200 ms after connection */
			USBH_ProcessDelay(phost, HOST_DEV_BUS_RESET_ON, 200);
		}
		break;

	case HOST_DEV_BUS_RESET_ON:
		//USBH_UsrLog(PSTR("USBH_Process: HOST_DEV_BUS_RESET_ON"));
		USBH_LL_ResetPort2(phost, 1);
		USBH_ProcessDelay(phost, HOST_DEV_BUS_RESET_OFF, 50);
		break;

	case HOST_DEV_BUS_RESET_OFF:
		//USBH_UsrLog(PSTR("USBH_Process: HOST_DEV_BUS_RESET_OFF"));
		USBH_LL_ResetPort2(phost, 0);

	#if (USBH_USE_OS == 1)
			osMessagePut ( phost->os_event, USBH_PORT_EVENT, 0);
	#endif
		phost->gState = HOST_DEV_WAIT_FOR_ATTACHMENT;
		break;

	case HOST_DELAY:
		if (-- phost->gPushTicks == 0)
			phost->gState = phost->gPushState;
		break;

	case HOST_DEV_WAIT_FOR_ATTACHMENT:
		//USBH_UsrLog(PSTR("USBH_Process: HOST_DEV_WAIT_FOR_ATTACHMENT"));
		break;

	case HOST_DEV_BEFORE_ATTACHED:
		// в этом состоянии игнорируем disconnect - возникает при старте устройства с установленным USB устройством
		//USBH_UsrLog(PSTR("USBH_Process: HOST_DEV_BEFORE_ATTACHED"));
		/* Wait for 100 ms after Reset */
		USBH_ProcessDelay(phost, HOST_DEV_ATTACHED_WAITSPEED, 100);
		break;

	case HOST_DEV_ATTACHED_WAITSPEED:
		// Ожидаем пока не завершится процесс выясненеия скорости, на которой может работать device.
		if (USBH_LL_GetSpeedReady(phost))
			phost->gState = HOST_DEV_ATTACHED;
		else
			;//dbg_putchar('^');
		break;

	case HOST_DEV_ATTACHED:
		//USBH_UsrLog(PSTR("USBH_Process: HOST_DEV_ATTACHED"));
		/* после таймаута */
		phost->device.speed = USBH_LL_GetSpeed(phost);

		phost->gState = HOST_ENUMERATION;

		phost->Control.pipe_out = USBH_AllocPipe(phost, 0x00);
		phost->Control.pipe_in  = USBH_AllocPipe(phost, 0x80);


		/* Open Control pipes */
		USBH_OpenPipe(phost,
			phost->Control.pipe_in,
			0x80,
			phost->device.address,
			phost->device.speed,
			USBH_EP_CONTROL,
			phost->Control.pipe_size);

		/* Open Control pipes */
		USBH_OpenPipe(phost,
			phost->Control.pipe_out,
			0x00,
			phost->device.address,
			phost->device.speed,
			USBH_EP_CONTROL,
			phost->Control.pipe_size);

#if (USBH_USE_OS == 1)
		osMessagePut ( phost->os_event, USBH_PORT_EVENT, 0);
#endif

		break;

	case HOST_ENUMERATION:
		//USBH_UsrLog(PSTR("USBH_Process: HOST_ENUMERATION\n"));
		/* Check for enumeration status */
		if (USBH_HandleEnum(phost) == USBH_OK)
		{
			/* The function shall return USBH_OK when full enumeration is complete */
			USBH_UsrLog(PSTR("Enumeration done."));
			phost->device.current_interface = 0;
			if (phost->device.DevDesc.bNumConfigurations == 1)
			{
				USBH_UsrLog(PSTR("This device has only 1 configuration."));
				phost->gState = HOST_SET_CONFIGURATION;
			}
			else
			{
				phost->gState = HOST_INPUT;
			}

		}
		break;

  case HOST_INPUT:
	  //USBH_UsrLog(PSTR("USBH_Process: HOST_INPUT\n"));
    {
      /* user callback for end of device basic enumeration */
      if (phost->pUser != NULL)
      {
        phost->pUser(phost, HOST_USER_SELECT_CONFIGURATION);
        phost->gState = HOST_SET_CONFIGURATION;

#if (USBH_USE_OS == 1)
        osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
      }
    }
    break;

  case HOST_SET_CONFIGURATION:
	  //USBH_UsrLog(PSTR("USBH_Process: HOST_SET_CONFIGURATION"));
    /* set configuration */
    if (USBH_SetCfg(phost, phost->device.CfgDesc.bConfigurationValue) == USBH_OK)
    {
      phost->gState  = HOST_CHECK_CLASS;
      USBH_UsrLog(PSTR("Default configuration set.\n"));

    }
    break;

	case HOST_CHECK_CLASS:
		// Если USBH_GetActiveClass(phost) == 0x01 - работаем с составным устройством.
		//USBH_UsrLog(PSTR("USBH_Process: HOST_CHECK_CLASS (0x%02X)"), (unsigned) USBH_GetActiveClass(phost));
		if (phost->ClassNumber == 0)
		{
			USBH_UsrLog(PSTR("No Class has been registered.\n"));
			phost->gState = HOST_ABORT_STATE;
		}
		else
		{
			uint_fast8_t idx = 0;
			phost->pActiveClass = NULL;

			for (idx = 0; idx < phost->ClassNumber; idx ++)
			{
				if (phost->pClass [idx] != NULL && phost->pClass [idx]->ClassCode == USBH_GetActiveClass(phost))
				{
					phost->pActiveClass = phost->pClass [idx];
					break;	// или должен был последний из встретив
				}
			}

			if (phost->pActiveClass != NULL)
			{
				if (phost->pActiveClass->Init(phost) == USBH_OK)
				{
					phost->gState  = HOST_CLASS_REQUEST;
					USBH_UsrLog(PSTR("%s class started."), phost->pActiveClass->Name);

					/* Inform user that a class has been activated */
					phost->pUser(phost, HOST_USER_CLASS_SELECTED);
				}
				else
				{
					phost->gState = HOST_ABORT_STATE;
					USBH_UsrLog(PSTR("Device not supporting %s class."), phost->pActiveClass->Name);
				}
			}
			else
			{
				phost->gState  = HOST_ABORT_STATE;
				USBH_UsrLog(PSTR("No registered class for this device."));
			}
		}

#if (USBH_USE_OS == 1)
		osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
    break;

  case HOST_CLASS_REQUEST:
	  //USBH_UsrLog(PSTR("USBH_Process: HOST_CLASS_REQUEST\n"));
    /* process class standard control requests state machine */
    if (phost->pActiveClass != NULL)
    {
      status = phost->pActiveClass->Requests(phost);

      if(status == USBH_OK)
      {
        phost->gState  = HOST_CLASS;
      }
    }
    else
    {
      phost->gState  = HOST_ABORT_STATE;
      USBH_UsrLog(PSTR("Invalid Class Driver."));

#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
    }
    break;

  case HOST_CLASS:
	  //USBH_UsrLog(PSTR("USBH_Process: HOST_CLASS"));
    /* process class state machine */
    if (phost->pActiveClass != NULL)
    {
      phost->pActiveClass->BgndProcess(phost);
    }
    break;

  case HOST_DEV_DISCONNECTED :
	  //USBH_UsrLog(PSTR("USBH_Process: HOST_DEV_DISCONNECTED"));
    DeInitStateMachine(phost);

    /* Re-Initilaize Host for new Enumeration */
    if (phost->pActiveClass != NULL)
    {
      phost->pActiveClass->DeInit(phost);
      phost->pActiveClass = NULL;
    }
    break;

  case HOST_ABORT_STATE:
	  //USBH_UsrLog(PSTR("USBH_Process: HOST_ABORT_STATE"));
    break;

  default :
	  //TP();
	  //USBH_UsrLog(PSTR("USBH_Process: default"));
    break;
  }
 return USBH_OK;
}


/**
  * @brief  USBH_HandleEnum
  *         This function includes the complete enumeration process
  * @param  phost: Host Handle
  * @retval USBH_Status
  */
static USBH_StatusTypeDef USBH_HandleEnum(USBH_HandleTypeDef *phost)
{
  USBH_StatusTypeDef Status = USBH_BUSY;
  USBH_StatusTypeDef ReqStatus = USBH_BUSY;

  switch (phost->EnumState)
  {
    case ENUM_IDLE:
      /* Get Device Desc for only 1st 8 bytes : To get EP0 MaxPacketSize */
      ReqStatus = USBH_Get_DevDesc(phost, 8U);
      if (ReqStatus == USBH_OK)
      {
        phost->Control.pipe_size = phost->device.DevDesc.bMaxPacketSize;

        phost->EnumState = ENUM_GET_FULL_DEV_DESC;

        /* modify control channels configuration for MaxPacket size */
        (void)USBH_OpenPipe(phost, phost->Control.pipe_in, 0x80U, phost->device.address,
                            phost->device.speed, USBH_EP_CONTROL,
                            (uint16_t)phost->Control.pipe_size);

        /* Open Control pipes */
        (void)USBH_OpenPipe(phost, phost->Control.pipe_out, 0x00U, phost->device.address,
                            phost->device.speed, USBH_EP_CONTROL,
                            (uint16_t)phost->Control.pipe_size);
      }
      else if (ReqStatus == USBH_NOT_SUPPORTED)
      {
        USBH_ErrLog("Control error: Get Device Descriptor request failed");
        phost->device.EnumCnt++;
        if (phost->device.EnumCnt > 3U)
        {
          /* Buggy Device can't complete get device desc request */
          USBH_UsrLog("Control error, Device not Responding Please unplug the Device.");
          phost->gState = HOST_ABORT_STATE;
        }
        else
        {
          /* free control pipes */
          (void)USBH_FreePipe(phost, phost->Control.pipe_out);
          (void)USBH_FreePipe(phost, phost->Control.pipe_in);

          /* Reset the USB Device */
          phost->gState = HOST_IDLE;
        }
      }
      else
      {
        /* .. */
      }
      break;

    case ENUM_GET_FULL_DEV_DESC:
      /* Get FULL Device Desc  */
      ReqStatus = USBH_Get_DevDesc(phost, USB_DEVICE_DESC_SIZE);
      if (ReqStatus == USBH_OK)
      {
        USBH_UsrLog("PID: %xh", phost->device.DevDesc.idProduct);
        USBH_UsrLog("VID: %xh", phost->device.DevDesc.idVendor);

        phost->EnumState = ENUM_SET_ADDR;
      }
      else if (ReqStatus == USBH_NOT_SUPPORTED)
      {
        USBH_ErrLog("Control error: Get Full Device Descriptor request failed");
        phost->device.EnumCnt++;
        if (phost->device.EnumCnt > 3U)
        {
          /* Buggy Device can't complete get device desc request */
          USBH_UsrLog("Control error, Device not Responding Please unplug the Device.");
          phost->gState = HOST_ABORT_STATE;
        }
        else
        {
          /* Free control pipes */
          (void)USBH_FreePipe(phost, phost->Control.pipe_out);
          (void)USBH_FreePipe(phost, phost->Control.pipe_in);

          /* Reset the USB Device */
          phost->EnumState = ENUM_IDLE;
          phost->gState = HOST_IDLE;
        }
      }
      else
      {
        /* .. */
      }
      break;

    case ENUM_SET_ADDR:
      /* set address */
      ReqStatus = USBH_SetAddress(phost, USBH_DEVICE_ADDRESS);
      if (ReqStatus == USBH_OK)
      {
        USBH_Delay(2U);
        phost->device.address = USBH_DEVICE_ADDRESS;

        /* user callback for device address assigned */
        USBH_UsrLog("Address (#%d) assigned.", phost->device.address);
        phost->EnumState = ENUM_GET_CFG_DESC;

        /* modify control channels to update device address */
        (void)USBH_OpenPipe(phost, phost->Control.pipe_in, 0x80U,  phost->device.address,
                            phost->device.speed, USBH_EP_CONTROL,
                            (uint16_t)phost->Control.pipe_size);

        /* Open Control pipes */
        (void)USBH_OpenPipe(phost, phost->Control.pipe_out, 0x00U, phost->device.address,
                            phost->device.speed, USBH_EP_CONTROL,
                            (uint16_t)phost->Control.pipe_size);
      }
      else if (ReqStatus == USBH_NOT_SUPPORTED)
      {
        USBH_ErrLog("Control error: Device Set Address request failed");

        /* Buggy Device can't complete get device desc request */
        USBH_UsrLog("Control error, Device not Responding Please unplug the Device.");
        phost->gState = HOST_ABORT_STATE;
        phost->EnumState = ENUM_IDLE;
      }
      else
      {
        /* .. */
      }
      break;

    case ENUM_GET_CFG_DESC:
      /* get standard configuration descriptor */
      ReqStatus = USBH_Get_CfgDesc(phost, USB_CONFIGURATION_DESC_SIZE);
      if (ReqStatus == USBH_OK)
      {
        phost->EnumState = ENUM_GET_FULL_CFG_DESC;
      }
      else if (ReqStatus == USBH_NOT_SUPPORTED)
      {
        USBH_ErrLog("Control error: Get Device configuration descriptor request failed");
        phost->device.EnumCnt++;
        if (phost->device.EnumCnt > 3U)
        {
          /* Buggy Device can't complete get device desc request */
          USBH_UsrLog("Control error, Device not Responding Please unplug the Device.");
          phost->gState = HOST_ABORT_STATE;
        }
        else
        {
          /* Free control pipes */
          (void)USBH_FreePipe(phost, phost->Control.pipe_out);
          (void)USBH_FreePipe(phost, phost->Control.pipe_in);

          /* Reset the USB Device */
          phost->EnumState = ENUM_IDLE;
          phost->gState = HOST_IDLE;
        }
      }
      else
      {
        /* .. */
      }
      break;

    case ENUM_GET_FULL_CFG_DESC:
      /* get FULL config descriptor (config, interface, endpoints) */
      ReqStatus = USBH_Get_CfgDesc(phost, phost->device.CfgDesc.wTotalLength);
      if (ReqStatus == USBH_OK)
      {
        phost->EnumState = ENUM_GET_MFC_STRING_DESC;
      }
      else if (ReqStatus == USBH_NOT_SUPPORTED)
      {
        USBH_ErrLog("Control error: Get Device configuration descriptor request failed");
        phost->device.EnumCnt++;
        if (phost->device.EnumCnt > 3U)
        {
          /* Buggy Device can't complete get device desc request */
          USBH_UsrLog("Control error, Device not Responding Please unplug the Device.");
          phost->gState = HOST_ABORT_STATE;
        }
        else
        {
          /* Free control pipes */
          (void)USBH_FreePipe(phost, phost->Control.pipe_out);
          (void)USBH_FreePipe(phost, phost->Control.pipe_in);

          /* Reset the USB Device */
          phost->EnumState = ENUM_IDLE;
          phost->gState = HOST_IDLE;
        }
      }
      else
      {
        /* .. */
      }
      break;

    case ENUM_GET_MFC_STRING_DESC:
      if (phost->device.DevDesc.iManufacturer != 0U)
      {
        /* Check that Manufacturer String is available */
        ReqStatus = USBH_Get_StringDesc(phost, phost->device.DevDesc.iManufacturer,
                                        phost->device.Data, 0xFFU);
        if (ReqStatus == USBH_OK)
        {
          /* User callback for Manufacturing string */
          USBH_UsrLog("Manufacturer : %s", (char *)(void *)phost->device.Data);
          phost->EnumState = ENUM_GET_PRODUCT_STRING_DESC;

#if (USBH_USE_OS == 1U)
          phost->os_msg = (uint32_t)USBH_STATE_CHANGED_EVENT;
#if (osCMSIS < 0x20000U)
          (void)osMessagePut(phost->os_event, phost->os_msg, 0U);
#else
          (void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, 0U);
#endif
#endif
        }
        else if (ReqStatus == USBH_NOT_SUPPORTED)
        {
          USBH_UsrLog("Manufacturer : N/A");
          phost->EnumState = ENUM_GET_PRODUCT_STRING_DESC;

#if (USBH_USE_OS == 1U)
          phost->os_msg = (uint32_t)USBH_STATE_CHANGED_EVENT;
#if (osCMSIS < 0x20000U)
          (void)osMessagePut(phost->os_event, phost->os_msg, 0U);
#else
          (void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, 0U);
#endif
#endif
        }
        else
        {
          /* .. */
        }
      }
      else
      {
        USBH_UsrLog("Manufacturer : N/A");
        phost->EnumState = ENUM_GET_PRODUCT_STRING_DESC;

#if (USBH_USE_OS == 1U)
        phost->os_msg = (uint32_t)USBH_STATE_CHANGED_EVENT;
#if (osCMSIS < 0x20000U)
        (void)osMessagePut(phost->os_event, phost->os_msg, 0U);
#else
        (void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, 0U);
#endif
#endif
      }
      break;

    case ENUM_GET_PRODUCT_STRING_DESC:
      if (phost->device.DevDesc.iProduct != 0U)
      {
        /* Check that Product string is available */
        ReqStatus = USBH_Get_StringDesc(phost, phost->device.DevDesc.iProduct,
                                        phost->device.Data, 0xFFU);
        if (ReqStatus == USBH_OK)
        {
          /* User callback for Product string */
          USBH_UsrLog("Product : %s", (char *)(void *)phost->device.Data);
          phost->EnumState = ENUM_GET_SERIALNUM_STRING_DESC;
        }
        else if (ReqStatus == USBH_NOT_SUPPORTED)
        {
          USBH_UsrLog("Product : N/A");
          phost->EnumState = ENUM_GET_SERIALNUM_STRING_DESC;

#if (USBH_USE_OS == 1U)
          phost->os_msg = (uint32_t)USBH_STATE_CHANGED_EVENT;
#if (osCMSIS < 0x20000U)
          (void)osMessagePut(phost->os_event, phost->os_msg, 0U);
#else
          (void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, 0U);
#endif
#endif
        }
        else
        {
          /* .. */
        }
      }
      else
      {
        USBH_UsrLog("Product : N/A");
        phost->EnumState = ENUM_GET_SERIALNUM_STRING_DESC;

#if (USBH_USE_OS == 1U)
        phost->os_msg = (uint32_t)USBH_STATE_CHANGED_EVENT;
#if (osCMSIS < 0x20000U)
        (void)osMessagePut(phost->os_event, phost->os_msg, 0U);
#else
        (void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, 0U);
#endif
#endif
      }
      break;

    case ENUM_GET_SERIALNUM_STRING_DESC:
      if (phost->device.DevDesc.iSerialNumber != 0U)
      {
        /* Check that Serial number string is available */
        ReqStatus = USBH_Get_StringDesc(phost, phost->device.DevDesc.iSerialNumber,
                                        phost->device.Data, 0xFFU);
        if (ReqStatus == USBH_OK)
        {
          /* User callback for Serial number string */
          USBH_UsrLog("Serial Number : %s", (char *)(void *)phost->device.Data);
          Status = USBH_OK;
        }
        else if (ReqStatus == USBH_NOT_SUPPORTED)
        {
          USBH_UsrLog("Serial Number : N/A");
          Status = USBH_OK;
        }
        else
        {
          /* .. */
        }
      }
      else
      {
        USBH_UsrLog("Serial Number : N/A");
        Status = USBH_OK;
      }
      break;

    default:
      break;
  }
  return Status;
}


/**
  * @brief  USBH_LL_SetTimer
  *         Set the initial Host Timer tick
  * @param  phost: Host Handle
  * @retval None
  */
void  USBH_LL_SetTimer(USBH_HandleTypeDef *phost, uint32_t time)
{
  phost->Timer = time;
}


/**
  * @brief  USBH_LL_IncTimer
  *         Increment Host Timer tick
  * @param  phost: Host Handle
  * @retval None
  */
void  USBH_LL_IncTimer(USBH_HandleTypeDef *phost)
{
  phost->Timer++;
  USBH_HandleSof(phost);
}


/**
  * @brief  USBH_HandleSof
  *         Call SOF process
  * @param  phost: Host Handle
  * @retval None
  */
static void  USBH_HandleSof(USBH_HandleTypeDef *phost)
{
  if ((phost->gState == HOST_CLASS) && (phost->pActiveClass != NULL))
  {
    phost->pActiveClass->SOFProcess(phost);
  }
}


/**
  * @brief  USBH_PortEnabled
  *         Port Enabled
  * @param  phost: Host Handle
  * @retval None
  */
void USBH_LL_PortEnabled(USBH_HandleTypeDef *phost)
{
  phost->device.PortEnabled = 1U;

#if (USBH_USE_OS == 1U)
  phost->os_msg = (uint32_t)USBH_PORT_EVENT;
#if (osCMSIS < 0x20000U)
  (void)osMessagePut(phost->os_event, phost->os_msg, 0U);
#else
  (void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, 0U);
#endif
#endif

  return;
}


/**
  * @brief  USBH_LL_PortDisabled
  *         Port Disabled
  * @param  phost: Host Handle
  * @retval None
  */
void USBH_LL_PortDisabled(USBH_HandleTypeDef *phost)
{
  phost->device.PortEnabled = 0U;

  return;
}


/**
  * @brief  HCD_IsPortEnabled
  *         Is Port Enabled
  * @param  phost: Host Handle
  * @retval None
  */
uint8_t USBH_IsPortEnabled(USBH_HandleTypeDef *phost)
{
  return (phost->device.PortEnabled);
}


/**
  * @brief  USBH_LL_Connect
  *         Handle USB Host connexion event
  * @param  phost: Host Handle
  * @retval USBH_Status
  */
USBH_StatusTypeDef  USBH_LL_Connect(USBH_HandleTypeDef *phost)
{
	switch (phost->gState)
	{
	case HOST_IDLE:
		//PRINTF(PSTR("USBH_LL_Connect at HOST_IDLE\n"));
		phost->device.is_connected = 1U;
		phost->device.is_disconnected = 0U;
		phost->device.is_ReEnumerated = 0U;

#if (USBH_USE_OS == 1U)
  phost->os_msg = (uint32_t)USBH_PORT_EVENT;
#if (osCMSIS < 0x20000U)
  (void)osMessagePut(phost->os_event, phost->os_msg, 0U);
#else
  (void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, 0U);
#endif
#endif

//		if (phost->pUser != NULL)
//		{
//			phost->pUser(phost, HOST_USER_CONNECTION);
//		}
		break;

	case HOST_DEV_WAIT_FOR_ATTACHMENT:
		//PRINTF(PSTR("USBH_LL_Connect at HOST_DEV_WAIT_FOR_ATTACHMENT\n"));
		phost->gState = HOST_DEV_BEFORE_ATTACHED;
		break;

	default:
		break;
	}

	return USBH_OK;
}


/**
  * @brief  USBH_LL_Disconnect
  *         Handle USB Host disconnection event
  * @param  phost: Host Handle
  * @retval USBH_Status
  */
USBH_StatusTypeDef  USBH_LL_Disconnect(USBH_HandleTypeDef *phost)
{
	//USBH_UsrLog("USBH_LL_Disconnect");

	switch (phost->gState)
	{
	case HOST_DEV_BEFORE_ATTACHED:
		//PRINTF(PSTR("USBH_LL_Disconnect at HOST_DEV_BEFORE_ATTACHED\n"));
		return USBH_OK;

	default:
		//PRINTF(PSTR("USBH_LL_Disconnect at phost->gState=%d\n"), (int) phost->gState);
		break;
	}

	  phost->device.is_disconnected = 1U;
	  phost->device.is_connected = 0U;
	  phost->device.PortEnabled = 0U;
	/*Stop Host */
	USBH_LL_Stop(phost);

	/* Free Control Pipes */
	USBH_FreePipe  (phost, phost->Control.pipe_in);
	USBH_FreePipe  (phost, phost->Control.pipe_out);

	if (phost->pUser != NULL)
	{
		phost->pUser(phost, HOST_USER_DISCONNECTION);
	}

	//PRINTF(PSTR("USB Device disconnected\n"));

	/* Start the low level driver  */
	USBH_LL_Start(phost);

	phost->gState = HOST_DEV_DISCONNECTED;

#if (USBH_USE_OS == 1)
	osMessagePut ( phost->os_event, USBH_PORT_EVENT, 0);
#endif

	return USBH_OK;
  /* update device connection states */
  phost->device.is_disconnected = 1U;
  phost->device.is_connected = 0U;
  phost->device.PortEnabled = 0U;

  /* Stop Host */
  (void)USBH_LL_Stop(phost);

  /* FRee Control Pipes */
  (void)USBH_FreePipe(phost, phost->Control.pipe_in);
  (void)USBH_FreePipe(phost, phost->Control.pipe_out);
#if (USBH_USE_OS == 1U)
  phost->os_msg = (uint32_t)USBH_PORT_EVENT;
#if (osCMSIS < 0x20000U)
  (void)osMessagePut(phost->os_event, phost->os_msg, 0U);
#else
  (void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, 0U);
#endif
#endif

  return USBH_OK;
}


#if (USBH_USE_OS == 1U)
/**
  * @brief  USB Host Thread task
  * @param  pvParameters not used
  * @retval None
  */

#if (osCMSIS < 0x20000U)
static void USBH_Process_OS(void const *argument)
{
  osEvent event;

  for (;;)
  {
    event = osMessageGet(((USBH_HandleTypeDef *)argument)->os_event, osWaitForever);
    if (event.status == osEventMessage)
    {
      USBH_Process((USBH_HandleTypeDef *)argument);
    }
  }
}
#else
static void USBH_Process_OS(void *argument)
{
  osStatus_t status;

  for (;;)
  {
    status = osMessageQueueGet(((USBH_HandleTypeDef *)argument)->os_event,
                               &((USBH_HandleTypeDef *)argument)->os_msg, NULL, osWaitForever);
    if (status == osOK)
    {
      USBH_Process((USBH_HandleTypeDef *)argument);
    }
  }
}
#endif /* (osCMSIS < 0x20000U) */


/**
  * @brief  USBH_LL_NotifyURBChange
  *         Notify URB state Change
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef  USBH_LL_NotifyURBChange(USBH_HandleTypeDef *phost)
{
  phost->os_msg = (uint32_t)USBH_PORT_EVENT;

#if (osCMSIS < 0x20000U)
  (void)osMessagePut(phost->os_event, phost->os_msg, 0U);
#else
  (void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, 0U);
#endif

  return USBH_OK;
}
#endif
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
