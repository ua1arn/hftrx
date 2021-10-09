/**
  ******************************************************************************
  * @file    usbh_msc_scsi.c
  * @author  MCD Application Team
  * @brief   This file implements the SCSI commands
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

/* BSPDependencies
- "stm32xxxxx_{eval}{discovery}{nucleo_144}.c"
- "stm32xxxxx_{eval}{discovery}_io.c"
- "stm32xxxxx_{eval}{discovery}{adafruit}_lcd.c"
- "stm32xxxxx_{eval}{discovery}_sdram.c"
EndBSPDependencies */

#include "hardware.h"
#include "formats.h"

#if WITHUSBHW

/* Includes ------------------------------------------------------------------*/
#include "../Inc/usbh_msc.h"
#include "../Inc/usbh_msc_scsi.h"
#include "../Inc/usbh_msc_bot.h"


/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_MSC_CLASS
  * @{
  */

/** @defgroup USBH_MSC_SCSI
  * @brief    This file includes the mass storage related functions
  * @{
  */


/** @defgroup USBH_MSC_SCSI_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @defgroup USBH_MSC_SCSI_Private_Defines
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_MSC_SCSI_Private_Macros
  * @{
  */
/**
  * @}
  */


/** @defgroup USBH_MSC_SCSI_Private_FunctionPrototypes
  * @{
  */
/**
  * @}
  */


/** @defgroup USBH_MSC_SCSI_Exported_Variables
  * @{
  */

/**
  * @}
  */


/** @defgroup USBH_MSC_SCSI_Private_Functions
  * @{
  */


/**
  * @brief  USBH_MSC_SCSI_TestUnitReady
  *         Issue TestUnitReady command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_SCSI_TestUnitReady(USBH_HandleTypeDef *phost,
                                               uint8_t lun)
{
  USBH_StatusTypeDef    error = USBH_FAIL ;
  MSC_HandleTypeDef * const MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  BOT_HandleTypeDef * const hBot = & MSC_Handle->hbot;

  switch (hBot->cmd_state)
  {
    case BOT_CMD_SEND:

      /*Prepare the CBW and relevant field*/
      hBot->cbw.field.DataTransferLength = DATA_LEN_MODE_TEST_UNIT_READY;
      hBot->cbw.field.Flags = USB_EP_DIR_OUT;
      hBot->cbw.field.CBLength = CBW_LENGTH;

      (void)USBH_memset(hBot->cbw.field.CB, 0, sizeof hBot->cbw.field.CB);
      hBot->cbw.field.CB[0]  = OPCODE_TEST_UNIT_READY;

      hBot->state = BOT_SEND_CBW;
      hBot->cmd_state = BOT_CMD_WAIT;
      error = USBH_BUSY;
      break;

    case BOT_CMD_WAIT:
      error = USBH_MSC_BOT_Process(phost, lun);
      break;

    default:
      break;
  }

  return error;
}

  /**
    * @brief  USBH_MSC_SCSI_ReadCapacity10
    *         Issue Read Capacity 10 command.
    * @param  phost: Host handle
    * @param  lun: Logical Unit Number
    * @param  capacity: pointer to the capacity structure
    * @retval USBH Status
    */
  USBH_StatusTypeDef USBH_MSC_SCSI_ReadCapacity10(USBH_HandleTypeDef *phost,
                                                uint8_t lun,
                                                SCSI_CapacityTypeDef *capacity)
  {
    USBH_StatusTypeDef    error = USBH_BUSY ;
    MSC_HandleTypeDef * const MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
    BOT_HandleTypeDef * const hBot = & MSC_Handle->hbot;

    switch (hBot->cmd_state)
    {
      case BOT_CMD_SEND:

        /*Prepare the CBW and relevant field*/
        hBot->cbw.field.DataTransferLength = DATA_LEN_READ_CAPACITY10;
        hBot->cbw.field.Flags = USB_EP_DIR_IN;
        hBot->cbw.field.CBLength = CBW_LENGTH;

        (void)USBH_memset(hBot->cbw.field.CB, 0, sizeof hBot->cbw.field.CB);
        hBot->cbw.field.CB[0]  = OPCODE_READ_CAPACITY10;

        hBot->state = BOT_SEND_CBW;

        hBot->cmd_state = BOT_CMD_WAIT;
        hBot->pbuf = (uint8_t *) hBot->data;
        error = USBH_BUSY;
        break;

      case BOT_CMD_WAIT:

        error = USBH_MSC_BOT_Process(phost, lun);

        if (error == USBH_OK)
        {
          /*assign the capacity*/
      	  capacity->block_nbr64 = USBD_peek_u32_BE(& hBot->pbuf[0]) + 1;	// last block LBA to number of blocks conversion
          /*assign the page length*/
          capacity->block_size = USBD_peek_u32_BE(& hBot->pbuf[4]);
         }
        break;

      default:
        break;
    }

    return error;
  }

  /**
    * @brief  USBH_MSC_SCSI_ReadCapacity16
    *         Issue Read Capacity 16 command.
    * @param  phost: Host handle
    * @param  lun: Logical Unit Number
    * @param  capacity: pointer to the capacity structure
    * @retval USBH Status
    */
  USBH_StatusTypeDef USBH_MSC_SCSI_ReadCapacity16(USBH_HandleTypeDef *phost,
                                                uint8_t lun,
                                                SCSI_CapacityTypeDef *capacity)
  {
    USBH_StatusTypeDef    error = USBH_BUSY ;
    MSC_HandleTypeDef * const MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
    BOT_HandleTypeDef * const hBot = & MSC_Handle->hbot;

    switch (hBot->cmd_state)
    {
      case BOT_CMD_SEND:

        /*Prepare the CBW and relevant field*/
        hBot->cbw.field.DataTransferLength = DATA_LEN_READ_CAPACITY16;
        hBot->cbw.field.Flags = USB_EP_DIR_IN;
        hBot->cbw.field.CBLength = CBW_LENGTH;

        (void)USBH_memset(hBot->cbw.field.CB, 0, sizeof hBot->cbw.field.CB);
        hBot->cbw.field.CB[0]  = OPCODE_READ_CAPACITY16;

        hBot->state = BOT_SEND_CBW;

        hBot->cmd_state = BOT_CMD_WAIT;
        hBot->pbuf = (uint8_t *) hBot->data;
        error = USBH_BUSY;
        break;

      case BOT_CMD_WAIT:

        error = USBH_MSC_BOT_Process(phost, lun);

        if (error == USBH_OK)
        {
          /*assign the capacity*/
      	  capacity->block_nbr64 = USBD_peek_u64_BE(& hBot->pbuf[0]) + 1;	// last block LBA to number of blocks conversion
          /*assign the page length*/
          capacity->block_size = USBD_peek_u64_BE(& hBot->pbuf[8]);
         }
        break;

      default:
        break;
    }

    return error;
  }

