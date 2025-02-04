
#ifndef SDCARD_H_INCLUDED
#define SDCARD_H_INCLUDED

#include <stdint.h>
#include "src/fatfs/ff.h"
#include "src/fatfs/diskio.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct drvfunc
{
	DSTATUS (* Initialize)(BYTE drv);	// detect media
	DSTATUS (* Status)(BYTE drv);
	DRESULT (* Sync)(BYTE drv);
	DRESULT (* Disk_write) (
		BYTE drv,			/* Physical drive number (0..) */
		const BYTE *buff,	/* Data to be written */
		LBA_t sector,		/* Sector address (LBA) */
		UINT count			/* Number of sectors to write */
		);
	DRESULT (* Disk_read) (
		BYTE drv,		/* Physical drive number (0..) */
		BYTE *buff,		/* Data buffer to store read data */
		LBA_t sector,	/* Sector address (LBA) */
		UINT count		/* Number of sectors to read */
	);
	DRESULT (* Get_Sector_Count) (
		BYTE drv,		/* Physical drive number (0..) */
		LBA_t  *buff	/* Data buffer to store read data */
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
void sdcardinitialize(void);	// перевод state machine в начальное состояние, назначение user mode обработчика
void sdcarddeinitialize(void);
void sdcardtoggle(void);	// комбинированная функция для переключения запись/стоп
void sdcardrecord(void);	// функция "начать запись"
void sdcardstop(void);	// функция "остановить запись"
void sdcardformat(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SDCARD_H_INCLUDED */
