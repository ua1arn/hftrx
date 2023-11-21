
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
#include "usbd_def.h"
//#include "usb_device.h"
#endif


#if WITHUSEUSBFLASH

#include "sdcard.h"
//#include "src/fatfs/ff.h"
#define USB_DEFAULT_BLOCK_SIZE 512

#if WITHTINYUSB && CFG_TUH_ENABLED


//--------------------------------------------------------------------+
//
//--------------------------------------------------------------------+
static volatile bool _disk_busy[CFG_TUH_DEVICE_MAX];

static scsi_inquiry_resp_t inquiry_resp;


bool inquiry_complete_cb(uint8_t dev_addr, tuh_msc_complete_data_t const * cb_data)
{
  msc_cbw_t const* cbw = cb_data->cbw;
  msc_csw_t const* csw = cb_data->csw;

  if (csw->status != 0)
  {
    PRINTF("Inquiry failed\r\n");
    return false;
  }
//
//  // Print out Vendor ID, Product ID and Rev
//  PRINTF("%.8s %.16s rev %.4s\r\n", inquiry_resp.vendor_id, inquiry_resp.product_id, inquiry_resp.product_rev);
//
//  // Get capacity of device
//  uint32_t const block_count = tuh_msc_get_block_count(dev_addr, cbw->lun);
//  uint32_t const block_size = tuh_msc_get_block_size(dev_addr, cbw->lun);
//
//  PRINTF("Disk Size: %lu MB\r\n", block_count / ((1024*1024)/block_size));
//  // PRINTF("Block Count = %lu, Block Size: %lu\r\n", block_count, block_size);
//
//  // For simplicity: we only mount 1 LUN per device
//  uint8_t const drive_num = dev_addr-1;
//  char drive_path[3] = "0:";
//  drive_path[0] += drive_num;
//
//  if ( f_mount(&fatfs[drive_num], drive_path, 1) != FR_OK )
//  {
//    puts("mount failed");
//  }
//
//  // change to newly mounted drive
//  f_chdir(drive_path);
//
//  // print the drive label
////  char label[34];
////  if ( FR_OK == f_getlabel(drive_path, label, NULL) )
////  {
////    puts(label);
////  }

  return true;
}

static uint8_t devad;


uint_fast8_t hamradio_get_usbh_active(void)
{
	return devad != 0 && tuh_msc_mounted(devad);
}

//------------- IMPLEMENTATION -------------//
void tuh_msc_mount_cb(uint8_t dev_addr)
{
  PRINTF("A MassStorage device is mounted, dev_addr=%u\n", dev_addr);

  uint8_t const lun = 0;
  tuh_msc_inquiry(dev_addr, lun, &inquiry_resp, inquiry_complete_cb, 0);
  devad = dev_addr;
}

void tuh_msc_umount_cb(uint8_t dev_addr)
{
	  devad = 0;
	  PRINTF("A MassStorage device is unmounted\r\n");
//
//  uint8_t const drive_num = dev_addr-1;
//  char drive_path[3] = "0:";
//  drive_path[0] += drive_num;
//
//  f_unmount(drive_path);
//
////  if ( phy_disk == f_get_current_drive() )
////  { // active drive is unplugged --> change to other drive
////    for(uint8_t i=0; i<CFG_TUH_DEVICE_MAX; i++)
////    {
////      if ( disk_is_ready(i) )
////      {
////        f_chdrive(i);
////        cli_init(); // refractor, rename
////      }
////    }
////  }
}

//--------------------------------------------------------------------+
// DiskIO
//--------------------------------------------------------------------+

static void wait_for_disk_io(BYTE pdrv)
{
  while(_disk_busy[pdrv])
  {
    tuh_task();
  }
}

static bool disk_io_complete(uint8_t dev_addr, tuh_msc_complete_data_t const * cb_data)
{
  (void) dev_addr; (void) cb_data;
  _disk_busy[dev_addr-1] = false;
  return true;
}

static DSTATUS USB_disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
  return devad && tuh_msc_mounted(devad) ? 0 : STA_NODISK;
}

static DSTATUS USB_disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
  (void) pdrv;
	return 0; // nothing to do
}

static DRESULT USB_disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	uint8_t const lun = 0;

	_disk_busy[pdrv] = true;
	tuh_msc_read10(devad, lun, buff, sector, (uint16_t) count, disk_io_complete, 0);
	wait_for_disk_io(pdrv);

	return RES_OK;
}

#if FF_FS_READONLY == 0

static DRESULT USB_disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	uint8_t const lun = 0;

	_disk_busy[pdrv] = true;
	tuh_msc_write10(devad, lun, buff, sector, (uint16_t) count, disk_io_complete, 0);
	wait_for_disk_io(pdrv);

	return RES_OK;
}

#endif

//static DRESULT USB_disk_ioctl (
//	BYTE pdrv,		/* Physical drive nmuber (0..) */
//	BYTE cmd,		/* Control code */
//	void *buff		/* Buffer to send/receive control data */
//)
//{
//  uint8_t const dev_addr = pdrv + 1;
//  uint8_t const lun = 0;
//  switch ( cmd )
//  {
//    case CTRL_SYNC:
//      // nothing to do since we do blocking
//      return RES_OK;
//
//    case GET_SECTOR_COUNT:
//      *((DWORD*) buff) = (WORD) tuh_msc_get_block_count(dev_addr, lun);
//      return RES_OK;
//
//    case GET_SECTOR_SIZE:
//      *((WORD*) buff) = (WORD) tuh_msc_get_block_size(dev_addr, lun);
//      return RES_OK;
//
//    case GET_BLOCK_SIZE:
//      *((DWORD*) buff) = 1;    // erase block size in units of sector size
//      return RES_OK;
//
//    default:
//      return RES_PARERR;
//  }
//
//	return RES_OK;
//}

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
	* buff = tuh_msc_get_block_count(devad, lun);
	return RES_OK;
}

// for _USE_MKFS
static
DRESULT USB_Get_Block_Size(
	BYTE lun,		/* Physical drive nmuber (0..) */
	DWORD  *buff	/* Data buffer to store read data */
	)
{
	* buff = tuh_msc_get_block_size(devad, lun);
	return RES_OK;
}

#else /* WITHTINYUSB */

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
DSTATUS USB_disk_status (
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


uint_fast8_t hamradio_get_usbh_active(void)
{
	return hUsbHostHS.device.is_connected != 0 && hUsbHostHS.gState == HOST_CLASS;
	return hUsbHostHS.device.is_connected != 0;
}

#endif /* WITHTINYUSB */

const struct drvfunc USBH_drvfunc =
{
	USB_Initialize,
	USB_disk_status,
	USB_Sync,
	USB_disk_write,
	USB_disk_read,
	USB_Get_Sector_Count,
	USB_Get_Block_Size,
};

#else

uint_fast8_t hamradio_get_usbh_active(void)
{
	return  0;
}

#endif /* WITHUSEUSBFLASH */


#endif /* WITHUSBHW */