/**
  * @brief  USBH_MSC_SCSI_Inquiry
  *         Issue Inquiry command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @param  capacity: pointer to the inquiry structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_SCSI_Inquiry(USBH_HandleTypeDef *phost, uint8_t lun,
                                         SCSI_StdInquiryDataTypeDef *inquiry)
{
  USBH_StatusTypeDef error = USBH_FAIL;
  MSC_HandleTypeDef * const MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  BOT_HandleTypeDef * const hBot = & MSC_Handle->hbot;

  switch (hBot->cmd_state)
  {
    case BOT_CMD_SEND:

      /*Prepare the CBW and relevant field*/
      hBot->cbw.field.DataTransferLength = DATA_LEN_INQUIRY;
      hBot->cbw.field.Flags = USB_EP_DIR_IN;
      hBot->cbw.field.CBLength = CBW_LENGTH;

      (void)USBH_memset(hBot->cbw.field.CB, 0, CBW_LENGTH);
      hBot->cbw.field.CB[0]  = OPCODE_INQUIRY;
      hBot->cbw.field.CB[1]  = (lun << 5);
      hBot->cbw.field.CB[2]  = 0U;
      hBot->cbw.field.CB[3]  = 0U;
      hBot->cbw.field.CB[4]  = 0x24U;
      hBot->cbw.field.CB[5]  = 0U;

      hBot->state = BOT_SEND_CBW;

      hBot->cmd_state = BOT_CMD_WAIT;
      hBot->pbuf = (uint8_t *) hBot->data;
      error = USBH_BUSY;
      break;

    case BOT_CMD_WAIT:

      error = USBH_MSC_BOT_Process(phost, lun);

      if (error == USBH_OK)
      {
        USBH_memset(inquiry, 0, sizeof(SCSI_StdInquiryDataTypeDef));
        /*assign Inquiry Data */
        inquiry->DeviceType = hBot->pbuf[0] & 0x1FU;
        inquiry->PeripheralQualifier = hBot->pbuf[0] >> 5U;

        if (((uint32_t)hBot->pbuf[1] & 0x80U) == 0x80U)
        {
          inquiry->RemovableMedia = 1U;
        }
        else
        {
          inquiry->RemovableMedia = 0U;
        }

        (void)USBH_memcpy(inquiry->vendor_id, &hBot->pbuf[8], 8U);
        inquiry->vendor_id [8U] = '\0';
        (void)USBH_memcpy(inquiry->product_id, &hBot->pbuf[16], 16U);
        inquiry->product_id [16U] = '\0';
        (void)USBH_memcpy(inquiry->revision_id, &hBot->pbuf[32], 4U);
        inquiry->revision_id [4U] = '\0';
      }
      break;

    default:
      break;
  }

  return error;
}

