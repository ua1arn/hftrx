
#ifndef SDCARD_H_INCLUDED
#define SDCARD_H_INCLUDED

#include <stdint.h>
#include "fatfs/ff.h"
#include "fatfs/diskio.h"


struct drvfunc
{
	DRESULT (* Initialize)(BYTE drv);	// detect media
	DRESULT (* Status)(BYTE drv);
	DRESULT (* Sync)(BYTE drv);
	DRESULT (* Disk_write) (
		BYTE drv,			/* Physical drive number (0..) */
		const BYTE *buff,	/* Data to be written */
		DWORD sector,		/* Sector address (LBA) */
		UINT count			/* Number of sectors to write */
		);
	DRESULT (* Disk_read) (
		BYTE drv,		/* Physical drive number (0..) */
		BYTE *buff,		/* Data buffer to store read data */
		DWORD sector,	/* Sector address (LBA) */
		UINT count		/* Number of sectors to read */
	);
	DRESULT (* Get_Sector_Count) (
		BYTE drv,		/* Physical drive number (0..) */
		DWORD  *buff	/* Data buffer to store read data */
	);
	DRESULT (* Get_Block_Size) (
		BYTE drv,		/* Physical drive number (0..) */
		DWORD  *buff	/* Data buffer to store read data */
	);
};

extern const struct drvfunc SD_drvfunc;
extern const struct drvfunc MMC_drvfunc;
extern const struct drvfunc USBH_drvfunc;

#define MMC_SECTORSIZE 512

void sdcardhw_initialize(void);
void sdcardinitialize(void);	// перевод state machine в начальное состояние
void sdcardbgprocess(void);
void sdcardtoggle(void);	// комбинированная функция для переключения запись/стоп
void sdcardrecord(void);	// функция "начать запись"
void sdcardstop(void);	// функция "остановить запись"
void sdcardformat(void);

#endif /* SDCARD_H_INCLUDED */
