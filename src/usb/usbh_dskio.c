
/*
 * usbd_msc.c
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
*/

#include "hardware.h"
#include "board.h"
#include "src/display/display.h"
#include "formats.h"
#include <string.h>

#if WITHUSBHW

#include "src/fatfs/ff.h"
#include "src/fatfs/diskio.h"		/* FatFs lower layer API */
#if WITHTINYUSB
#include "tusb.h"
#else
#include "../../Class/MSC/Inc/usbh_msc.h"
#endif

#include "usbd_def.h"
#include "usbd_core.h"
#include "usb200.h"
#include "usbch9.h"

#if WITHUSEUSBFLASH

#include "usb_device.h"
#include "sdcard.h"
//#include "src/fatfs/ff.h"
#define USB_DEFAULT_BLOCK_SIZE 512

static
DSTATUS USB_Initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
	)
{
	  /* CAUTION : USB Host library has to be initialized in the application */
	//PRINTF(PSTR("USB_Initialize: drv=%d\n"), (int) drv);
	return 0;
	if (1)
	{
		//if (HCD_IsDeviceConnected(&USB_OTG_Core) && TM_USB_MSCHOST_INT_Result == TM_USB_MSCHOST_Result_Connected) {
		//	USB_Stat &= ~ STA_NOINIT;
		//} else {
		//	USB_Stat |= STA_NOINIT;
		//}

		//return USB_Stat;
	}
	return STA_NODISK;
}


static
DSTATUS USB_Status (
	BYTE lun		/* Physical drive nmuber (0..) */
	)
{
	DRESULT res = RES_ERROR;

	if (USBH_MSC_UnitIsReady(&hUsbHostHS, lun))
	{
		res = RES_OK;	// STA_NOINIT or STA_NODISK or STA_PROTECT
	}
	else
	{
		res = RES_ERROR;
	}

	return res;
}

// write a size Byte big block beginning at the address.
static
DRESULT USB_disk_write(
	BYTE lun,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write */
	)
{
	  DRESULT res = RES_ERROR;
	  MSC_LUNTypeDef info;

	  //PRINTF("USB_disk_write: lun=%d, buff=%p, sector=%lu, count=%u\n", (int) lun, buff, (unsigned long) sector, (unsigned) count);
	  if(USBH_MSC_Write(&hUsbHostHS, lun, sector, (BYTE *)buff, count) == USBH_OK)
	  {
	    res = RES_OK;
	  }
	  else
	  {
	    USBH_MSC_GetLUNInfo(&hUsbHostHS, lun, &info);

	    switch (info.sense.asc)
	    {
	    case SCSI_ASC_WRITE_PROTECTED:
	      PRINTF("USB Disk is Write protected!\n");
	      res = RES_WRPRT;
	      break;

	    case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
	    case SCSI_ASC_MEDIUM_NOT_PRESENT:
	    case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
	    	PRINTF("USB Disk is not ready!\n");
	      res = RES_NOTRDY;
	      break;

	    default:
	      res = RES_ERROR;
	      break;
	    }
	  }

	  return res;
}

// read a size Byte big block beginning at the address.
static
DRESULT USB_disk_read(
	BYTE lun,			/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read */
	)
{
	  DRESULT res = RES_ERROR;
	  MSC_LUNTypeDef info;

	  //PRINTF("USB_disk_read: lun=%d, buff=%p, sector=%lu, count=%u\n", (int) lun, buff, (unsigned long) sector, (unsigned) count);
	  if(USBH_MSC_Read(&hUsbHostHS, lun, sector, buff, count) == USBH_OK)
	  {
	    res = RES_OK;
	  }
	  else
	  {
	    USBH_MSC_GetLUNInfo(&hUsbHostHS, lun, &info);

	    switch (info.sense.asc)
	    {
	    case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
	    case SCSI_ASC_MEDIUM_NOT_PRESENT:
	    case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
	    	PRINTF ("USB Disk is not ready!\n");
	      res = RES_NOTRDY;
	      break;

	    default:
	      res = RES_ERROR;
	      break;
	    }
	  }

	  return res;
}

/* запись буферизированных данных на носитель */
static
DRESULT USB_Sync(BYTE drv)
{
	//if (sdhost_sdcard_waitstatus() != 0)
	//	return RES_ERROR;
	return RES_OK;
}

// for _USE_MKFS
static
DRESULT USB_Get_Sector_Count (
	BYTE lun,		/* Physical drive nmuber (0..) */
	LBA_t  *buff	/* Data buffer to store read data */
	)
{
	DRESULT res;
	MSC_LUNTypeDef info;

    if (USBH_MSC_GetLUNInfo(&hUsbHostHS, lun, &info) == USBH_OK)
    {
    	if ((sizeof (LBA_t) <= sizeof (uint32_t)) && (info.capacity.block_nbr64 >= (uint64_t) 1 << 32))
    	{
    		res = RES_ERROR;	// Too large device
    	}
    	else
    	{
    		* buff = info.capacity.block_nbr64;
    		res = RES_OK;
    	}
    }
    else
    {
		res = RES_ERROR;
    }
	return res;
}

// for _USE_MKFS
static
DRESULT USB_Get_Block_Size(
	BYTE lun,		/* Physical drive nmuber (0..) */
	DWORD  *buff	/* Data buffer to store read data */
	)
{
	DRESULT res;
	MSC_LUNTypeDef info;

    if (USBH_MSC_GetLUNInfo(&hUsbHostHS, lun, &info) == USBH_OK)
    {
		* buff = info.capacity.block_size / USB_DEFAULT_BLOCK_SIZE;
		res = RES_OK;
    }
    else
    {
		res = RES_ERROR;
    }
	return res;
}


const struct drvfunc USBH_drvfunc =
{
	USB_Initialize,
	USB_Status,
	USB_Sync,
	USB_disk_write,
	USB_disk_read,
	USB_Get_Sector_Count,
	USB_Get_Block_Size,
};

#endif /* WITHUSEUSBFLASH */


#endif /* WITHUSBHW */