/**
  * @brief  USBH_MSC_SCSI_RequestSense
  *         Issue RequestSense command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @param  capacity: pointer to the sense data structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_SCSI_RequestSense(USBH_HandleTypeDef *phost,
                                              uint8_t lun,
                                              SCSI_SenseTypeDef *sense_data)
{
  USBH_StatusTypeDef    error = USBH_FAIL ;
  MSC_HandleTypeDef * const MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  BOT_HandleTypeDef * const hBot = & MSC_Handle->hbot;

  switch (hBot->cmd_state)
  {
    case BOT_CMD_SEND:

      /*Prepare the CBW and relevant field*/
      hBot->cbw.field.DataTransferLength = DATA_LEN_REQUEST_SENSE;
      hBot->cbw.field.Flags = USB_EP_DIR_IN;
      hBot->cbw.field.CBLength = CBW_LENGTH;

      (void)USBH_memset(hBot->cbw.field.CB, 0, sizeof hBot->cbw.field.CB);
      hBot->cbw.field.CB[0]  = OPCODE_REQUEST_SENSE;
      hBot->cbw.field.CB[1]  = (lun << 5);
      hBot->cbw.field.CB[2]  = 0U;
      hBot->cbw.field.CB[3]  = 0U;
      hBot->cbw.field.CB[4]  = DATA_LEN_REQUEST_SENSE;
      hBot->cbw.field.CB[5]  = 0U;

      hBot->state = BOT_SEND_CBW;
      hBot->cmd_state = BOT_CMD_WAIT;
      hBot->pbuf = (uint8_t *) hBot->data;
      error = USBH_BUSY;
      break;

    case BOT_CMD_WAIT:

      error = USBH_MSC_BOT_Process(phost, lun);

      if (error == USBH_OK)
      {
        sense_data->key  = hBot->pbuf[2] & 0x0FU;
        sense_data->asc  = hBot->pbuf[12];
        sense_data->ascq = hBot->pbuf[13];
      }
      break;

    default:
      break;
  }

  return error;
}

