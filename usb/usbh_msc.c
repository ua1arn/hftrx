
/*
 * usbd_msc.c
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
*/

#include "hardware.h"
#include "pio.h"
#include "board.h"
#include "audio.h"
#include "spifuncs.h"

#include "display/display.h"
#include "formats.h"
#include <string.h>

#if WITHUSBHW

#include "usb_core.h"


#if 0 && WITHUSEUSBFLASH

#include "sdcard.h"
#include "fatfs/ff.h"

static volatile DSTATUS USB_Stat = STA_NOINIT;	/* Disk status */
extern TM_USB_MSCHOST_Result_t 	TM_USB_MSCHOST_INT_Result;

extern USB_OTG_CORE_HANDLE   USB_OTG_Core;
extern USBH_HOST             USB_Host;


static
DSTATUS USB_Initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
	)
{
	//PRINTF(PSTR("disk_initialize: drv=%d\n"), (int) drv);
	if (1)
	{
		if (HCD_IsDeviceConnected(&USB_OTG_Core) && TM_USB_MSCHOST_INT_Result == TM_USB_MSCHOST_Result_Connected) {
			USB_Stat &= ~ STA_NOINIT;
		} else {
			USB_Stat |= STA_NOINIT;
		}

		return USB_Stat;
	}
	return STA_NODISK;
}


static
DSTATUS USB_Status (
	BYTE drv		/* Physical drive nmuber (0..) */
	)
{
	//PRINTF(PSTR("disk_status: drv=%d\n"), (int) drv);
	if (1)
	{
		return USB_Stat;
		return 0;	// STA_NOINIT or STA_NODISK or STA_PROTECT
	}
	return STA_NODISK;
}

// write a size Byte big block beginning at the address.
static
DRESULT USB_disk_write(
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write */
	)
{
	BYTE status = USBH_MSC_OK;
	if (!count) {
		return RES_PARERR;
	}
	if (USB_Stat & STA_NOINIT) {
		return RES_NOTRDY;
	}
	if (TM_USB_MSCHOST_INT_Result == TM_USB_MSCHOST_Result_WriteProtected) {
		return RES_WRPRT;
	}

	if (HCD_IsDeviceConnected(&USB_OTG_Core) && TM_USB_MSCHOST_INT_Result == TM_USB_MSCHOST_Result_Connected) {
		do
		{
			status = USBH_MSC_Write10(&USB_OTG_Core, (BYTE*)buff, sector, 512 * count);
			USBH_MSC_HandleBOTXfer(&USB_OTG_Core, &USB_Host);

			if (!HCD_IsDeviceConnected(&USB_OTG_Core)) {
				return RES_ERROR;
			}
		} while (status == USBH_MSC_BUSY);
	}

	if (status == USBH_MSC_OK) {
		return RES_OK;
	}
	return RES_ERROR;
}

// read a size Byte big block beginning at the address.
static
DRESULT USB_disk_read(
	BYTE drv,			/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read */
	)
{
	BYTE status = USBH_MSC_OK;

	if (!count) {
		return RES_PARERR;
	}
	if (USB_Stat & STA_NOINIT) {
		return RES_NOTRDY;
	}

	if (HCD_IsDeviceConnected(&USB_OTG_Core) && TM_USB_MSCHOST_INT_Result == TM_USB_MSCHOST_Result_Connected) {
		do
		{
			status = USBH_MSC_Read10(&USB_OTG_Core, buff, sector, 512 * count);
			USBH_MSC_HandleBOTXfer(&USB_OTG_Core, &USB_Host);

			if (!HCD_IsDeviceConnected(&USB_OTG_Core)) {
				return RES_ERROR;
			}
		} while (status == USBH_MSC_BUSY);
	}

	if (status == USBH_MSC_OK) {
		return RES_OK;
	}
	return RES_ERROR;
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
	BYTE drv,		/* Physical drive nmuber (0..) */
	DWORD  *buff	/* Data buffer to store read data */
	)
{
	* buff = (DWORD) USBH_MSC_Param.MSCapacity;
	return RES_OK;
}

// for _USE_MKFS
static
DRESULT USB_Get_Block_Size (
	BYTE drv,		/* Physical drive nmuber (0..) */
	DWORD  *buff	/* Data buffer to store read data */
	)
{
	* buff = 512;
	return RES_OK;
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
