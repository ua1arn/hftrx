/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	/* sprintf() replacement */
#include <ctype.h>
#include <string.h>

#if 1//WITHUSEAUDIOREC

#include "board.h"
#include "sdcard.h"
#include "ff.h"	
#include "diskio.h"		/* FatFs lower layer API */

#include "src/display/display.h"	/* используем функцию получения рабочей частоты */
//#include "audio.h"

static const struct drvfunc * const drvfuncs [] =
{
#if WITHUSESDCARD && WITHSDHCHW
	& SD_drvfunc,
#endif /* WITHUSESDCARD */
#if WITHUSESDCARD && ! WITHSDHCHW
	& MMC_drvfunc,
#endif /* WITHUSESDCARD && ! WITHSDHCHW */
#if WITHUSEUSBFLASH
	& USBH_drvfunc,
#endif /* WITHUSEUSBFLASH */
};
/*
	TODO:

	If the card responds to CMD8, the response of ACMD41 includes the CCS field information. CCS is
	valid when the card returns ready (the busy bit is set to 1). CCS=0 means that the card is SDSC.
	CCS=1 means that the card is SDHC or SDXC.

 */


#if FF_MULTI_PARTITION	/* Volume - Partition resolution table */
PARTITION VolToPart[] = {
	{0, 0},	/* "0:" <== Disk# 0, auto detect */
	{1, 0},	/* "1:" <== Disk# 1, auto detect */
	{2, 0},	/* "2:" <== Disk# 2, auto detect */
	{3, 1},	/* "3:" <== Disk# 3, 1st partition */
	{3, 2},	/* "4:" <== Disk# 3, 2nd partition */
	{3, 3},	/* "5:" <== Disk# 3, 3rd partition */
	{4, 0},	/* "6:" <== Disk# 4, auto detect */
	{5, 0}	/* "7:" <== Disk# 5, auto detect */
};
#endif /* FF_MULTI_PARTITION */

/* Check physical drive status */
DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	//PRINTF(PSTR("disk_initialize drv=%u (n=%u)\n"), drv, (sizeof drvfuncs / sizeof drvfuncs [0]));
	if (drv < (sizeof drvfuncs / sizeof drvfuncs [0]))
		return drvfuncs [drv]->Initialize(drv);	// detect media
	return STA_NODISK;
}

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	if (drv < (sizeof drvfuncs / sizeof drvfuncs [0]))
		return drvfuncs [drv]->Status(drv);
	return STA_NODISK;
}

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read */
)
{
	if (drv < (sizeof drvfuncs / sizeof drvfuncs [0]))
		return drvfuncs [drv]->Disk_read(drv, buff, sector, count);
	return STA_NODISK;
}

DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write */
)
{
	if (drv < (sizeof drvfuncs / sizeof drvfuncs [0]))
		return drvfuncs [drv]->Disk_write(drv, buff, sector, count);
	return STA_NODISK;
}

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	if (drv < (sizeof drvfuncs / sizeof drvfuncs [0]))
	{
		switch (ctrl)
		{
		case CTRL_SYNC:
			return drvfuncs [drv]->Sync(drv);

		// for _USE_MKFS
		case GET_SECTOR_COUNT:
			return drvfuncs [drv]->Get_Sector_Count(drv, buff);

		// for _USE_MKFS
		case GET_BLOCK_SIZE:
			return drvfuncs [drv]->Get_Block_Size(drv, buff);

		case GET_SECTOR_SIZE:
			* (WORD *) buff = 512;
			return RES_OK;
		/*
		case CTRL_TRIM:
			return RES_PARERR;
		 */

		}
		PRINTF(PSTR("Unsupported ioctl: ctrl = %u\n"), ctrl);
		return RES_PARERR;
	}
	return STA_NODISK;
}


/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/

DWORD get_fattime (void)
{
	uint_fast16_t year;
	uint_fast8_t month, day;
	uint_fast8_t hour, minute, secounds;

	board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & secounds);

	return	  ((DWORD)(year - 1980) << 25)	/* Year = 2012 */
			| ((DWORD)month << 21)				/* Month = 1 */
			| ((DWORD)day << 16)				/* Day_m = 1*/
			| ((DWORD)hour << 11)				/* Hour = 0 */
			| ((DWORD)minute << 5)				/* Min = 0 */
			| ((DWORD)secounds >> 1);				/* Sec = 0 */
}

#endif /* WITHUSEAUDIOREC */