/**
  * @brief  USBH_MSC_SCSI_Write10
  *         Issue write10 command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @param  address: sector address
  * @param  pbuf: pointer to data
  * @param  length: number of sector to write
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_SCSI_Write10(USBH_HandleTypeDef *phost,
                                       uint8_t lun,
                                       uint32_t address,
                                       uint8_t *pbuf,
                                       uint32_t length)
{
  USBH_StatusTypeDef    error = USBH_FAIL ;

  MSC_HandleTypeDef * const MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  BOT_HandleTypeDef * const hBot = & MSC_Handle->hbot;

  switch (hBot->cmd_state)
  {
    case BOT_CMD_SEND:

      /*Prepare the CBW and relevant field*/
      hBot->cbw.field.DataTransferLength = length * MSC_Handle->unit[0].capacity.block_size;
      hBot->cbw.field.Flags = USB_EP_DIR_OUT;
      hBot->cbw.field.CBLength = CBW_LENGTH;

      (void)USBH_memset(hBot->cbw.field.CB, 0, sizeof hBot->cbw.field.CB);
      hBot->cbw.field.CB[0]  = OPCODE_WRITE10;

      /*logical block address*/
      USBD_poke_u32_BE(& hBot->cbw.field.CB[2], address);

      /*Transfer length */
      USBD_poke_u16_BE(& hBot->cbw.field.CB[7], length);

      hBot->state = BOT_SEND_CBW;
      hBot->cmd_state = BOT_CMD_WAIT;
      hBot->pbuf = pbuf;
      error = USBH_BUSY;
      break;

    case BOT_CMD_WAIT:
      error = USBH_MSC_BOT_Process(phost, lun);
      break;

    default:
      break;
  }

  return error;
}

/**
  * @brief  USBH_MSC_SCSI_Read10
  *         Issue Read10 command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @param  address: sector address
  * @param  pbuf: pointer to data
  * @param  length: number of sector to read
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_SCSI_Read10(USBH_HandleTypeDef *phost,
                                      uint8_t lun,
                                      uint32_t address,
                                      uint8_t *pbuf,
                                      uint32_t length)
{
  USBH_StatusTypeDef    error = USBH_FAIL ;
  MSC_HandleTypeDef * const MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  BOT_HandleTypeDef * const hBot = & MSC_Handle->hbot;

  switch (hBot->cmd_state)
  {
    case BOT_CMD_SEND:

      /*Prepare the CBW and relevant field*/
      hBot->cbw.field.DataTransferLength = length * MSC_Handle->unit[0].capacity.block_size;
      hBot->cbw.field.Flags = USB_EP_DIR_IN;
      hBot->cbw.field.CBLength = CBW_LENGTH;

      (void)USBH_memset(hBot->cbw.field.CB, 0, sizeof hBot->cbw.field.CB);
      hBot->cbw.field.CB[0]  = OPCODE_READ10;

      /*logical block address*/
       USBD_poke_u32_BE(& hBot->cbw.field.CB[2], address);

       /*Transfer length */
       USBD_poke_u16_BE(& hBot->cbw.field.CB[7], length);

      hBot->state = BOT_SEND_CBW;
      hBot->cmd_state = BOT_CMD_WAIT;
      hBot->pbuf = pbuf;
      error = USBH_BUSY;
      break;

    case BOT_CMD_WAIT:
      error = USBH_MSC_BOT_Process(phost, lun);
      break;

    default:
      break;
  }

  return error;
}

/**
  * @brief  USBH_MSC_SCSI_Write12
  *         Issue write12 command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @param  address: sector address
  * @param  pbuf: pointer to data
  * @param  length: number of sector to write
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_SCSI_Write12(USBH_HandleTypeDef *phost,
                                       uint8_t lun,
                                       uint32_t address,
                                       uint8_t *pbuf,
                                       uint32_t length)
{
  USBH_StatusTypeDef    error = USBH_FAIL ;

  MSC_HandleTypeDef * const MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  BOT_HandleTypeDef * const hBot = & MSC_Handle->hbot;

  switch (hBot->cmd_state)
  {
    case BOT_CMD_SEND:

      /*Prepare the CBW and relevant field*/
      hBot->cbw.field.DataTransferLength = length * MSC_Handle->unit[0].capacity.block_size;
      hBot->cbw.field.Flags = USB_EP_DIR_OUT;
      hBot->cbw.field.CBLength = 12;//CBW_LENGTH;

      (void)USBH_memset(hBot->cbw.field.CB, 0, sizeof hBot->cbw.field.CB);
      hBot->cbw.field.CB[0]  = OPCODE_WRITE12;
      hBot->cbw.field.CB[1]  =
    		  (1uL << 3) |		// FUA bit
			  0;

      /*logical block address*/
      USBD_poke_u32_BE(& hBot->cbw.field.CB[2], address);

      /*Transfer length */
      USBD_poke_u32_BE(& hBot->cbw.field.CB[6], length);

      hBot->state = BOT_SEND_CBW;
      hBot->cmd_state = BOT_CMD_WAIT;
      hBot->pbuf = pbuf;
      error = USBH_BUSY;
      break;

    case BOT_CMD_WAIT:
      error = USBH_MSC_BOT_Process(phost, lun);
      break;

    default:
      break;
  }

  return error;
}

/**
  * @brief  USBH_MSC_SCSI_Read12
  *         Issue Read12 command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @param  address: sector address
  * @param  pbuf: pointer to data
  * @param  length: number of sector to read
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_SCSI_Read12(USBH_HandleTypeDef *phost,
                                      uint8_t lun,
                                      uint32_t address,
                                      uint8_t *pbuf,
                                      uint32_t length)
{
  USBH_StatusTypeDef    error = USBH_FAIL ;
  MSC_HandleTypeDef * const MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  BOT_HandleTypeDef * const hBot = & MSC_Handle->hbot;

  switch (hBot->cmd_state)
  {
    case BOT_CMD_SEND:

      /*Prepare the CBW and relevant field*/
      hBot->cbw.field.DataTransferLength = length * MSC_Handle->unit[0].capacity.block_size;
      hBot->cbw.field.Flags = USB_EP_DIR_IN;
      hBot->cbw.field.CBLength = 12;//CBW_LENGTH;

      (void)USBH_memset(hBot->cbw.field.CB, 0, sizeof hBot->cbw.field.CB);
      hBot->cbw.field.CB[0]  = OPCODE_READ12;
      hBot->cbw.field.CB[1]  =
    		  (1uL << 3) |		// FUA bit
			  0;

      /*logical block address*/
       USBD_poke_u32_BE(& hBot->cbw.field.CB[2], address);

       /*Transfer length */
       USBD_poke_u32_BE(& hBot->cbw.field.CB[6], length);

      hBot->state = BOT_SEND_CBW;
      hBot->cmd_state = BOT_CMD_WAIT;
      hBot->pbuf = pbuf;
      error = USBH_BUSY;
      break;

    case BOT_CMD_WAIT:
      error = USBH_MSC_BOT_Process(phost, lun);
      break;

    default:
      break;
  }

  return error;
}


/**
  * @brief  USBH_MSC_SCSI_Process
  *         Oricess execution command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_SCSI_Process(USBH_HandleTypeDef *phost,
                                      uint8_t lun)
{
  USBH_StatusTypeDef    error = USBH_FAIL ;
  MSC_HandleTypeDef * const MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  BOT_HandleTypeDef * const hBot = & MSC_Handle->hbot;

  switch (hBot->cmd_state)
  {

    case BOT_CMD_WAIT:
      error = USBH_MSC_BOT_Process(phost, lun);
      break;

    default:
      break;
  }

  return error;
}

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

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/



#endif /* WITHUSBHW */
