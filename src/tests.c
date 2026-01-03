//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "synthcalcs.h"
#include "board.h"
#include "audio.h"
#include "keyboard.h"
#include "encoder.h"
#include "bootloader.h"

#include "fontsys.h"
#include "display2.h"
#include "formats.h"
#include "codecs.h"
#include "spi.h"
#include "gpio.h"
#include "utils.h"
#include "clocks.h"

#if WITHUSEFATFS
	#include "fatfs/ff.h"
#endif /* WITHUSEFATFS */
#if WITHUSEFATFS
	#include "sdcard.h"
#endif /* WITHUSEFATFS */
#if WITHTINYUSB
#include "tusb.h"
#endif /* WITHTINYUSB */

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
//#include <stdbool.h>

#define HALFCOUNT_SMALL 1

#if 0
// Печать параметров на экране
static void showpos(uint_fast8_t pos)
{
	//static unsigned  count;
	//count ++;
	char buff [22];

	local_snprintf_P(buff, sizeof buff / sizeof buff [0], 
		PSTR("POS %2u"), (unsigned) pos
		 );
	display_gotoxy(0, 0);
	display_text(buff, 0, & dbstylev);
}

#endif

#if 1 && WITHDEBUG && WITHUSEFATFS

struct fb
{
#if _USE_LFN
    char lfn [FF_MAX_LFN + 1];   /* Buffer to store the LFN */
#endif
	FILINFO fno;					/* File information object */
};

void fb_initialize(struct fb * p)
{
	//p->lfnamebuff [0] = '\0';
	//p->fno.lfname = p->lfnamebuff;
    //char *fn;   /* This function is assuming non-Unicode cfg. */
#if 0// _USE_LFN
    ///*static */char lfn[FF_MAX_LFN + 1];   /* Buffer to store the LFN */
    p->fno.lfname = p->lfn;
    p->fno.lfsize = sizeof p->lfn;
#endif
}

static RAMNOINIT_D1 FATFSALIGN_BEGIN uint8_t rbwruff [FF_MAX_SS * 32] FATFSALIGN_END;		// буфер записи - при совпадении с _MAX_SS нельзя располагать в Cortex-M4 CCM


static void showprogress(
	unsigned long filepos,
	unsigned long total
	)
{
}

static void printtextfile(const char * filename)
{
	unsigned long filepos = 0;	// количество выданных байтов
	unsigned long fulllength = 0;	// размер файла
	UINT br = 0;		//  количество считанных байтов
	UINT i = 0;			// номер выводимого байта
	
	FRESULT rc;				/* Result code */
	static RAMNOINIT_D1 FIL Fil;			/* Описатель открытого файла - нельзя располагать в Cortex-M4 CCM */
	// чтение файла
	rc = f_open(& Fil, filename, FA_READ);
	if (rc) 
	{
		PRINTF(PSTR("Can not open file '%s'\n"), filename);
		PRINTF(PSTR("Failed with rc=%u.\n"), rc);
		return;
	}
	
	// печать тестового файла
	PRINTF(PSTR("Type the file content: '%s'\n"), filename);
	for (;;)
	{
		char kbch;
		testsloopprocessing();		// обработка отложенного вызова user mode функций

		if (dbg_getchar(& kbch) != 0)
		{
			if (kbch == 0x1b)
				break;
		}

		if (i >= br)
		{
			// если буфер не заполнен - читаем
			rc = f_read(& Fil, rbwruff, sizeof rbwruff, &br);	/* Read a chunk of file */
			if (rc != FR_OK || !br)
				break;			/* Error or end of file */
			i = 0;		// начальное положение указателя в буфере для вывода данных
			showprogress(filepos, fulllength);
		}
		else if (0)	// "проглатываем" символы без отображения
		{
			++ i;
			showprogress(++ filepos, fulllength);
		}
		else if (dbg_putchar(rbwruff[i]))
		{
			++ i;
			showprogress(++ filepos, fulllength);
		}
	}

	PRINTF(PSTR("read complete: %lu bytes\n"), filepos);

	if (rc) 
	{
		TP();
		PRINTF(PSTR("Failed with rc=%u.\n"), rc);
		return;
	}

	//PRINTF("\nClose the file.\n");
	rc = f_close(& Fil);
	if (rc) 
	{
		TP();
		PRINTF(PSTR("Failed with rc=%u.\n"), rc);
		return;
	}
}

static void rxqclear(void)
{

}

static uint_fast8_t rxqpeek(char * ch)
{
	static int i = ' ';

	* ch = i;
	if (++ i == 0x80)
		i = ' ';
	return 1;
}

// сохранение потока данных с CNC на флэшке
static void dosaveserialport(const char * fname)
{
	static RAMNOINIT_D1 FIL Fil;			/* Описатель открытого файла - нельзя располагать в Cortex-M4 CCM */
	unsigned i;
	FRESULT rc;				/* Result code */

	rc = f_open(& Fil, fname, FA_WRITE | FA_CREATE_ALWAYS);
	if (rc)
	{
		PRINTF(PSTR("can not start recording\n"));
		return;	//die(rc);
	}
	PRINTF(PSTR("Write the file content: '%s'\n"), fname);

	rxqclear();	// очистить буфер принятых символов

	unsigned long filepos;	// количество сохранённых байтов
	filepos = 0;
	i = 0;
	showprogress(filepos, 0);
	for (;;)
	{
		char kbch;
		char c;

		testsloopprocessing();		// обработка отложенного вызова user mode функций
		if (dbg_getchar(& kbch) != 0)
		{
			if (kbch == 0x1b)
			{
				PRINTF(PSTR("break recording\n"));
				break;
			}
		}
		if (rxqpeek(& c) != 0)
		{
			// получен очередной символ из порта - сохраняем в буфер
			rbwruff [i ++] = c;
			showprogress(++ filepos, 0);
			if (i >= (sizeof rbwruff / sizeof rbwruff [0]))
			{
				UINT bw;
				rc = f_write(& Fil, rbwruff, i, & bw);
				if (rc != 0 || bw == 0)
					break;
				i = 0;
			}
		}
	}

	if (i != 0)
	{
		UINT bw;
		rc = f_write(& Fil, rbwruff, i, & bw);
		if (rc != 0 || bw != i)
		{
			TP();
			PRINTF(PSTR("Failed with rc=%u.\n"), rc);
			return;
		}
	}
	rc = f_close(& Fil);
	if (rc) 
	{
		TP();
		PRINTF(PSTR("Failed with rc=%u.\n"), rc);
		return;
	}
}

static volatile unsigned long recticks;
static volatile int recstop;

static void test_recodspool(void * ctx)
{
	if (recticks < NTICKS(60000))
	{
		++ recticks;
	}
	else
	{
		recstop = 1;
	}
}

static void test_recodstart(void)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	recticks = 0;
	recstop = 0;
	LowerIrql(oldIrql);
}

#if 1

#if WITHUSESDCARD && WITHUSEUSBFLASH
	// когда одновременно и eMMC и USB FLASH - 0:USB, 1: eMMC,

	#define VOLPREFIX "0:"					// префикс для файловых операций
	static const BYTE eMMCtargetdrv = 1;	// том для diskio функций
#else

	#define VOLPREFIX "0:"					// префикс для файловых операций
	static const BYTE eMMCtargetdrv = 0;	// том для diskio функций
#endif

// сохранение потока данных большими блоками
// 1 - конец циклпа проверок
static int dosaveblocks(const char * fname)
{
	int manualstop = 0;
	unsigned long long kbs = 0;
	static RAMNOINIT_D1 FATFS Fatfs;		/* File system object  - нельзя располагать в Cortex-M4 CCM */
	static RAMNOINIT_D1 FIL Fil;			/* Описатель открытого файла - нельзя располагать в Cortex-M4 CCM */
	FRESULT rc;				/* Result code */

	PRINTF(PSTR("FAT FS test - write file '%s'.\n"), fname);
	f_mount(& Fatfs, VOLPREFIX "", 0);		/* Register volume work area (never fails) */

	/* формирование сигнатуры в данных = для конроля достоверности записи */
	unsigned j;
	static unsigned i;
	static unsigned q;
	memset(rbwruff, 0x20, sizeof rbwruff);
	for (j = 0; j < ARRAY_SIZE(rbwruff) - 16; j += 80)
	{
		USBD_poke_u32_BE(rbwruff + j + 16 - 4, ++ q);
	}
	USBD_poke_u32_BE(rbwruff, ++ i);
	rc = f_open(& Fil, fname, FA_WRITE | FA_CREATE_ALWAYS);
	if (rc)
	{
		PRINTF("can not create file, rc=0x%02X\n", (unsigned) rc);
		return 1;	//die(rc);
	}

#if 1
	//409,337,856
	rc = f_expand(& Fil, 1uLL * 1024 * 1024 * 1024, 0);
	if (rc)
	{
		PRINTF("f_expand: rc=0x%02X\n", (unsigned) rc);
		return 1;	//die(rc);
	}
	else
	{
		//PRINTF("f_expand: rc=0x%02X\n", (unsigned) rc);
	}
#endif

#if 0
	enum { SZ_TBL = 8192 };
	static DWORD clmt [SZ_TBL];                    /* Cluster link map table buffer */
	Fil.cltbl = clmt;
	clmt [0] = SZ_TBL;                      /* Set table size */
	rc = f_lseek(& Fil, CREATE_LINKMAP);     /* Create CLMT */
	if (rc)
	{
		PRINTF("can not set clusters map recording, rc=0x%02X\n", (unsigned) rc);
		return;	//die(rc);
	}
	else
	{
		PRINTF("f_lseek info, clmt [0]=%u\n", (unsigned) clmt [0]);
	}
#endif

	test_recodstart();

	for (;;)
	{
		testsloopprocessing();		// обработка отложенного вызова user mode функций
		char kbch;
		char c;

		if (recstop != 0)
		{
			PRINTF("end of timed recording\n");
			break;
		}
		if (dbg_getchar(& kbch) != 0)
		{
			if (kbch == 0x1b)
			{
				PRINTF("break recording\n");
				manualstop = 1;
				break;
			}
		}
		UINT bw;
		rc = f_write(& Fil, rbwruff, sizeof rbwruff, & bw);
		if (rc != 0 || bw != sizeof rbwruff)
		{
			PRINTF("write fail, rc=0x%02X\n", (unsigned) rc);
			break;
		}
		kbs += bw;
	}
	rc = f_truncate(& Fil);
	if (rc)
	{
		PRINTF("f_truncate: rc=0x%02X\n", (unsigned) rc);
	}
	rc = f_close(& Fil);
	f_mount(NULL, VOLPREFIX "", 0);		/* Unregister volume work area (never fails) */
	if (rc)
	{
		TP();
		PRINTF("f_close failed, rc=0x%02X\n", (unsigned) rc);
		return 1;
	}
	else
	{
		PRINTF("Write speed %ld kB/S\n", (long) (kbs / 1000 / 60));
	}
	return manualstop;
}
#endif

#if 0

static const char * fb_getname(const struct fb * p)
{
	const char * fn;
	const FILINFO * pfno = & p->fno;
	#if _USE_LFN
		fn = *pfno->lfname ? pfno->lfname : pfno->fname;
	#else
		fn = pfno->fname;
	#endif

	return fn;
}

static void fb_print(const struct fb * p, int x, int y, int selected)
{
	gxstyle_t dbstylev;
	gxstyle_initialize(& dbstylev);
	char buff [100 + FF_MAX_LFN + 1];
	const char * fn = fb_getname(p);
	if (p->fno.fattrib & AM_DIR)
	{
		gxstyle_textcolor(& dbstylev, COLOR_GOLD, selected ? COLOR_BLUE: COLOR_BLACK);
		local_snprintf_P(buff, sizeof buff / sizeof buff [0], "%c   <dir>  %s", selected ? 'X' : ' ',
			fn);
		PRINTF(PSTR("   <dir>  %s\n"), p->fno.fname);
	}
	else
	{
		gxstyle_textcolor(& dbstylev, COLOR_GREEN, selected ? COLOR_BLUE: COLOR_BLACK);
		local_snprintf_P(
			buff,						// куда форматировать строку
			sizeof buff / sizeof buff [0],	// размер буфера
			"%c%8lu  %s", selected ? '>' : ' ',
			p->fno.fsize, fn);
		PRINTF(PSTR("%8lu  %s\n"), p->fno.fsize, p->fno.fname);
	}

}
#endif

// Печать имен файлов корневого каталога.
void displfiles_buff(const char* path)
{
    FRESULT res;
    //FILINFO fno;
    DIR dir;
#if _USE_LFN
    ///*static */char lfn[FF_MAX_LFN + 1];   /* Buffer to store the LFN */
    //fno.lfname = lfn;
    //fno.lfsize = sizeof lfn;
#endif

	PRINTF(PSTR("Open root directory '%s'.\n"), path);
    res = f_opendir(& dir, path);                       /* Open the directory */
    if (res == FR_OK) 
	{
		for (;;)
		{
			char *fn;   /* This function is assuming non-Unicode cfg. */
			struct fb fbt;
			FILINFO * pfno = & fbt.fno;
 			fb_initialize(& fbt);		// подготовка к использованию элемента буфера
            res = f_readdir(& dir, pfno);                   /* Read a directory item */
            if (res != FR_OK || pfno->fname[0] == 0) break;  /* Break on error or end of dir */
            if (pfno->fname[0] == '.') continue;             /* Ignore dot entry */
#if 0//_USE_LFN
            fn = *pfno->lfname ? pfno->lfname : pfno->fname;
#else
            fn = pfno->fname;
#endif
            if (pfno->fattrib & AM_DIR)                    /* It is a directory */
			{
                //sprintf(&path[i], "/%s", fn);
                //res = scan_files(path);
                //if (res != FR_OK) break;
                //path[i] = 0;
            } 
			else                                        /* It is a file. */
			{
                //PRINTF("%s/%s\n", path, fn);
 				PRINTF(PSTR("displfiles_buff: %9lu '%s'\n"), (unsigned long) pfno->fsize,  fn);
			}
        }
        //f_closedir(&dir);
    }
    else
    {
    	PRINTF(PSTR("Can not open root directory '%s', res=%d.\n"), path, (int) res);

    }
}

static char mmcInitialize(BYTE drv)
{
	DSTATUS st = disk_initialize (drv);				/* Physical drive nmuber (0..) */
	//PRINTF(PSTR("disk_initialize code=%02X\n"), st);
	return st != RES_OK;
}

// read a size Byte big block beginning at the address.
//char mmcReadBlock(uint_fast32_t address, unsigned long count, unsigned char *pBuffer);
static char mmcReadSector(BYTE drv, uint_fast32_t sector, unsigned char *pBuffer)
{
	DSTATUS st = disk_read(drv, pBuffer, sector, 1);
	//PRINTF(PSTR("disk_read code=%02X\n"), st);
	return st != RES_OK;
}
//#define 

// write a 512 Byte big block beginning at the (aligned) address
//char mmcWriteBlock (uint_fast32_t address, unsigned long count, const unsigned char *pBuffer);
static char mmcWriteSector(BYTE drv, uint_fast32_t sector, const unsigned char *pBuffer)
{
	DSTATUS st = disk_write(drv, pBuffer, sector, 1);
	//PRINTF(PSTR("disk_write code=%02X\n"), st);
	return st != RES_OK;
}

static uint_fast64_t mmcCardSize(BYTE drv)
{
	DWORD v;
	DSTATUS st = disk_ioctl(drv, GET_SECTOR_COUNT, & v);
	return st != RES_OK ? 0 : (uint_fast64_t) v * MMC_SECTORSIZE;
}

#if 0 && WITHDEBUG && WITHUSEFATFS
static void diskio_test(BYTE drv)
{
	const unsigned long MMC_SUCCESS2 = 0x00;
	unsigned long lba_sector = 0;
	static RAMNOINIT_D1 FATFSALIGN_BEGIN unsigned char sectbuffr [MMC_SECTORSIZE] FATFSALIGN_END;
	static RAMNOINIT_D1 FATFSALIGN_BEGIN unsigned char sectbuffw [MMC_SECTORSIZE] FATFSALIGN_END;

	PRINTF(PSTR("Test SD/eMMC storage (drv=%d)\n"), (int) drv);
	mmcInitialize(drv);
	{
		uint_fast64_t v = mmcCardSize(drv);
		PRINTF(PSTR("SD/eMMC storage size = %lu KB (%lu MB) (%08lx:%08lx bytes)\n"),
			(unsigned long) (v / 1024),
			(unsigned long) (v / 1024 / 1024),
			(unsigned long) (v >> 32),
			(unsigned long) (v >> 0));

	}

	PRINTF(PSTR("Enter command:\n"));
//test_disk();
//print_opened_files();
// SD CARD initializations done.
// тест записи/чтения на SD CARD

	static const uint8_t sectproto [MMC_SECTORSIZE] =
	{
			0xEB, 0xFE, 0x90, 0x4D, 0x53, 0x44, 0x4F, 0x53, 0x35, 0x2E, 0x30, 0x00, 0x02, 0x40, 0x86, 0x78,
			0x01, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x3F, 0x00, 0xFF, 0x00, 0x3F, 0x00, 0x00, 0x00,
			0xC1, 0x3F, 0xE7, 0x00, 0x3B, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
			0x01, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x80, 0x00, 0x29, 0x87, 0x4C, 0x4E, 0x57, 0x4E, 0x4F, 0x20, 0x4E, 0x41, 0x4D, 0x45, 0x20, 0x20,
			0x20, 0x20, 0x46, 0x41, 0x54, 0x33, 0x32, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA,

	};

	memset(sectbuffw, 0, sizeof sectbuffw);
	for (;;)
	{
		static int testdataval;
		int i;

		/* Обеспечение работы USER MODE DPC */
		uint_fast16_t kbch;
		uint_fast8_t kbready;
		processmessages(& kbch, & kbready);

		char c;
		if (dbg_getchar(& c))
		{
			switch (c)
			{
			default:
				PRINTF(PSTR("Undefined command letter with code 0x%02X\n"), (unsigned char) c);
				break;

			case 'x':
				PRINTF(PSTR("SD CARD test done.\n"));
				return;

			case '1':
				/* подготовка тестовых данных */
				PRINTF(PSTR("Fill write buffer by 0xff\n"));
				memset(sectbuffw, 0xff, MMC_SECTORSIZE);
				break;

			case '2':
				/* подготовка тестовых данных */
				PRINTF(PSTR("Fill write buffer by 0x00\n"));
				memset(sectbuffw, 0x00, MMC_SECTORSIZE);
				break;

			case '3':
				/* подготовка тестовых данных */
				PRINTF(PSTR("Fill write buffer by 0x55\n"));
				memset(sectbuffw, 0x55, MMC_SECTORSIZE);
				break;

			case '4':
				/* подготовка тестовых данных */
				PRINTF(PSTR("Fill write buffer by 0xaa\n"));
				memset(sectbuffw, 0xaa, MMC_SECTORSIZE);
				break;

			case 'c':
				/* копирование данных */
				PRINTF(PSTR("Copy read buffer to write buffer\n"));
				memcpy(sectbuffw, sectbuffr, MMC_SECTORSIZE);
				break;

			case '5':
				/* подготовка тестовых данных */
				PRINTF(PSTR("Fill write buffer by text data\n"));
				for (i = 0; i < 16; ++ i)
				{
					local_snprintf_P((char *) sectbuffw + i * 32, 32, PSTR("TEST DATA %9d"), testdataval ++);
				}
				local_snprintf_P((char *) sectbuffw, MMC_SECTORSIZE, PSTR("Sector %9lu " __TIME__ " " __DATE__ "  "), lba_sector);
				break;

			case '6':
				/* подготовка даннных как при форматировании FatFS */
				PRINTF(PSTR("Fill write buffer by FatFS data\n"));
				memcpy(sectbuffw, sectproto, MMC_SECTORSIZE);
				break;

			case 'r':
				PRINTF(PSTR("Read SD card, sector = %lu\n"), lba_sector);
				if (mmcReadSector(drv, lba_sector, sectbuffr) != MMC_SUCCESS2)
					PRINTF("Read error\n");
				else
					printhex(0, sectbuffr, MMC_SECTORSIZE);
				break;

			case 'w':
				/* запись тестовых данных */
				PRINTF(PSTR("Write SD card, sector = %lu\n"), lba_sector);
				if (mmcWriteSector(drv, lba_sector, sectbuffw) != MMC_SUCCESS2)
					PRINTF(PSTR("Write error\n"));
				else
					PRINTF(PSTR("Write Okay\n"));
				break;

			case 'v':
				PRINTF(PSTR("Verify SD card, sector = %lu\n"), lba_sector);
				if (mmcReadSector(drv, lba_sector, sectbuffr) != MMC_SUCCESS2)
					PRINTF(PSTR("Read error\n"));
				else if (memcmp(sectbuffr, sectbuffw, MMC_SECTORSIZE) == 0)
					PRINTF(PSTR("No errors\n"));
				else
					PRINTF(PSTR("Different data\n"));
				break;

			case 'n':
				PRINTF(PSTR("Next sector (%lu) on SD card\n"), ++ lba_sector);
				break;

			case 'p':
				PRINTF(PSTR("Previous sector (%lu) on SD card\n"), -- lba_sector);
				break;

			case 'u':
				lba_sector = BOOTLOADER_SELFSIZE / MMC_SECTORSIZE;
				PRINTF(PSTR("Set sector (%lu) on SD card\n"), lba_sector);
				break;

			//case 'Q':
			//	PRINTF(PSTR("Wait for ready\n"));
			//	while (SD_ReadCardSize() == 0 && dbg_getchar(& c) == 0)
					;
			case 'q':
				{
					uint_fast64_t v = mmcCardSize(drv);
					PRINTF(PSTR("SD/eMMC storage size = %lu KB (%lu MB) (%08lx:%08lx bytes)\n"),
						(unsigned long) (v / 1024), 
						(unsigned long) (v / 1024 / 1024), 
						(unsigned long) (v >> 32), 
						(unsigned long) (v >> 0));

				}
				break;
			//case 'D':
			//	PRINTF(PSTR("Wait for ready\n"));
			//	while (mmcCardSize() == 0 && dbg_getchar(& c) == 0)
			//		;
			case 'd':
				{
					uint_fast64_t pos;
					const uint_fast64_t v = mmcCardSize(drv);
					PRINTF(PSTR("Dump SD Card with size = %lu KB (%lu MB) (%08lx:%08lx bytes)\n"),
						(unsigned long) (v / 1024), 
						(unsigned long) (v / 1024 / 1024), 
						(unsigned long) (v >> 32), 
						(unsigned long) (v >> 0));

					for (pos = 0; pos < v; )
					{
						uint_fast16_t kbch;
						uint_fast8_t kbready;
						processmessages(& kbch, & kbready);
						// проверка прерывания работы с клавиатуры
						char c;
						if (dbg_getchar(& c))
						{
							dbg_putchar(c);
							if (c == 0x1b)
								break;
						}
						// работа
						const unsigned long sector = pos / MMC_SECTORSIZE;
						if (mmcReadSector(drv, sector, sectbuffr) != MMC_SUCCESS2)
						{
							PRINTF(PSTR("Read error\n"));
						}
						else
						{
							printhex(pos, sectbuffr, MMC_SECTORSIZE);
							pos += MMC_SECTORSIZE;
						}
					}
					PRINTF(PSTR("Done dumping.\n"));
				}
				break;

			case 'W':
				{
					//unsigned nsect = mmcCardSize(drv) / MMC_SECTORSIZE;
					unsigned nsect = 10 * 1024 * 2;	// 10M
					// Wipe SD
					PRINTF(PSTR("Wipe SD card - first %u sectors (by previously prepared data). Press 'y' for proceed\n"), nsect);
					char c = 0;
					for (;;)
					{
						/* Обеспечение работы USER MODE DPC */
						uint_fast16_t kbch;
						uint_fast8_t kbready;
						processmessages(& kbch, & kbready);

						if (dbg_getchar(& c))
						{
								break;
						}
					}
					if (c == 'y')
					{
						unsigned sector;
						sector = 0;
						if (mmcWriteSector(drv, sector, sectbuffw) != MMC_SUCCESS2)
						{
							PRINTF(PSTR("Write error ar sector %u\n"), sector);
							break;
						}
						sector = nsect - 1;
						if (mmcWriteSector(drv, sector, sectbuffw) != MMC_SUCCESS2)
						{
							PRINTF(PSTR("Write error ar sector %u\n"), sector);
							break;
						}
						PRINTF("GPT sectors erased\n");
						for (sector = 0; sector < nsect; )
						{
							/* Обеспечение работы USER MODE DPC */
							uint_fast16_t kbch;
							uint_fast8_t kbready;
							processmessages(& kbch, & kbready);
							// проверка прерывания работы с клавиатуры
							char c;
							if (dbg_getchar(& c))
							{
								dbg_putchar(c);
								if (c == 0x1b)
									break;
							}
							// работа
							if (mmcWriteSector(drv, sector, sectbuffw) != MMC_SUCCESS2)
							{
								PRINTF(PSTR("Write error ar sector %u\n"), sector);
								break;
							}
							else
							{
								++ sector;
								//sector += 8;
								if ((sector % 1024) == 0)
									dbg_putchar('.');
							}
						}
						PRINTF(PSTR("Done erasing.\n"));
					}
				}
				break;

			case 'z':
				mmcInitialize(drv);
				PRINTF(PSTR("mmcInitialize.\n"));
				break;
				
			//case 't':
			//	PRINTF("Card %s\n", mmcPing() ? "present" : "missing");
			//	break;
			}
		}
	}
}

static void fatfs_filesystest(int speedtest)
{
	FATFSALIGN_BEGIN BYTE work [FF_MAX_SS] FATFSALIGN_END;
	FRESULT rc;  
	static RAMNOINIT_D1 FATFS Fatfs;		/* File system object  - нельзя располагать в Cortex-M4 CCM */
	static const char testfile [] = VOLPREFIX "readme.txt";
	char testlog [FF_MAX_LFN + 1];
	//int nlog = 0;

	//mmcInitialize();
	PRINTF(PSTR("FAT FS test.\n"));
	f_mount(& Fatfs, VOLPREFIX "", 0);		/* Register volume work area (never fails) */

	for (;;)
	{
		/* Обеспечение работы USER MODE DPC */
		uint_fast16_t kbch;
		uint_fast8_t kbready;
		processmessages(& kbch, & kbready);
		char c;
		if (dbg_getchar(& c))
		{
			switch (c)
			{
			default:
				PRINTF(PSTR("Undefined command letter with code 0x%02X\n"), (unsigned char) c);
				break;

//			case 'q':
//				{
//					uint_fast64_t v = mmcCardSize();
//					PRINTF(PSTR("SD Card size = %lu KB (%lu MB) (%08lx:%08lx bytes)\n"),
//						(unsigned long) (v / 1024),
//						(unsigned long) (v / 1024 / 1024),
//						(unsigned long) (v >> 32),
//						(unsigned long) (v >> 0));
//
//				}
//				break;

//			case 'z':
//				mmcInitialize();
//				break;
				
			case 'x':
				rc = f_mount(NULL, VOLPREFIX "", 0);		/* Unregister volume work area (never fails) */
				if (rc != FR_OK)
				{
					PRINTF("f_mount error, rc=%d\n", (int) rc);
					//break;
				}
				PRINTF(PSTR("FAT FS test done.\n"));
				return;

			case 'd':
				PRINTF(PSTR("FAT FS test - display root directory.\n"));
				rc = f_mount(NULL, VOLPREFIX "", 0);		/* Unregister volume work area (never fails) */
				if (rc != FR_OK)
				{
					PRINTF("f_mount error, rc=%d\n", (int) rc);
					break;
				}
				rc = f_mount(& Fatfs, VOLPREFIX "", 0);		/* Register volume work area (never fails) */
				if (rc != FR_OK)
				{
					PRINTF("f_mount error, rc=%d\n", (int) rc);
					break;
				}
				displfiles_buff(VOLPREFIX "");	// Заполнение буфера имён файлов в памяти
				break;

			case 't':
				PRINTF(PSTR("FAT FS test - print file '%s'.\n"), testfile);
				rc = f_mount(NULL, VOLPREFIX "", 0);		/* Unregister volume work area (never fails) */
				if (rc != FR_OK)
				{
					PRINTF("f_mount error, rc=%d\n", (int) rc);
					break;
				}
				rc = f_mount(& Fatfs, VOLPREFIX "", 0);		/* Register volume work area (never fails) */
				if (rc != FR_OK)
				{
					PRINTF("f_mount error, rc=%d\n", (int) rc);
					break;
				}
				printtextfile(testfile);
				break;

			case 'F':
				PRINTF(PSTR("FAT FS formatting.\n"));
				rc = f_mount(NULL, VOLPREFIX "", 0);		/* Unregister volume work area (never fails) */
				if (rc != FR_OK)
				{
					PRINTF("f_mount error, rc=%d\n", (int) rc);
					break;
				}
				rc = f_mkfs("0:", NULL, work, sizeof (work));
				if (rc != FR_OK)
				{
					PRINTF(PSTR("f_mkfs error (rc=%d)\n"), rc);
				}
				else
				{
					PRINTF(PSTR("sdcardformat: f_mkfs okay\n"));
				}
				f_mount(& Fatfs, VOLPREFIX "", 0);		/* Register volume work area (never fails) */
				break;

			case 'w':
				{
					uint_fast16_t year;
					uint_fast8_t month, day;
					uint_fast8_t hour, minute, seconds;
					board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & seconds);
					static unsigned ser;
					local_snprintf_P(testlog, sizeof testlog / sizeof testlog [0],
						PSTR(VOLPREFIX "rec_%04d-%02d-%02d_%02d%02d%02d_%08X_%u.txt"),
						year, month, day,
						hour, minute, seconds,
						(unsigned) hardware_get_random(),
						++ ser
						);
					PRINTF(PSTR("FAT FS test - write file '%s'.\n"), testlog);
					rc = f_mount(NULL, VOLPREFIX "", 0);		/* Unregister volume work area (never fails) */
					if (rc != FR_OK)
					{
						PRINTF(PSTR("f_mount error (rc=%d)\n"), rc);
					}
					rc = f_mount(& Fatfs, VOLPREFIX "", 0);		/* Register volume work area (never fails) */
					if (rc != FR_OK)
					{
						PRINTF(PSTR("f_mount error (rc=%d)\n"), rc);
					}
					dosaveserialport(testlog);
				}
				break;
#if 0
			case 'W':
				if (speedtest)
				{
					uint_fast16_t year;
					uint_fast8_t month, day;
					uint_fast8_t hour, minute, seconds;
					board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & seconds);
					static unsigned ser;
					local_snprintf_P(testlog, sizeof testlog / sizeof testlog [0],
						PSTR(VOLPREFIX "rec_%04d-%02d-%02d_%02d%02d%02d_%08lX_%u.txt"),
						year, month, day,
						hour, minute, seconds,
						hardware_get_random(),
						++ ser
						);
					f_mount(NULL, VOLPREFIX "", 0);		/* Unregister volume work area (never fails) */
					dosaveblocks(testlog);
				}
				break;
#endif
			}
		}
	}
}

#if 1

static int fatfs_filesyspeedstest(void)
{
	uint_fast16_t year;
	uint_fast8_t month, day;
	uint_fast8_t hour, minute, seconds;
	board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & seconds);
	static unsigned ser;
	static const char testfile [] = "readme.txt";
	char testlog [FF_MAX_LFN + 1];
	//int nlog = 0;


	//mmcInitialize();
	local_snprintf_P(testlog, sizeof testlog / sizeof testlog [0],
		PSTR("rec_%04d-%02d-%02d_%02d%02d%02d_%08X_%u.txt"),
		year, month, day,
		hour, minute, seconds,
		(unsigned) hardware_get_random(),
		++ ser
		);
	return dosaveblocks(testlog);
}
#endif

#if CPUSTYLE_ALLWINNER

static void programming(FIL * f, unsigned offset, BYTE targetDEV)
{
	FRESULT rc;				/* Result code */
	DRESULT dc;
	unsigned score = 0;
	for (;;)
	{
		static BYTE buff [512 * 4];		/* File system object  - нельзя располагать в Cortex-M4 CCM */
		unsigned chunksize = sizeof buff;
		UINT br;
		//PRINTF("Write at %u.\n", score);
		rc = f_read(f, buff, chunksize, &br);	/* Read a chunk of file */
		if (rc != FR_OK || ! br)
			break;			/* Error or end of file */
		dc = disk_write(targetDEV, buff, (offset + score) / 512, chunksize / 512);
		if (dc != 0)
		{
			PRINTF("Write error");
			break;
		}
		score += chunksize;
	}
	PRINTF("%08X: %u bytes written.\n", offset, score);
}

static void verifying(FIL * f, unsigned offset, BYTE targetDEV)
{
	FRESULT rc;				/* Result code */
	DRESULT dc;
	unsigned score = 0;
	for (;;)
	{
		static BYTE buff [512 * 4];		/* File system object  - нельзя располагать в Cortex-M4 CCM */
		static BYTE buffchk [512 * 4];		/* File system object  - нельзя располагать в Cortex-M4 CCM */
		unsigned chunksize = sizeof buff;
		UINT br;
		//PRINTF("Write at %u.\n", score);
		rc = f_read(f, buff, chunksize, &br);	/* Read a chunk of file */
		if (rc != FR_OK || ! br)
			break;			/* Error or end of file */
		dc = disk_read(targetDEV, buffchk, (offset + score) / 512, chunksize / 512);
		if (dc != 0)
		{
			PRINTF("Read error");
			break;
		}
		if (memcmp(buff, buffchk, br))
		{
			PRINTF("Verificaton fault\n");
			return;
		}
		score += chunksize;
	}
	PRINTF("%08X: %u bytes verified.\n", offset, score);
}

static void
bootloaderFLASH(const char * volPrefix, BYTE targetDEV)
{
	static const struct base
	{
		unsigned offs;
		const char * name;
	} jobs [] =
	{
#if CPUSTYLE_ALLWINNER
		{	EMMC_EGON_OFFSET,	"fsbl.bt0",	},
#endif /* CPUSTYLE_ALLWINNER */
		{	BOOTLOADER_SELFSIZE,	"tc1_awt507_app.alw32", },
	};
	int i;
	FRESULT rc;				/* Result code */

	PRINTF("Flash files from USB FatFS storage, '%s' to device %d\n", volPrefix, (int) targetDEV);
	PRINTF("Press 'y' for contiunue\n");
	for (;;)
	{
		/* Обеспечение работы USER MODE DPC */
		uint_fast16_t kbch;
		uint_fast8_t kbready;
		processmessages(& kbch, & kbready);
		char c;
		if (dbg_getchar(& c))
		{
			switch (c)
			{
			default:
				return;
				break;
			case 'y':
				goto startProgramming;
			}
		}
	}

startProgramming:
	static RAMNOINIT_D1 FATFS Fatfs;		/* File system object  - нельзя располагать в Cortex-M4 CCM */

	if (disk_initialize(targetDEV) != 0)
	{
		PRINTF("No targed device\n");
		return;
	}
	rc = f_mount(& Fatfs, VOLPREFIX "", 0);		/* Unregister volume work area (never fails) */
	if (rc != FR_OK)
	{
		PRINTF("f_mount error, rc=%d\n", (int) rc);
		return;
	}
	PRINTF("Start...\n");
	displfiles_buff(volPrefix);	// Заполнение буфера имён файлов в памяти
	for (i = 0; i < ARRAY_SIZE(jobs); ++ i)
	{
		static RAMNOINIT_D1 FIL Fil;			/* Описатель открытого файла - нельзя располагать в Cortex-M4 CCM */
		static RAMNOINIT_D1 char filename [128];
		local_snprintf_P(filename, ARRAY_SIZE(filename), "%s%s", volPrefix, jobs [i].name);
		// чтение файла
		rc = f_open(& Fil, filename, FA_READ);
		if (rc != FR_OK)
		{
			PRINTF(PSTR("Can not open file '%s', rc=%u\n"), filename, rc);
			continue;
		}

		programming(& Fil, jobs [i].offs, targetDEV);
		rc = f_close(& Fil);
		if (rc != FR_OK)
		{
			PRINTF(PSTR("Can not close file '%s', rc=%u\n"), filename, rc);
			break;
		}


		// check written
		rc = f_open(& Fil, filename, FA_READ);
		if (rc != FR_OK)
		{
			PRINTF(PSTR("Can not open file '%s', rc=%u\n"), filename, rc);
			continue;
		}

		verifying(& Fil, jobs [i].offs, targetDEV);
		rc = f_close(& Fil);
		if (rc != FR_OK)
		{
			PRINTF(PSTR("Can not close file '%s', rc=%u\n"), filename, rc);
			break;
		}
	}
	rc = f_mount(NULL, VOLPREFIX "", 0);		/* Unregister volume work area (never fails) */
	PRINTF("Done\n");
}
#endif /* CPUSTYLE_ALLWINNER */

#endif

#endif /* WITHDEBUG && WITHUSEAUDIOREC */

//HARDWARE_SPI_HANGON()	- поддержка SPI программатора - подключение к программируемому устройству
//HARDWARE_SPI_HANGOFF() - поддержка SPI программатора - отключение от программируемого устройства

#if 0 && defined(targetdataflash)

static void spi_hangon(void)
{
	HARDWARE_SPI_HANGON();
	local_delay_ms(200);
}

static void spi_hangoff(void)
{
	HARDWARE_SPI_HANGOFF();
}

/////////

#define SPIMODE_AT26DF081A	SPIC_MODE3

static unsigned char spidataflash_read_status(
	spitarget_t target	/* addressing to chip */
	)
{
	unsigned char v;

	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x05);		/* read status register */

	spi_to_read(target);
	v = spi_read_byte(target, 0xff);
	spi_to_write(target);

	spi_unselect(target);	/* done sending data to target chip */

	return v;
}

static int spitimed_dataflash_read_status(
	spitarget_t target
	)
{
	unsigned long w = 40000;
	while (w --)
	{
		if ((spidataflash_read_status(target) & 0x01) == 0)
			return 0;
	}
	PRINTF(PSTR("DATAFLASH timeout error\n"));
	return 1;
}

static int spilargetimed_dataflash_read_status(
	spitarget_t target
	)
{
	unsigned long w = 40000000;
	while (w --)
	{
		if ((spidataflash_read_status(target) & 0x01) == 0)
			return 0;
	}
	PRINTF(PSTR("DATAFLASH erase timeout error\n"));
	return 1;
}

static int spitestchipDATAFLASH(void)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	unsigned char mf_id;	// Manufacturer ID
	unsigned char mf_devid1;	// device ID (part 1)
	unsigned char mf_devid2;	// device ID (part 2)
	unsigned char mf_dlen;	// Extended Device Information String Length


	/* Ожидание бита ~RDY в слове состояния. Для FRAM не имеет смысла.
	Вставлено для возможности использования DATAFLASH */

	if (spitimed_dataflash_read_status(target))
		return 1;

	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x9f);		/* read id register */

	spi_to_read(target);

	//prog_spi_to_read();
	mf_id = spi_read_byte(target, 0xff);
	mf_devid1 = spi_read_byte(target, 0xff);
	mf_devid2 = spi_read_byte(target, 0xff);
	mf_dlen = spi_read_byte(target, 0xff);

	spi_to_write(target);

	spi_unselect(target);	/* done sending data to target chip */

	PRINTF(PSTR("Read: ID = 0x%02X devId = 0x%02X%02X, mf_dlen=0x%02X\n"), mf_id, mf_devid1, mf_devid2, mf_dlen);
	//PRINTF(PSTR("Need: ID = 0x%02X devId = 0x%02X%02X, mf_dlen=0x%02X\n"), 0x1f, 0x45, 0x01, 0x00);
	return mf_id != 0x1f || mf_devid1 != 0x45 || mf_devid2 != 0x01 || mf_dlen != 0;
}

static int spieraseDATAFLASH(void)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x06);		/* write enable */
	spi_unselect(target);	/* done sending data to target chip */

	if (spitimed_dataflash_read_status(target))
		return 1;

	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x60);		/* chip erase */
	spi_unselect(target);	/* done sending data to target chip */

	if (spilargetimed_dataflash_read_status(target))
		return 1;

	if ((spidataflash_read_status(target) & (0x01 << 5)) != 0)	// write error
	{
		PRINTF(PSTR("Erase error\n"));
		return 1;
	}
	return 0;
}

static int spiprepareDATAFLASH(void)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x06);		/* write enable */
	spi_unselect(target);	/* done sending data to target chip */

	// Write Status Register
	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x01);		/* write status register */
	spi_progval8(target, 0x00);		/* write status register */
	spi_unselect(target);	/* done sending data to target chip */

	return 0;
}

static int spiwriteEnableDATAFLASH(void)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x06);		/* write enable */
	spi_unselect(target);	/* done sending data to target chip */

	return 0;
}

static int spiwriteDisableDATAFLASH(void)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x04);		/* write disable */
	spi_unselect(target);	/* done sending data to target chip */

	return 0;
}


static int writesinglepageDATAFLASH(unsigned long flashoffset, const unsigned char * data, unsigned long len)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	//PRINTF(PSTR(" Prog to address %08lX %02X\n"), flashoffset, len);

	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x06);		/* write enable */
	spi_unselect(target);	/* done sending data to target chip */

	// start byte programm
	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8_p1(target, 0x02);				/* Page Program */

	spi_progval8_p2(target, flashoffset >> 16);
	spi_progval8_p2(target, flashoffset >> 8);
	spi_progval8_p2(target, flashoffset >> 0);

	while (len --)
		spi_progval8_p2(target, (unsigned char) * data ++);	// data

	spi_complete(target);	/* done sending data to target chip */

	spi_unselect(target);	/* done sending data to target chip */

	/* Ожидание бита ~RDY в слове состояния. Для FRAM не имеет смысла.
	Вставлено для возможности использования DATAFLASH */

	if (spitimed_dataflash_read_status(target))
		return 1;

	//PRINTF(PSTR("Done programming\n"));
	return 0;
}

static int spiwriteDATAFLASH(void * ctx, unsigned long flashoffset, const unsigned char * data, unsigned long len)
{
	//PRINTF(PSTR("Write to address %08lX %02X\n"), flashoffset, len);
	while (len != 0)
	{
		unsigned long offset = flashoffset & 0xFF;
		unsigned long part = ulmin(len, ulmin(256, 256 - offset));

		int status = writesinglepageDATAFLASH(flashoffset, data, part);
		if (status != 0)
			return status;
		len -= part;
		flashoffset += part;
		data += part;
	}
	return 0;
}

static int spiverifyDATAFLASH(void * ctx, unsigned long flashoffset, const unsigned char * data, unsigned long len)
{
	unsigned long count;
	unsigned long err = 0;
	unsigned char v;
	spitarget_t target = targetdataflash;	/* addressing to chip */

	//PRINTF(PSTR("Compare from address %08lX\n"), flashoffset);

	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x03);		/* sequential read block */

	spi_progval8(target, flashoffset >> 16);
	spi_progval8(target, flashoffset >> 8);
	spi_progval8(target, flashoffset >> 0);

	spi_to_read(target);

	for (count = 0; count < len; ++ count)
	{
		v = spi_read_byte(target, 0xff);
		if (v != data [count])
		{
			PRINTF(PSTR("Data mismatch at %08lx: read=%02x, expected=%02x\n"), flashoffset + count, v, data [count]);
			err = 1;
			break;
		}
	}

	spi_to_write(target);

	spi_unselect(target);	/* done sending data to target chip */

	if (err)
		PRINTF(PSTR("Done compare, have errors\n"));

	return err;
}



/////////
static int
toprintcFLASH(int c)
{
	if (c < 0x20 || c >= 0x7f)
		return '.';
	return c;
}

static int
printhexDATAFLASH(unsigned long voffs, const unsigned char * buff, unsigned long length)
{
	unsigned i, j;
	unsigned rows = (length + 15) / 16;

	for (i = 0; i < rows; ++ i)
	{
		const int trl = ((length - 1) - i * 16) % 16 + 1;
		PRINTF(PSTR("%08lX "), voffs + i * 16);
		for (j = 0; j < trl; ++ j)
			PRINTF(PSTR(" %02X"), buff [i * 16 + j]);

		PRINTF(PSTR("%*s"), (16 - trl) * 3, "");

		PRINTF(PSTR("  "));
		for (j = 0; j < trl; ++ j)
			PRINTF(PSTR("%c"), toprintcFLASH(buff [i * 16 + j]));

		PRINTF(PSTR("\n"));
	}
	return 0;
}

//static unsigned long flashbase = 0x18000000ul;

enum
{
	HSINIT,
	HSPARSEADDR1, HSPARSEADDR2, HSPARSEADDR3, HSPARSEADDR4,
	HSPARSELEN1, HSPARSELEN2,
	HSPARSETYPE1, HSPARSETYPE2,
	HSPARSEDATA1, HSPARSEDATA2,
	HSPARSEBODY1, HSPARSEBODY2,
	HSPARSECKS1, HSPARSECKS2,
};

static int ascii2nibble(int c)
{
	if (isdigit(c))
		return c - '0';
	if (isupper(c))
		return c - 'A' + 10;
	if (islower(c))
		return c - 'a' + 10;
	return 0;
}

static int parsehex(void * ctx, const TCHAR * filename, int (* usedata)(void * ctx, unsigned long addr, const unsigned char * data, unsigned long length))
{
	int hexstate = HSINIT;
	unsigned long rowaddress = 0;
	unsigned long segaddress = 0;
	unsigned long length;
	unsigned long address;
	unsigned long type;
	unsigned long body2, body32;
	unsigned long count;
	unsigned long cksread;
	unsigned long ckscalc;
	unsigned char body [256];

	//unsigned long filepos = 0;	// количество выданных байтов
	UINT br = 0;		//  количество считанных байтов
	UINT i = 0;			// номер выводимого байта
	
	FRESULT rc;				/* Result code */
	static RAMNOINIT_D1 FIL Fil;			/* Описатель открытого файла - нельзя располагать в Cortex-M4 CCM */
	// чтение файла
	rc = f_open(& Fil, filename, FA_READ);
	if (rc) 
	{
		PRINTF(PSTR("Can not open file '%s'\n"), filename);
		PRINTF(PSTR("Failed with rc=%u.\n"), rc);
		return 1;
	}
	for (;;)
	{
		if (i >= br)
		{
			// если буфер не заполнен - читаем
			rc = f_read(& Fil, rbwruff, sizeof rbwruff, &br);	/* Read a chunk of file */
			if (rc || !br) 
				break;			/* Error or end of file */
			i = 0;		// начальное положение указателя в буфере для вывода данных
			//showprogress(filepos, fulllength);
		}
		else 
		{
			int c = rbwruff [i ++];
			if (c == EOF)
			{
				if (hexstate != HSINIT)
					PRINTF(PSTR("Incomplete file '%s'\n"), filename);
				break;
			}
			if (c == '\n' || c == '\r')
			{
				if (hexstate != HSINIT)
				{
					PRINTF(PSTR("Incomplete file '%s'\n"), filename);
					break;
				}
				continue;
			}

			switch (hexstate)
			{
			case HSINIT:
				if (c != ':')
				{
					PRINTF(PSTR("Incomplete file '%s'\n"), filename);
					break;
				}
				hexstate = HSPARSELEN1;
				address = 0;
				type = 0;
				count = 0;
				ckscalc = 0;
				length = 0;
				continue;

			case HSPARSELEN1:
				body2 = body2 * 16 + ascii2nibble(c);
				hexstate = HSPARSELEN2;
				continue;

			case HSPARSELEN2:
				body2 = body2 * 16 + ascii2nibble(c);
				ckscalc += (body2 & 0xff);
				length = body2 & 0x00ff;
				hexstate = HSPARSEADDR1;
				continue;

			case HSPARSEADDR1:
				body2 = body2 * 16 + ascii2nibble(c);
				hexstate = HSPARSEADDR2;
				continue;

			case HSPARSEADDR2:
				body2 = body2 * 16 + ascii2nibble(c);
				ckscalc += (body2 & 0xff);
				hexstate = HSPARSEADDR3;
				continue;

			case HSPARSEADDR3:
				body2 = body2 * 16 + ascii2nibble(c);
				hexstate = HSPARSEADDR4;
				continue;

			case HSPARSEADDR4:
				body2 = body2 * 16 + ascii2nibble(c);
				ckscalc += (body2 & 0xff);
				address = body2 & 0xffff;
				hexstate = HSPARSETYPE1;
				continue;

			case HSPARSETYPE1:
				body2 = body2 * 16 + ascii2nibble(c);
				hexstate = HSPARSETYPE2;
				continue;

			case HSPARSETYPE2:
				body2 = body2 * 16 + ascii2nibble(c);
				ckscalc += (body2 & 0xff);
				type = body2 & 0x00ff;

				if (length != 0)
				{
					hexstate = HSPARSEBODY1;
				}
				else
				{
					hexstate = HSPARSECKS1;
				}
				continue;

			case HSPARSEBODY1:
				body2 = body2 * 16 + ascii2nibble(c);
				hexstate = HSPARSEBODY2;
				continue;

			case HSPARSEBODY2:
				body2 = body2 * 16 + ascii2nibble(c);
				ckscalc += (body2 & 0xff);

				if (count < (sizeof body / sizeof body [0]))
					body [count ++] = body2 & 0xff;

				if (count >= length)
				{
					// last byte in body
					hexstate = HSPARSECKS1;
					body32 = body2;
				}
				else
					hexstate = HSPARSEBODY1;
				continue;

			case HSPARSECKS1:
				cksread = cksread * 16 + ascii2nibble(c);
				hexstate = HSPARSECKS2;
				continue;

			case HSPARSECKS2:
				cksread = cksread * 16 + ascii2nibble(c);
				ckscalc += (cksread & 0xff);
				hexstate = HSINIT;

				if ((ckscalc & 0xff) != 0)
				{
					PRINTF(PSTR("Wrong checksum in file '%s'\n"), filename);
					break;
				}
				// Use data
				switch (type)
				{
				case 0:
					// Data
					if (usedata(ctx, segaddress + address, body, count))
						break;
					continue;

				case 4:
					// Extended Linear Address
					segaddress = (0xffff & body32) << 16;
					continue;

				case 5:
					// Start Linear Address
					PRINTF(PSTR("Start Linear Address: %08lX\n"), body32);
					continue;

				case 1:
					//PRINTF("End of file record\n");
					continue;

				default:
					PRINTF(PSTR("record with code %02X\n"), (unsigned) type);
					break;
				}
				break;
			}
			break;
		}
	} // for each char in file

	if (rc) 
	{
		rc = f_close(& Fil);
		return 1;
	}

	//PRINTF("\nClose the file.\n");
	rc = f_close(& Fil);
	if (rc) 
	{
		return 1;
	}
	return 0;
}

static void
fatfs_proghexfile(void * ctx, const char * hexfile)
{
	spi_hangon();
	PRINTF(PSTR("SPI FLASH programmer\n"));
	//parsehex(hexfile, printhexDATAFLASH);

	// AT26DF081A chip write
	if (spitestchipDATAFLASH() == 0)
	{
		//spitestchipDATAFLASH();
		//parsehex(hexfile, printhexDATAFLASH);

		do 
		{
			PRINTF(PSTR("Prepare...\n"));
			if (spiprepareDATAFLASH()) break;
			PRINTF(PSTR("Erase...\n"));
			if (spieraseDATAFLASH()) break;
			if (spiwriteEnableDATAFLASH()) break;
			if (parsehex(NULL, hexfile, spiwriteDATAFLASH)) break;
			PRINTF(PSTR("Programming...\n"));
			if (spiwriteDisableDATAFLASH()) break;
			PRINTF(PSTR("Verify...\n"));
			if (parsehex(ctx, hexfile, spiverifyDATAFLASH)) break;
		} while (0);
		PRINTF(PSTR("SPI FLASH programmer done\n"));
	}
	spi_hangoff();	// после этого ничего не делаем - так как может опять включиться SPI - для работы с SD картой
	for (;;)
		;
}

static void
fatfs_progspi(void)
{
	static RAMNOINIT_D1 FATFS Fatfs;		/* File system object  - нельзя располагать в Cortex-M4 CCM */
	f_mount(& Fatfs, VOLPREFIX "", 0);		/* Register volume work area (never fails) */
	fatfs_proghexfile(NULL, "tc1_r7s721_rom.hex");
	f_mount(NULL, VOLPREFIX "", 0);		/* Unregister volume work area (never fails) */

	for (;;)
		;
}
#endif /* defined(targetdataflash) */

#if 0 && CPUSTYLE_R7S721

/////////////////////
static void leds(uint_fast8_t v)
{
	i2c_start(0x40);	// CAT9554 address
	i2c_write(0x01);	// #1 - output port address
	i2c_write(0x07 & ~ v);
	i2c_waitsend();
	i2c_stop();
}

static void ledsinit(void)
{
	i2c_start(0x40);	// CAT9554 address
	i2c_write(0x03);	// #3 - Configuration register
	i2c_write(~ 0x07);	// "0" - output, "1" - input
	i2c_waitsend();
	i2c_stop();
}

#endif

#define LED_TARGET_PORT_S(v) do { R7S721_TARGET_PORT_S(7, v); } while (0)
#define LED_TARGET_PORT_C(v) do { R7S721_TARGET_PORT_C(7, v); } while (0)

enum { LEDBIT = 1uL << 1 }; // P7_1
enum { SW1BIT = 1uL << 9 }; // P1_9

#if 0 && WITHDEBUG && WITHUART1HW
// "трассировка" с помощью выдачи на SPI порт информации о сработавших прерываниях
static void test_spi_trace(uint_fast8_t v)
{
	const spitarget_t target = targetctl1;
	//
	spi_select(target, SPIC_MODE3);
	spi_progval8_p1(target, v);
	spi_complete(target);
	spi_unselect(target);
}
//++++++++++++++++

enum { qSZ = 8 };
static volatile uint_fast8_t queue [qSZ];
static volatile uint_fast8_t qp, qg;
static void	qput(uint_fast8_t c)
{
	uint_fast8_t next = (qp + 1) % qSZ;
	if (next != qg)
	{
		queue [qp] = c;
		qp = next;
	}
}

static uint_fast8_t qget(uint_fast8_t * pc)
{
	if (qp != qg)
	{
		* pc = queue [qg];
		qg = (qg + 1) % qSZ;
		return 1;
	}
	return 0;
}

static uint_fast8_t qempty(void)
{
	return qp == qg;
}

// ---------
static volatile uint_fast8_t rxcount, txcount, rxerrcount;

static int cat3_puts_impl_P(const char * s)
{
	char c;
	while ((c = * s ++) != '\0')
	{
		while (HARDWARE_DEBUGSIRQ_PUTCHAR(c) == 0)
			;
	}
	return 0;
}

/* вызывается из обработчика прерываний */
// произошла потеря символа (символов) при получении данных с CAT компорта
void cat3_rxoverflow(void)
{
	++ rxerrcount;
}

/* вызывается из обработчика прерываний */
void cat3_disconnect(void)
{
}

void cat3_parsechar(uint_fast8_t c)
{
	//while (hardware_usart1_putchar(c) == 0)
	//	;
	//return;
	++ rxcount;
	//test_spi_trace((rxcount & 0x0f) * 16 + (txcount & 0x0f));

	if (c == 0x1b)
	{
		qput('<');
		qput('e');
		qput('s');
		qput('c');
		qput('>');
	}
	else if (c == 0x0d)
	{
		qput('<');
		qput('c');
		qput('r');
		qput('>');
	}
	else if (c == 0x0a)
	{
		qput('<');
		qput('l');
		qput('f');
		qput('>');
	}
	else
		qput(c);
	if (! qempty())
		HARDWARE_DEBUGSIRQ_ENABLETX(1);
}

void cat3_sendchar(void * ctx)
{
	++ txcount;
	//test_spi_trace((rxcount & 0x0f) * 16 + (txcount & 0x0f));

	uint_fast8_t c;
	if (qget(& c))
	{
		HARDWARE_DEBUGSIRQ_TX(ctx, c);
		if (qempty())
			HARDWARE_DEBUGSIRQ_ENABLETX(0);
	}
	else
	{
		HARDWARE_DEBUGSIRQ_ENABLETX(0);
	}
}

static void serial_irq_loopback_test(void)
{
	//test_spi_trace((rxcount & 0x0f) * 16 + (txcount & 0x0f));

	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	HARDWARE_DEBUGSIRQ_INITIALIZE();
	HARDWARE_DEBUGSIRQ_SET_SPEED(DEBUGSPEED);
	HARDWARE_DEBUGSIRQ_ENABLERX(1);
	LowerIrql(oldIrql);
	cat3_puts_impl_P(PSTR("Serial port ECHO test (with IRQ).\r\n"));
	for (;;)
	{
	}

}

#endif /* WITHDEBUG && WITHUART1HW */

// Периодически вызывается в главном цикле
void looptests(void)
{
#if 0
	{
		// Encoder tests
		PRINTF("ef1=%+3d, ef2=%+3d, ef3=%+3d, ef4=%+3d\n",
				(int) encoder_get_delta(& encoder_ENC1F),
				(int) encoder_get_delta(& encoder_ENC2F),
				(int) encoder_get_delta(& encoder_ENC3F),
				(int) encoder_get_delta(& encoder_ENC4F)
				);
	}
#endif
#if CPUSTYLE_T507 && 0		// Allwinner T507 Thermal sensor test
	if ((THS->THS_DATA_INTS & 0x01) != 0)
	{
		THS->THS_DATA_INTS = 0x01;
		const uint32_t t = (THS->THSx_DATA[0]) & 0xFFF;
		PRINTF("t=%08" PRIx32 ", %2.1f\n", t, ((float) t - 3255.0f) / -12.401f);
	}
#endif
#if 0 && WITHFQMETER
	{
		const uint32_t v = board_get_fqmeter();
		PRINTF("fq=%08" PRIX32 " (%" PRIu32 ")\n", v, v);
	}
#endif
#if 0
	{
		// Display SOF frequency on USB device
		// Also set:
		//	hpcd_USB_OTG.Init.Sof_enable = DISABLE;
		unsigned v = hamradio_get_getsoffreq();
		PRINTF("SofFreq=%u\n", v);
	}
#endif
#if 0 && defined (KI_LIST) // && WITHKEYBOARD && KEYBOARD_USE_ADC
	{
		// Тестирование АЦП клавиатуры
		static const uint8_t adcinputs [] =
		{
			KI_LIST
		};
		unsigned i;

		for (i = 0; i < ARRAY_SIZE(adcinputs); ++ i)
		{
			const uint_fast8_t adci = adcinputs [i];

			PRINTF("%2u adci%2d %08lX\n", i, (unsigned) adci, (unsigned long) board_getadc_unfiltered_1 /* true */value(adci));
		}
	}
#endif
#if 0
	{
		// вычисления с плавающей точкой
		//
		//				   1.4142135623730950488016887242096981L
		// #define M_SQRT2  1.41421356237309504880

		//original: sqrt(2)=1.41421356237309514547462
		//double:   sqrt(2)=1.41421356237309514547462
		//float:    sqrt(2)=1.41421353816986083984375
		char b [64];

		//snprintf(b, sizeof b / sizeof b [0], "%u\n", (unsigned) SCB_GetFPUType());
		//PRINTF(PSTR("SCB_GetFPUType: %s"), b);

		snprintf(b, sizeof b / sizeof b [0], "sqrt(2)=%1.23f\n", (double) 1.41421356237309504880);
		PRINTF(PSTR("original: %s"), b);

		volatile double d0 = 2;
		volatile double d = sqrt(d0);
		snprintf(b, sizeof b / sizeof b [0], "sqrt(2)=%1.23f\n", d);
		PRINTF(PSTR("double:   %s"), b);

		volatile float f0 = 2;
		volatile float f = sqrtf(f0);
		snprintf(b, sizeof b / sizeof b [0], "sqrt(2)=%1.23f\n", f);
		PRINTF(PSTR("float:    %s"), b);
	}
#endif
#if 0
	// Failt handlers test
	// Data abort test
	* (int volatile *) 0x00000100 = 44;

	// Preefetch abort test
	typedef void (* pfn)(void);
	const pfn p = (pfn) 0x30000000uL;
	(p)();

#endif
#if 0 && WITHINTEGRATEDDSP && WITHDEBUG
	{
		dsp_speed_diagnostics();	// печать в последовательный порт результатов диагностики
	}
#endif
#if 0
	{
		PRINTF("TCONTV_PTR->TV_DEBUG_REG=%08X\n", (unsigned) TCONTV_PTR->TV_DEBUG_REG);
		//printhex32(0x00000000065100f0, 0x00000000065100f0, 16);

	}
#endif
#if 1 && WITHINTEGRATEDDSP && WITHDEBUG
	{
		// See buffers2.cpp - WITHBUFFERSDEBUG
		buffers_diagnostics();
		audio_diagnostics();
	}
#endif
#if 0 && WITHCURRLEVEL
	{
		// Тестирование датчика тока
		(void) hamradio_get_pacurrent_value();
	}
#endif
#if 0 && WITHVOLTLEVEL
	{
		// Тестирование датчика напряжния
		(void) hamradio_get_volt_value();
	}
#endif
#if 0 && WITHTEMPSENSOR
	{
		// Тестирование датчика температуры CPU STM32
		// TODO: пока не показывает температуру!
		const uint_fast8_t tempi = TEMPIX;
		const adcvalholder_t v = board_getadc_unfiltered_truevalue(tempi);
		// Измерение опрного напряжения
		const uint_fast8_t vrefi = VREFIX;
		const adcvalholder_t ref = board_getadc_unfiltered_truevalue(vrefi);	// текущее значение данного АЦП
		if (ref != 0)
		{
			const long Vref_mV = (uint_fast32_t) board_getadc_fsval(vrefi) * WITHREFSENSORVAL / ref;
			// Temperature (in °C) = {(V25 - VSENSE) / Avg_Slope} + 25.
			// Average slope = 4.3
			// Voltage at 25 °C = 1.43 V
			const long celsius = (1430 - (v * Vref_mV / board_getadc_fsval(tempi))) / 430 + 2500;
			PRINTF(PSTR("celsius=%3ld.%02ld\n"), celsius / 100, celsius % 100);
		}
	}
#endif
}

#if 0 && CTLSTYLE_V1V
// "прерыватель"

static void showstate(
	unsigned offtime,
	unsigned ontime
	)
{
	char buff [32];


	local_snprintf_P(buff, 32, PSTR(" ON: %4u0 mS"), ontime);
	display_gotoxy(0, 0);
	display_text(buff, 0, & dbstylev);


	local_snprintf_P(buff, 32, PSTR("OFF: %4u0 mS"), offtime);
	display_gotoxy(0, 1);
	display_text(buff, 0, & dbstylev);

}


static unsigned modif (unsigned v, unsigned pos, unsigned dd, unsigned width)
{
	unsigned d = (dd % width);
	unsigned leftpos = pos * width;
	unsigned left = v / leftpos;
	unsigned right = v % pos;
	return left * leftpos + (((v / pos) + d) % width) * pos + right;

}

#endif /* 1 && CTLSTYLE_V1V */

#if 0

// PG7: fpga ok
// PG6: sys ok
// PG3: dsp ok
// PG2: res ok
enum { MFPGA = 1ul << 7 };
enum { MSYS = 1ul << 6 };
enum { MDSP = 1ul << 3 };
enum { MRES = 1ul << 2 };

static void sdfault(void)
{
	PRINTF(PSTR("sdfault\n"));
	arm_hardware_piog_outputs(MRES, MRES);
	for (;;)
		;
}

static void sdtick(void)
{
	GPIOG->ODR ^= MDSP;	// Debug LEDs
}
#endif

#if LCDMODE_COLORED && ! DSTYLE_G_DUMMY



// Рисуем на основном экране цветной прямоугольник.
// x2, y2 - координаты второго угла (не входящие в закрашиваемый прямоугольник)
static void display_solidbar(
	uint_fast16_t x,
	uint_fast16_t y,
	uint_fast16_t x2,
	uint_fast16_t y2,
	COLORPIP_T color
	)
{
	gxdrawb_t dbv;
	gxdrawb_initialize(& dbv, colmain_fb_draw(), DIM_X, DIM_Y);
	if (x2 < x)
	{
		const uint_fast16_t t = x;
		x = x2, x2 = t;
	}
	if (y2 < y)
	{
		const uint_fast16_t t = y;
		y = y2, y2 = t;
	}
	colpip_fillrect(& dbv, x, y, x2 - x, y2 - y, color);
}


// Получение псевдослучайныз чисел.
// 0 .. num-1
static int local_randomgr(unsigned long num)
{

	static unsigned long rand_val = 123456uL;

	if (rand_val & 0x80000000UL)
		rand_val = (rand_val << 1);
	else	rand_val = (rand_val << 1) ^ 0x201051uL;

	return (rand_val % num);

}


/*                                                                      */
/*      RANDOMBARS: Display random bars                                 */
/*                                                                      */

static void BarTest(void)
{
	//PRINTF("BarTest\n");
	int forever = 0;
	unsigned n = 20000;
	for (;forever || n --;)
	{                    /* Until user enters a key...   */
		const int r = local_randomgr(256);
		const int g = local_randomgr(256);
		const int b = local_randomgr(256);

		const COLORPIP_T color = TFTRGB(r, g, b);

		int x = local_randomgr(DIM_X);
		int y = local_randomgr(DIM_Y);
		int x2 = local_randomgr(DIM_X);
		int y2 = local_randomgr(DIM_Y);

		display_solidbar(x, y, x2, y2, color);	// MDMA работает минуя кеш-память

		colmain_nextfb();
		//local_delay_ms(5);
		testsloopprocessing();		// обработка отложенного вызова user mode функций
	}

	//getch();             /* Pause for user's response    */
}

static  void
GridTest(void)
{
	gxdrawb_t dbv;
	gxdrawb_initialize(& dbv, colmain_fb_draw(), DIM_X, DIM_Y);

	PRINTF("GridTest\n");
	board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
	board_update();

	int     xm, ym, xm4, ym4;
	int xm1, ym1;
	unsigned long col1, col20, col21, col22, col23, col3;
	int     n, k;

	col1 = TFTRGB(192,192,192);

	col20 = TFTRGB(64,128,128);
	col21 = TFTRGB(128,64,128);
	col22 = TFTRGB(128,128,64);
	col23 = TFTRGB(64,64,64);

	col3 = TFTRGB(0,192,192);


	xm = DIM_X - 1;
	ym = DIM_Y - 1;
	xm4 = xm / 4;
	ym4 = ym / 4;
	xm1 = xm / 40;
	ym1 = ym / 40;

	/* Filled rectangle - all screen. */
	display_solidbar(0, 0, xm, ym, col1);
	/* Filled rectangle at right-down corner. */
	display_solidbar(xm4 * 3 + xm1, ym4 * 3 + ym1, xm4 * 4 - xm1, ym4 * 4 - ym1, col20);
	/* Filled rectangle at right-upper corner. */
	display_solidbar(xm4 * 3 + xm1, ym1, xm4 * 4 - xm1, ym4 - ym1, col21);
	/* Filled rectangle at left - down corner. */
	display_solidbar(xm1, ym4 * 3 + ym1, xm4 - xm1, ym4 * 4 - ym1, col22);
	/* Filled rectangle at center. */
	display_solidbar(xm4 + xm1, ym4 + ym1, xm4 * 3 - xm1, ym4 * 3 - ym1, col23);


	// Тест порядка цветов в пикселе
	const unsigned yrct0 = DIM_Y / 4;
	const unsigned xrct0 = DIM_X / 4;
	colpip_fillrect(& dbv, xrct0, yrct0 * 1, xrct0, yrct0, COLORPIP_RED);
	colpip_fillrect(& dbv, xrct0, yrct0 * 2, xrct0, yrct0, COLORPIP_GREEN);
	colpip_fillrect(& dbv, xrct0, yrct0 * 3, xrct0, yrct0, COLORPIP_BLUE);
/*
	const unsigned yg0 = DIM_Y / 24;
	const unsigned xg0 = DIM_X / 30;
	for (k = 0; k < 16; ++ k)
		for (n = 0; n < 16; ++ n)
			display_solidbar(n * xg0,
				 k * yg0,
				 n * xg0 + xg0,
				 k * yg0 + yg0,
				 TFTRGB(n * 16, k * 16, 255 - (n * 8 + k * 8) )
				 );
*/

	colpip_fillrect(& dbv, xm * 4 / 10, 0, xm * 3 / 10, ym * 2 / 10, COLORPIP_WHITE);
	display_line(& dbv, xm * 6 / 10,  0, xm * 6 / 10, ym,  COLORPIP_RED);

	/* Interlase clocke test.	*/
	display_line(& dbv, 10,  0,  xm, 10 + 1,  col3);
	display_line(& dbv, 10,  0,  xm, 10 + 3,  col3);
	display_line(& dbv, 10,  0,  xm, 10 + 5,  col3);
	display_line(& dbv, 10,  0,  xm, 10 + 7,  col3);

	/* diagonales test.	*/
	display_line(& dbv, xm, 0,  xm, ym, col3);
	display_line(& dbv, xm, ym, 0,  ym, col3);
	display_line(& dbv, 0,  ym, 0,  0,  col3);
	display_line(& dbv, 0,  0,  xm, ym, col3);
	display_line(& dbv, 0,  ym, xm, 0,  col3);

	// тест перестановки байтов при выборке видеоконтроллером
	const unsigned rctx = DIM_X / 3;
	const unsigned rcty = DIM_Y / 3;
	display_line(& dbv, rctx, rcty,  rctx * 2 - 1, rcty * 2 - 1, COLORPIP_BLACK);
	display_line(& dbv, rctx, rcty * 2 - 1, rctx * 2 - 1,  rcty, COLORPIP_BLACK);

	colmain_nextfb();

	for (;;)
		;
}

#endif /* LCDMODE_COLORED && ! DSTYLE_G_DUMMY */

#if 0
// MCU_AHB_SRAM - 96k

static void memfill(unsigned k)
{
	unsigned long offset = k * 1024uL;

	((volatile uint8_t *) 0x20000000) [offset + 4] = 0xAA;
	((volatile uint8_t *) 0x20000000) [offset + 5] = 0x55;
	((volatile uint32_t *) 0x20000000) [offset + 0] = offset;
}

static int memprobe(unsigned k)
{
	unsigned long offset = k * 1024uL;

	const uint_fast8_t ok1 = ((volatile uint8_t *) 0x20000000) [offset + 4] == 0xAA;
	const uint_fast8_t ok2 = ((volatile uint8_t *) 0x20000000) [offset + 5] == 0x55;
	const uint_fast8_t ok3 = ((volatile uint32_t *) 0x20000000) [offset + 0] == offset;
	return ok1 && ok2 && ok3;
}
#endif


#if 0 && WITHDEBUG
// FPU speed test

#define FFTZS 8192
typedef struct
{
	float r;
	float i;
} cplxf;

static RAMFRAMEBUFF ALIGNX_BEGIN cplxf src [FFTZS] ALIGNX_END;
static RAMFRAMEBUFF ALIGNX_BEGIN cplxf dst [FFTZS] ALIGNX_END;
static RAMDTCM ALIGNX_BEGIN cplxf refv [FFTZS] ALIGNX_END;

static void RAMFUNC_NONILINE cplxmla(cplxf *s, cplxf *d, cplxf *ref, int len) {
	while (len--) {
		d->r += s->r * ref->r - s->i * ref->i;
		d->i += s->i * ref->r + s->r * ref->i;
		++s;  ++d; ++ref;
	}
}

static void RAMFUNC_NONILINE cplxmlafast(cplxf *s, cplxf *d, cplxf *ref, int len) {
	int i;
	for (i = 0; i < len; ++ i) {
		d [i].r += s [i].r * ref [i].r - s [i].i * ref [i].i;
		d [i].i += s [i].i * ref [i].r + s [i].r * ref [i].i;
	}
}

static void RAMFUNC_NONILINE cplxmlasave(cplxf *d, int len) {
	while (len--) {
		volatile float t;
		t = d->r;
		t = d->i;
	}
}

#endif

#if defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U)

static void disableAllIRQs(void)
{
	IRQ_Disable(43);	// DMA1_Stream0_IRQn
	IRQ_Disable(47);	// DMA1_Stream4_IRQn
	IRQ_Disable(89);	// DMA2_Stream1_IRQn
	IRQ_Disable(100);	// DMA2_Stream5_IRQn
//	IRQ_Disable(106);	// USBH_OHCI_IRQn
//	IRQ_Disable(107);	// USBH_EHCI_IRQn
	IRQ_Disable(82);	// TIM5_IRQn systick
//	IRQ_Disable(61);	// TIM3_IRQn elkey
	IRQ_Disable(99);	// EXTI9_IRQn
	IRQ_Disable(109);	// EXTI13_IRQn
	IRQ_Disable(50);	// ADC1_IRQn
	IRQ_Disable(122);	// ADC2_IRQn
	IRQ_Disable(130);	// OTG_IRQn

	// Get ITLinesNumber
	const unsigned n = ((GIC_DistributorInfo() & 0x1f) + 1) * 32;
	unsigned i;
	// 32 - skip SGI handlers (keep enabled for CPU1 start).
	for (i = 32; i < n; ++ i)
	{
		if (IRQ_GetEnableState(i))
			PRINTF("disableAllIRQs: active=%u // IRQ_Disable(%u); \n", i, i);
		IRQ_Disable(i);
	}
	PRINTF("disableAllIRQs: n=%u\n", n);

}

#endif /* defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U) */


#if 1

void setnormal(float * p, int state)
{
	int dir = state % 2;
	int ix = state / 3;
	p [0] = 0;
	p [1] = 0;
	p [2] = 0;

	p [ix] = dir ? 1 : -1;
}

//
//	Нормали Треугольников
//	Нормаль к плоскости — это единичный вектор который направлен перпендикулярно к этой плоскости.
//	Нормаль к треугольнику — это единичный вектор направленный перпендикулярно к треугольнику.
//	Нормаль очень просто рассчитывается с помощью векторного произведения двух сторон
//	треугольника(если вы помните, векторное произведение двух векторов дает нам
//	перпендикулярный вектор к обоим) и нормализованный: его длина устанавливается в единицу.
//	Вот псевдокод вычисления нормали:
//
//	треугольник( v1, v2, v3 )
//	сторона1 = v2-v1
//	сторона2 = v3-v1
//	треугольник.нормаль = вектПроизведение(сторона1, сторона2).нормализировать()

void calcnormaface(const float * model, int * faces, float normal [3])
{
	int nvertixes = model [0];
	int nfaces = model [1];
	const float * const bufvertixes = model + 2;
	const float * const buffaces = bufvertixes + (nvertixes * 6);

	int i;
	for (i = 0; i < 3; ++ i)
	{
		int face = faces [i];
	}
}

void calcnormaltriangle(const float * model)
{
	int nvertixes = model [0];
	int nfaces = model [1];
	const float * const bufvertixes = model + 2;
	const float * const buffaces = bufvertixes + (nvertixes * 6);
	int i;
	int faces [nvertixes][3];
	int foundfaces = 0;

	for (i = 0; i < nvertixes; ++ i)
	{
		int j;
		// Ищем faces, имеющие в себе данную вершину.
		for (j = 0; j < nfaces && foundfaces < 3; ++ j)
		{
			if (
					buffaces [j * 6 + 0] == i ||
					buffaces [j * 6 + 1] == i ||
					buffaces [j * 6 + 2] == i ||
					0)
			{
				faces [i][foundfaces ++] = j;	// Нашли треугольник содержащий вершину
			}
		}
		if (foundfaces != 3)
		{
			PRINTF("nvertixes = %d: faces should be 3 (foundfaces=%d)\n", nvertixes, foundfaces);
			return;
		}

		//calcfacesnormal(model, )
	}
	for (i = 0; i < nvertixes; ++ i)
	{
		float normal [3][3];
		int j;
		// расчитать нормаль треугольников
		for (j = 0; j < 3; ++ j)
		{
			calcnormaface(model, faces [i], normal [j]);
		}
	}
//	int x1 = verticexs [0];
//	int y1 = verticexs [1];
//	int z1 = verticexs [3];
}
//	Вершинная Нормаль
//	Это нормаль введенная для удобства вычислений.
//	Это комбинированная нормаль от нормалей окружающих данную вершину треугольников.
//	Это очень удобно, так как в вершинных шейдерах мы имеем дело с вершинами,
//	а не с треугольниками.
//	В любом случае в OpenGL у мы почти никогда и не имеем дела с треугольниками.
//
//	вершина v1, v2, v3, ....
//	треугольник tr1, tr2, tr3 // они все используют вершину v1
//	v1.нормаль = нормализовать( tr1.нормаль + tr2.нормаль + tr3.нормаль)
#endif

#if 0 && WITHDEBUG && WITHSMPSYSTEM

static void SecondCPUTaskSGI13(void)
{
	const int cpu = arm_hardware_cpuid();
	PRINTF("mSGI13 Run debug thread test: I am CPU=%d\n", cpu);
}

static void SecondCPUTaskSGI15(void)
{
	const int cpu = arm_hardware_cpuid();
	PRINTF("mSGI15 Run debug thread test: I am CPU=%d\n", cpu);
}

#endif

#if 0
// See also https://www.analog.com/media/en/technical-documentation/data-sheets/ADIS16137.pdf
static void adis161xx_write16(unsigned page, unsigned addr, unsigned value)
{
	enum { WRITEFLAG = 0x80 };
	const uint_fast8_t spispeedindex = SPIC_SPEED1M;
	const spi_modes_t spimode = SPIC_MODE3;
	const unsigned wrriteflag = 1;
	const spitarget_t cs = targetext2;
	unsigned v1, v2;

	spi_select2(cs, spimode, spispeedindex);
	spi_read_byte(cs, WRITEFLAG | 0x00);	// 0x80: write, addr=0x00: page_id register
	spi_read_byte(cs, page);	// page value
	spi_unselect(cs);

	spi_select2(cs, spimode, spispeedindex);
	spi_read_byte(cs, WRITEFLAG | (addr & 0x7F));
	spi_read_byte(cs, value >> 0);
	spi_unselect(cs);

	spi_select2(cs, spimode, spispeedindex);
	spi_read_byte(cs, WRITEFLAG | ((addr & 0x7F) + 1));
	spi_read_byte(cs, value >> 8);
	spi_unselect(cs);

	//local_delay_ms(10);

}

// See also https://www.analog.com/media/en/technical-documentation/data-sheets/ADIS16137.pdf
static unsigned adis161xx_read16(unsigned page, unsigned addr)
{
	enum { WRITEFLAG = 0x80 };
	const uint_fast8_t spispeedindex = SPIC_SPEED1M;
	const spi_modes_t spimode = SPIC_MODE3;
	const unsigned wrriteflag = 0;
	const spitarget_t cs = targetext2;
	unsigned v1, v2;

	spi_select2(cs, spimode, spispeedindex);
	spi_read_byte(cs, WRITEFLAG | 0x00);	// 0x80: write, addr=0x00: page_id register
	spi_read_byte(cs, page);	// page value
	spi_unselect(cs);

	spi_select2(cs, spimode, spispeedindex);
	spi_read_byte(cs, addr & 0x7F);
	spi_read_byte(cs, 0);
	spi_unselect(cs);

	spi_select2(cs, spimode, spispeedindex);
	v1 = spi_read_byte(cs, 0);
	v2 = spi_read_byte(cs, 0);
	spi_unselect(cs);

	return v1 * 256 + v2;
}

// See also https://www.analog.com/media/en/technical-documentation/data-sheets/ADIS16137.pdf
static uint_fast32_t adis161xx_read32(unsigned page, unsigned addr)
{
	enum { WRITEFLAG = 0x80 };
	const uint_fast8_t spispeedindex = SPIC_SPEED1M;
	const spi_modes_t spimode = SPIC_MODE3;
	const unsigned wrriteflag = 0;
	const spitarget_t cs = targetext2;
	unsigned v1, v2, v3, v4;

	spi_select2(cs, spimode, spispeedindex);
	spi_read_byte(cs, WRITEFLAG | 0x00);	// 0x80: write, addr=0x00: page_id register
	spi_read_byte(cs, page);	// page value
	spi_unselect(cs);

	// LOW part of pair
	spi_select2(cs, spimode, spispeedindex);
	spi_read_byte(cs, (addr & 0x7F) + 0);
	spi_read_byte(cs, 0);
	spi_unselect(cs);

	spi_select2(cs, spimode, spispeedindex);
	v1 = spi_read_byte(cs, 0);
	v2 = spi_read_byte(cs, 0);
	spi_unselect(cs);

	// OUT part of pair
	spi_select2(cs, spimode, spispeedindex);
	spi_read_byte(cs, (addr & 0x7F) + 2);
	spi_read_byte(cs, 0);
	spi_unselect(cs);


	spi_select2(cs, spimode, spispeedindex);
	v3 = spi_read_byte(cs, 0);
	v4 = spi_read_byte(cs, 0);
	spi_unselect(cs);

	return
			((uint_fast32_t) v1) << 8 |
			((uint_fast32_t) v2) << 0 |
			((uint_fast32_t) v3) << 24 |
			((uint_fast32_t) v4) << 16 |
			0;
}

#endif

#if CPUSTYLE_F133

// https://github.com/bluespec/CLINT/blob/main/src/CLINT_AXI4.bsv

//#define RISCV_MSIP0 (CLINT_BASE  + 0x0000)
#define RISCV_MTIMECMP_ADDR (CLINT_BASE  + 0x4000)

// На Allwinner F133-A доступ к регистрам таймера только 32-х битный
static uint64_t mtimer_get_raw_time_cmp(void) {
#if 0//( __riscv_xlen == 64)
    // Directly read 64 bit value
    volatile uint64_t *mtime = (volatile uint64_t *)(RISCV_MTIMECMP_ADDR);
    return *mtime;
#elif 1//( __riscv_xlen == 32)
    volatile uint32_t * mtimel = (volatile uint32_t *)(RISCV_MTIMECMP_ADDR);
    volatile uint32_t * mtimeh = (volatile uint32_t *)(RISCV_MTIMECMP_ADDR+4);
    uint32_t mtimeh_val;
    uint32_t mtimel_val;
    do {
        // There is a small risk the mtimeh will tick over after reading mtimel
        mtimeh_val = *mtimeh;
        mtimel_val = *mtimel;
        // Poll mtimeh to ensure it's consistent after reading mtimel
        // The frequency of mtimeh ticking over is low
    } while (mtimeh_val != *mtimeh);
    return (uint64_t) ( ( ((uint64_t)mtimeh_val)<<32) | mtimel_val);
#else
    return 888;
#endif
}

// На Allwinner F133-A доступ к регистрам таймера только 32-х битный
static void mtimer_set_raw_time_cmp(uint64_t new_mtimecmp) {
#if 0//(__riscv_xlen == 64)
    // Single bus access
    volatile uint64_t *mtimecmp = (volatile uint64_t*)(RISCV_MTIMECMP_ADDR);
    *mtimecmp = new_mtimecmp;
#elif 1//( __riscv_xlen == 32)
    volatile uint32_t *mtimecmpl = (volatile uint32_t *)(RISCV_MTIMECMP_ADDR);
    volatile uint32_t *mtimecmph = (volatile uint32_t *)(RISCV_MTIMECMP_ADDR+4);
    // AS we are doing 32 bit writes, an intermediate mtimecmp value may cause spurious interrupts.
    // Prevent that by first setting the dummy MSB to an unacheivable value
    *mtimecmph = 0xFFFFFFFF;  // cppcheck-suppress redundantAssignment
    // set the LSB
    *mtimecmpl = (uint32_t)(new_mtimecmp & 0x0FFFFFFFFUL);
    // Set the correct MSB
    *mtimecmph = (uint32_t)(new_mtimecmp >> 32); // cppcheck-suppress redundantAssignment
#else
#endif
}
#endif /* CPUSTYLE_F133 */

#if 0 && LCDMODE_LTDC

// PNG files test

#include "lupng.h"

// PNG files test
void testpng(const void * pngbuffer)
{
	PACKEDCOLORPIP_T * const fb = colmain_fb_draw();
	LuImage * png = luPngReadMemory((char *) pngbuffer);	// Read data in DE2_FORMAT_XBGR_8888 format

	PACKEDCOLORPIP_T * const fbpic = (PACKEDCOLORPIP_T *) png->data;
	const COLORPIP_T keycolor = TFTRGB(png->data [0], png->data [1], png->data [2]);	/* угловой пиксель - надо правильно преобразовать из ABGR*/
	const unsigned picdx = png->width;//GXADJ(png->width);
	const unsigned picw = png->width;
	const unsigned pich = png->height;
	gxdrawb_t dbv_fb;
	gxdrawb_initialize(& dbv_fb, fb, DIM_X, DIM_Y);
	gxdrawb_t dbv_fbpic;
	gxdrawb_initialize(& dbv_fbpic, fbpic, png->width, png->height);

	PRINTF("testpng: sz=%u data=%p, dataSize=%u, depth=%u, w=%u, h=%u\n", (unsigned) sizeof fbpic [0], png, (unsigned) png->dataSize,  (unsigned) png->depth, (unsigned) png->width, (unsigned) png->height);

	colpip_fillrect(& dbv_fb, 0, 0, DIM_X, DIM_Y, COLORPIP_GRAY);

	colpip_stretchblt(
		dbv_fb.cachebase, dbv_fb.cachesize,
		& dbv_fb,
		0, 0, picw / 4, pich / 4,		/* позиция и размеры прямоугольника - получателя */
		dbv_fbpic.cachebase, dbv_fbpic.cachesize,
		& dbv_fbpic,
		0, 0, picdx, pich,
		BITBLT_FLAG_NONE | 1*BITBLT_FLAG_CKEY | 1*BITBLT_FLAG_SRC_ABGR8888, keycolor
		);

	colpip_stretchblt(
		dbv_fb.cachebase, dbv_fb.cachesize,
		& dbv_fb,
		30, 0, picw / 2, pich / 2,		/* позиция и размеры прямоугольника - получателя */
		dbv_fbpic.cachebase, dbv_fbpic.cachesize,
		& dbv_fbpic,
		0, 0, picdx, pich,
		BITBLT_FLAG_NONE | 0*BITBLT_FLAG_CKEY | 1*BITBLT_FLAG_SRC_ABGR8888, keycolor
		);

	colpip_stretchblt(
		dbv_fb.cachebase, dbv_fb.cachesize,
		& dbv_fb,
		30, pich / 2, picw / 2, pich / 2,		/* позиция и размеры прямоугольника - получателя */
		dbv_fbpic.cachebase, dbv_fbpic.cachesize,
		& dbv_fbpic,
		0, 0, picdx, pich,
		BITBLT_FLAG_NONE | 1*BITBLT_FLAG_CKEY | 1*BITBLT_FLAG_SRC_ABGR8888, keycolor
		);

	colpip_stretchblt(
		dbv_fb.cachebase, dbv_fb.cachesize,
		& dbv_fb,
		300, 100, picw / 1, pich / 1,		/* позиция и размеры прямоугольника - получателя */
		dbv_fbpic.cachebase, dbv_fbpic.cachesize,
		& dbv_fbpic,
		0, 0, picdx, pich,
		BITBLT_FLAG_NONE | 1*BITBLT_FLAG_CKEY | 1*BITBLT_FLAG_SRC_ABGR8888, keycolor
		);

	dcache_clean((uintptr_t) fb,  GXSIZE(DIM_X, DIM_Y) * sizeof fb [0]);
	hardware_ltdc_main_set4(RTMIXIDLCD, (uintptr_t) fb, (uintptr_t) 0, 0*(uintptr_t) 0, 0*(uintptr_t) 0);

	luImageRelease(png, NULL);
	for (;;)
		;
}

// PNG files test, no transparency (no key color)
void testpng_no_stretch(const void * pngbuffer, int useKeyColor)
{
	PACKEDCOLORPIP_T * const fb = colmain_fb_draw();
	LuImage * png = luPngReadMemory((char *) pngbuffer);	// Read data in DE2_FORMAT_XBGR_8888 format

	PACKEDCOLORPIP_T * const fbpic = (PACKEDCOLORPIP_T *) png->data;
	const COLORPIP_T keycolor = TFTRGB(png->data [0], png->data [1], png->data [2]);	/* угловой пиксель - надо правильно преобразовать из ABGR*/
	const unsigned picdx = png->width;//GXADJ(png->width);
	const unsigned picw = png->width;
	const unsigned pich = png->height;
	gxdrawb_t dbv_fb;
	gxdrawb_initialize(& dbv_fb, fb, DIM_X, DIM_Y);
	gxdrawb_t dbv_fbpic;
	gxdrawb_initialize(& dbv_fbpic, fbpic, png->width, png->height);

	PRINTF("testpng: sz=%u data=%p, dataSize=%u, depth=%u, w=%u, h=%u\n", (unsigned) sizeof fbpic [0], png, (unsigned) png->dataSize,  (unsigned) png->depth, (unsigned) png->width, (unsigned) png->height);
	PRINTF("testpng: dim_x=%u, dim_y=%u\n", DIM_X, DIM_Y);

	//colpip_fillrect(fb, DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, COLORPIP_GRAY);

	if (1)
	{
		colpip_bitblt(
			dbv_fb.cachebase, dbv_fb.cachesize,
			& dbv_fb,
			0, 0,			/* позиция прямоугольника - получателя */
			dbv_fbpic.cachebase, dbv_fbpic.cachesize,
			& dbv_fbpic,
			0, 0, picdx, pich,	/* позиция прямоугольника и размеры источника */
			BITBLT_FLAG_NONE | !! useKeyColor * BITBLT_FLAG_CKEY | 1*BITBLT_FLAG_SRC_ABGR8888, keycolor
			//BITBLT_FLAG_NONE | 0*BITBLT_FLAG_CKEY, keycolor
			);
	}
	else
	{
		colpip_stretchblt(
			dbv_fb.cachebase, dbv_fb.cachesize,
			& dbv_fb,
			0, 0, DIM_X, DIM_Y,		/* позиция и размеры прямоугольника - получателя */
			dbv_fbpic.cachebase, dbv_fbpic.cachesize,
			& dbv_fbpic,
			0, 0, picdx, pich,
			BITBLT_FLAG_NONE | !! useKeyColor * BITBLT_FLAG_CKEY | 1*BITBLT_FLAG_SRC_ABGR8888, keycolor
			);

	}

	if (0)
	{
		// Convert ABGR8888 to ARGB8888
		for (unsigned y = 0; y < DIM_Y; ++ y)
		{
			for (unsigned x = 0; x < DIM_X; ++ x)
			{
				PACKEDCOLORPIP_T * const p = colpip_mem_at(& dbv_fb, x, y);
				unsigned a = (* p >> 24) & 0xFF;
				unsigned b = (* p >> 16) & 0xFF;
				unsigned g = (* p >> 8) & 0xFF;
				unsigned r = (* p >> 0) & 0xFF;
				* p = TFTALPHA(a, TFTRGB(r, g, b));
			}
		}
	}

	colmain_nextfb();

	luImageRelease(png, NULL);
}

#endif

#if (__CORTEX_A == 53U) && CPUSTYLE_CA53 && (! defined(__aarch64__))

// 4.5.80 Configuration Base Address Register
/** \brief  Get CBAR
    \return               Configuration Base Address Register
    MRC p15, 1, <Rt>, c15, c3, 0; Read CBAR into Rt
 */
__STATIC_FORCEINLINE uint32_t __get_CA53_CBAR(void)
{
	uint32_t result;
  __get_CP(15, 1, result, 15, 3, 0);
  return(result);
}

// G8.2.112 MIDR, Main ID Register
// FAULT!!!
__STATIC_FORCEINLINE uint32_t __get_MIDR(void)
{
	uint32_t result;
  __get_CP(15, 0, result, 0, 0, 0);
  return(result);
}

#endif /* (__CORTEX_A == 53U) && CPUSTYLE_CA53 */

#if (CPUSTYLE_T113 || CPUSTYLE_F133) && 0

#define GPASDC_Vref 1800.f

///GPADC_DATA=Vin/Vref*4095

void gpadc_initialize(void)
{

	CCU->GPADC_BGR_REG |= (UINT32_C(1) << 16); // 1: De-assert reset  HOSC
	CCU->GPADC_BGR_REG |= (UINT32_C(1) << 0); // 1: Pass clock

	GPADC->GP_SR_CON |= (0x2fu << 0);
	GPADC->GP_CTRL |= (0x2u << 18); // continuous mode

	GPADC->GP_CS_EN |= (UINT32_C(1) << 0); // enable

	GPADC->GP_CTRL |= (UINT32_C(1) << 17); // calibration

	GPADC->GP_CTRL |= (UINT32_C(1) << 16);

	while ((GPADC->GP_DATA_INTS) & (1uL << 0))	///if 1 complete
		;

}

void gpadc_test(void)
{
	float Vin;
	Vin = GPADC->GP_CH0_DATA * GPASDC_Vref / 4095.f * 2.8f; ///2.8f my divide resistors

	PRINTF("5V= %d \n", (int) Vin);
}

void gpadc_inthandler(void)
{
	PRINTF("%ld\n", GPADC->GP_CH0_DATA);
}

#endif /* (CPUSTYLE_T113 || CPUSTYLE_F133) */

#if CPUSTYLE_T113

/* HiFI4 DSP-viewed address offset translate to host cpu viewwed */
static ptrdiff_t xlate_dsp2mpu(ptrdiff_t a)
{
	const ptrdiff_t BANKSIZE = 0x08000u;
	const ptrdiff_t CELLBASE = 0x10000u;
	const ptrdiff_t CELLSIZE = 16;
	const ptrdiff_t cellbank = (a - CELLBASE) / BANKSIZE;
	const ptrdiff_t cellrow = (a - CELLBASE) % BANKSIZE / CELLSIZE;	/* гранулярность 16 байт */
	const unsigned cellpos = (a % CELLSIZE);	/* гранулярность 16 байт */

	if (a < CELLBASE)
		return a;	/* translation not needed. */

	return CELLBASE +
			cellbank * BANKSIZE +
			CELLSIZE * ((cellrow % 2) ? (cellrow / 2) + (BANKSIZE / CELLSIZE / 2) : cellrow / 2) +
			cellpos;
}

/* memcpy replacement for Allwinner T113-s3 dsp memory */
static void copy2dsp(uint8_t * pdspmap, const uint8_t * pcpu, unsigned offs, unsigned size)
{
	for (; size --; ++ offs)
	{
		pdspmap [xlate_dsp2mpu(offs)] = pcpu [offs];
	}
}

/* memset replacement for Allwinner T113-s3 dsp memory */
static void zero2dsp(uint8_t * pdspmap, unsigned offs, unsigned size)
{
	for (; size --; ++ offs)
	{
		pdspmap [xlate_dsp2mpu(offs)] = 0x00;	/* fill by zero */
	}
}

//static void xtest(void)
//{
//	unsigned mpu;
//	unsigned dsp;
//
//	dsp = 0x10000;
//	mpu = xlate_dsp2mpu(dsp);
//	PRINTF("dsp=%08X, mpu=%08X\n", dsp, mpu);
//
//	dsp = 0x10010;
//	mpu = xlate_dsp2mpu(dsp);
//	PRINTF("dsp=%08X, mpu=%08X\n", dsp, mpu);
//
//	dsp = 0x10020;
//	mpu = xlate_dsp2mpu(dsp);
//	PRINTF("dsp=%08X, mpu=%08X\n", dsp, mpu);
//
//	dsp = 0x10030;
//	mpu = xlate_dsp2mpu(dsp);
//	PRINTF("dsp=%08X, mpu=%08X\n", dsp, mpu);
//
//	dsp = 0x18020;
//	mpu = xlate_dsp2mpu(dsp);
//	PRINTF("dsp=%08X, mpu=%08X\n", dsp, mpu);
//
//	dsp = 0x18030;
//	mpu = xlate_dsp2mpu(dsp);
//	PRINTF("dsp=%08X, mpu=%08X\n", dsp, mpu);
//}

#endif /* CPUSTYLE_T113 */

#if CPUSTYLE_VM14

#define SZ_8K				0x00002000
#define NAND_BBT_USE_FLASH	0x00020000
/**
 * struct mtd_ecc_stats - error correction stats
 *
 * @corrected:	number of corrected bits
 * @failed:	number of uncorrectable errors
 * @badblocks:	number of bad blocks in this partition
 * @bbtblocks:	number of blocks reserved for bad block tables
 */
struct mtd_ecc_stats {
	uint32_t corrected;
	uint32_t failed;
	uint32_t badblocks;
	uint32_t bbtblocks;
};
/**
 * MTD operation modes
 *
 * @MTD_OPS_PLACE_OOB:	OOB data are placed at the given offset (default)
 * @MTD_OPS_AUTO_OOB:	OOB data are automatically placed at the free areas
 *			which are defined by the internal ecclayout
 * @MTD_OPS_RAW:	data are transferred as-is, with no error correction;
 *			this mode implies %MTD_OPS_PLACE_OOB
 *
 * These modes can be passed to ioctl(MEMWRITE) and are also used internally.
 * See notes on "MTD file modes" for discussion on %MTD_OPS_RAW vs.
 * %MTD_FILE_MODE_RAW.
 */
enum {
	MTD_OPS_PLACE_OOB = 0,
	MTD_OPS_AUTO_OOB = 1,
	MTD_OPS_RAW = 2,
};

#define MTD_ABSENT		0
#define MTD_RAM			1
#define MTD_ROM			2
#define MTD_NORFLASH		3
#define MTD_NANDFLASH		4	/* SLC NAND */
#define MTD_DATAFLASH		6
#define MTD_UBIVOLUME		7
#define MTD_MLCNANDFLASH	8	/* MLC NAND (including TLC) */

#define MTD_WRITEABLE		0x400	/* Device is writeable */
#define MTD_BIT_WRITEABLE	0x800	/* Single bits can be flipped */
#define MTD_NO_ERASE		0x1000	/* No erase necessary */
#define MTD_POWERUP_LOCK	0x2000	/* Always locked after reset */

/* Some common devices / combinations of capabilities */
#define MTD_CAP_ROM		0
#define MTD_CAP_RAM		(MTD_WRITEABLE | MTD_BIT_WRITEABLE | MTD_NO_ERASE)
#define MTD_CAP_NORFLASH	(MTD_WRITEABLE | MTD_BIT_WRITEABLE)
#define MTD_CAP_NANDFLASH	(MTD_WRITEABLE)

/* Obsolete ECC byte placement modes (used with obsolete MEMGETOOBSEL) */
#define MTD_NANDECC_OFF		0	// Switch off ECC (Not recommended)
#define MTD_NANDECC_PLACE	1	// Use the given placement in the structure (YAFFS1 legacy mode)
#define MTD_NANDECC_AUTOPLACE	2	// Use the default placement scheme
#define MTD_NANDECC_PLACEONLY	3	// Use the given placement in the structure (Do not store ecc result on read)
#define MTD_NANDECC_AUTOPL_USR 	4	// Use the given autoplacement scheme rather than using the default

/* OTP mode selection */
#define MTD_OTP_OFF		0
#define MTD_OTP_FACTORY		1
#define MTD_OTP_USER		2

static void udelay(unsigned us)
{
	local_delay_us(us * 1);
}

static uint32_t read32(uintptr_t addr)
{
	return * (volatile uint32_t *) addr;
}

static void write32(uintptr_t addr, uint32_t value)
{
	* (volatile uint32_t *) addr = value;
}

#define le16_to_cpu(v) ((uint16_t) (v))

#define writel(data, addr) do { write32((uintptr_t) (addr), (data)); } while (0)
#define readl(addr) (read32((uintptr_t) (addr)))


/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright © 1999-2010 David Woodhouse <dwmw2@infradead.org> et al.
 *
 */

#ifndef __MTD_MTD_H__
#define __MTD_MTD_H__

//#ifndef __UBOOT__
//#include <linux/types.h>
//#include <linux/uio.h>
//#include <linux/notifier.h>
//#include <linux/device.h>
//
//#include <mtd/mtd-abi.h>
//
//#include <asm/div64.h>
//#else
//#include <linux/compat.h>
//#include <mtd/mtd-abi.h>
//#include <linux/errno.h>
//#include <linux/list.h>
//#include <div64.h>
//#if IS_ENABLED(CONFIG_DM)
//#include <dm/device.h>
//#endif

#define MAX_MTD_DEVICES 32
//#endif

#define MTD_ERASE_PENDING	0x01
#define MTD_ERASING		0x02
#define MTD_ERASE_SUSPEND	0x04
#define MTD_ERASE_DONE		0x08
#define MTD_ERASE_FAILED	0x10

#define MTD_FAIL_ADDR_UNKNOWN -1LL

/*
 * If the erase fails, fail_addr might indicate exactly which block failed. If
 * fail_addr = MTD_FAIL_ADDR_UNKNOWN, the failure was not at the device level
 * or was not specific to any particular block.
 */
struct erase_info {
	struct mtd_info *mtd;
	uint64_t addr;
	uint64_t len;
	uint64_t fail_addr;
	u_long time;
	u_long retries;
	unsigned dev;
	unsigned cell;
	void (*callback) (struct erase_info *self);
	u_long priv;
	u_char state;
	struct erase_info *next;
	int scrub;
};

struct mtd_erase_region_info {
	uint64_t offset;		/* At which this region starts, from the beginning of the MTD */
	uint32_t erasesize;		/* For this region */
	uint32_t numblocks;		/* Number of blocks of erasesize in this region */
	unsigned long *lockmap;		/* If keeping bitmap of locks */
};

/**
 * struct mtd_oob_ops - oob operation operands
 * @mode:	operation mode
 *
 * @len:	number of data bytes to write/read
 *
 * @retlen:	number of data bytes written/read
 *
 * @ooblen:	number of oob bytes to write/read
 * @oobretlen:	number of oob bytes written/read
 * @ooboffs:	offset of oob data in the oob area (only relevant when
 *		mode = MTD_OPS_PLACE_OOB or MTD_OPS_RAW)
 * @datbuf:	data buffer - if NULL only oob data are read/written
 * @oobbuf:	oob data buffer
 */
struct mtd_oob_ops {
	unsigned int	mode;
	size_t		len;
	size_t		retlen;
	size_t		ooblen;
	size_t		oobretlen;
	uint32_t	ooboffs;
	uint8_t		*datbuf;
	uint8_t		*oobbuf;
};

#ifdef CONFIG_SYS_NAND_MAX_OOBFREE
#define MTD_MAX_OOBFREE_ENTRIES_LARGE	CONFIG_SYS_NAND_MAX_OOBFREE
#else
#define MTD_MAX_OOBFREE_ENTRIES_LARGE	32
#endif

#ifdef CONFIG_SYS_NAND_MAX_ECCPOS
#define MTD_MAX_ECCPOS_ENTRIES_LARGE	CONFIG_SYS_NAND_MAX_ECCPOS
#else
#define MTD_MAX_ECCPOS_ENTRIES_LARGE	680
#endif
/**
 * struct mtd_oob_region - oob region definition
 * @offset: region offset
 * @length: region length
 *
 * This structure describes a region of the OOB area, and is used
 * to retrieve ECC or free bytes sections.
 * Each section is defined by an offset within the OOB area and a
 * length.
 */
struct mtd_oob_region {
	uint32_t offset;
	uint32_t length;
};

/*
 * struct mtd_ooblayout_ops - NAND OOB layout operations
 * @ecc: function returning an ECC region in the OOB area.
 *	 Should return -ERANGE if %section exceeds the total number of
 *	 ECC sections.
 * @free: function returning a free region in the OOB area.
 *	  Should return -ERANGE if %section exceeds the total number of
 *	  free sections.
 */
struct mtd_ooblayout_ops {
	int (*ecc)(struct mtd_info *mtd, int section,
		   struct mtd_oob_region *oobecc);
	int (*free)(struct mtd_info *mtd, int section,
		    struct mtd_oob_region *oobfree);
};

typedef signed long loff_t;
typedef signed long resource_size_t;
typedef int flstate_t;
typedef int ofnode;
struct kvec;
struct nand_chip;

struct nand_oobfree {
	uint32_t offset;
	uint32_t length;
};


struct otp_info {
	uint32_t start;
	uint32_t length;
	uint32_t locked;
};

/*
 * Internal ECC layout control structure. For historical reasons, there is a
 * similar, smaller struct nand_ecclayout_user (in mtd-abi.h) that is retained
 * for export to user-space via the ECCGETLAYOUT ioctl.
 * nand_ecclayout should be expandable in the future simply by the above macros.
 */
struct nand_ecclayout {
	uint32_t eccbytes;
	uint32_t eccpos[MTD_MAX_ECCPOS_ENTRIES_LARGE];
	uint32_t oobavail;
	struct nand_oobfree oobfree[MTD_MAX_OOBFREE_ENTRIES_LARGE];
};

struct module;	/* only needed for owner field in mtd_info */

struct mtd_info {
	u_char type;
	uint32_t flags;
	uint64_t size;	 // Total size of the MTD

	/* "Major" erase size for the device. Naïve users may take this
	 * to be the only erase size available, or may use the more detailed
	 * information below if they desire
	 */
	uint32_t erasesize;
	/* Minimal writable flash unit size. In case of NOR flash it is 1 (even
	 * though individual bits can be cleared), in case of NAND flash it is
	 * one NAND page (or half, or one-fourths of it), in case of ECC-ed NOR
	 * it is of ECC block size, etc. It is illegal to have writesize = 0.
	 * Any driver registering a struct mtd_info must ensure a writesize of
	 * 1 or larger.
	 */
	uint32_t writesize;

	/*
	 * Size of the write buffer used by the MTD. MTD devices having a write
	 * buffer can write multiple writesize chunks at a time. E.g. while
	 * writing 4 * writesize bytes to a device with 2 * writesize bytes
	 * buffer the MTD driver can (but doesn't have to) do 2 writesize
	 * operations, but not 4. Currently, all NANDs have writebufsize
	 * equivalent to writesize (NAND page size). Some NOR flashes do have
	 * writebufsize greater than writesize.
	 */
	uint32_t writebufsize;

	uint32_t oobsize;   // Amount of OOB data per block (e.g. 16)
	uint32_t oobavail;  // Available OOB bytes per block

	/*
	 * If erasesize is a power of 2 then the shift is stored in
	 * erasesize_shift otherwise erasesize_shift is zero. Ditto writesize.
	 */
	unsigned int erasesize_shift;
	unsigned int writesize_shift;
	/* Masks based on erasesize_shift and writesize_shift */
	unsigned int erasesize_mask;
	unsigned int writesize_mask;

	/*
	 * read ops return -EUCLEAN if max number of bitflips corrected on any
	 * one region comprising an ecc step equals or exceeds this value.
	 * Settable by driver, else defaults to ecc_strength.  User can override
	 * in sysfs.  N.B. The meaning of the -EUCLEAN return code has changed;
	 * see Documentation/ABI/testing/sysfs-class-mtd for more detail.
	 */
	unsigned int bitflip_threshold;

	// Kernel-only stuff starts here.
#ifndef __UBOOT__
	const char *name;
#else
	char *name;
#endif
	int index;

	/* OOB layout description */
	const struct mtd_ooblayout_ops *ooblayout;

	/* ECC layout structure pointer - read only! */
	struct nand_ecclayout *ecclayout;

	/* the ecc step size. */
	unsigned int ecc_step_size;

	/* max number of correctible bit errors per ecc step */
	unsigned int ecc_strength;

	/* Data for variable erase regions. If numeraseregions is zero,
	 * it means that the whole device has erasesize as given above.
	 */
	int numeraseregions;
	struct mtd_erase_region_info *eraseregions;

	/*
	 * Do not call via these pointers, use corresponding mtd_*()
	 * wrappers instead.
	 */
	int (*_erase) (struct mtd_info *mtd, struct erase_info *instr);
#ifndef __UBOOT__
	int (*_point) (struct mtd_info *mtd, loff_t from, size_t len,
		       size_t *retlen, void **virt, resource_size_t *phys);
	int (*_unpoint) (struct mtd_info *mtd, loff_t from, size_t len);
#endif
	unsigned long (*_get_unmapped_area) (struct mtd_info *mtd,
					     unsigned long len,
					     unsigned long offset,
					     unsigned long flags);
	int (*_read) (struct mtd_info *mtd, loff_t from, size_t len,
		      size_t *retlen, u_char *buf);
	int (*_write) (struct mtd_info *mtd, loff_t to, size_t len,
		       size_t *retlen, const u_char *buf);
	int (*_panic_write) (struct mtd_info *mtd, loff_t to, size_t len,
			     size_t *retlen, const u_char *buf);
	int (*_read_oob) (struct mtd_info *mtd, loff_t from,
			  struct mtd_oob_ops *ops);
	int (*_write_oob) (struct mtd_info *mtd, loff_t to,
			   struct mtd_oob_ops *ops);
	int (*_get_fact_prot_info) (struct mtd_info *mtd, size_t len,
				    size_t *retlen, struct otp_info *buf);
	int (*_read_fact_prot_reg) (struct mtd_info *mtd, loff_t from,
				    size_t len, size_t *retlen, u_char *buf);
	int (*_get_user_prot_info) (struct mtd_info *mtd, size_t len,
				    size_t *retlen, struct otp_info *buf);
	int (*_read_user_prot_reg) (struct mtd_info *mtd, loff_t from,
				    size_t len, size_t *retlen, u_char *buf);
	int (*_write_user_prot_reg) (struct mtd_info *mtd, loff_t to,
				     size_t len, size_t *retlen, u_char *buf);
	int (*_lock_user_prot_reg) (struct mtd_info *mtd, loff_t from,
				    size_t len);
#ifndef __UBOOT__
	int (*_writev) (struct mtd_info *mtd, const struct kvec *vecs,
			unsigned long count, loff_t to, size_t *retlen);
#endif
	void (*_sync) (struct mtd_info *mtd);
	int (*_lock) (struct mtd_info *mtd, loff_t ofs, uint64_t len);
	int (*_unlock) (struct mtd_info *mtd, loff_t ofs, uint64_t len);
	int (*_is_locked) (struct mtd_info *mtd, loff_t ofs, uint64_t len);
	int (*_block_isreserved) (struct mtd_info *mtd, loff_t ofs);
	int (*_block_isbad) (struct mtd_info *mtd, loff_t ofs);
	int (*_block_markbad) (struct mtd_info *mtd, loff_t ofs);
#ifndef __UBOOT__
	int (*_suspend) (struct mtd_info *mtd);
	void (*_resume) (struct mtd_info *mtd);
	void (*_reboot) (struct mtd_info *mtd);
#endif
	/*
	 * If the driver is something smart, like UBI, it may need to maintain
	 * its own reference counting. The below functions are only for driver.
	 */
	int (*_get_device) (struct mtd_info *mtd);
	void (*_put_device) (struct mtd_info *mtd);

#ifndef __UBOOT__
	/* Backing device capabilities for this device
	 * - provides mmap capabilities
	 */
	struct backing_dev_info *backing_dev_info;

	//struct notifier_block reboot_notifier;  /* default mode before reboot */
#endif

	/* ECC status information */
	struct mtd_ecc_stats ecc_stats;
	/* Subpage shift (NAND) */
	int subpage_sft;

	void *priv;

	struct module *owner;
#ifndef __UBOOT__
	//struct device dev;
#else
	//struct udevice *dev;
#endif
	int usecount;

	/* MTD devices do not have any parent. MTD partitions do. */
	struct mtd_info *parent;

	/*
	 * Offset of the partition relatively to the parent offset.
	 * Is 0 for real MTD devices (ie. not partitions).
	 */
	uint64_t offset;

	/*
	 * List node used to add an MTD partition to the parent
	 * partition list.
	 */
	//struct list_head node;

	/*
	 * List of partitions attached to this MTD device (the parent
	 * MTD device can itself be a partition).
	 */
	//struct list_head partitions;
};

//#if IS_ENABLED(CONFIG_DM)
////static void mtd_set_of_node(struct mtd_info *mtd,
////				   const struct device_node *np)
////{
////	mtd->dev->node.np = np;
////}
////
////static const struct device_node *mtd_get_of_node(struct mtd_info *mtd)
////{
////	return mtd->dev->node.np;
////}
//#else
//struct device_node;
////
////static void mtd_set_of_node(struct mtd_info *mtd,
////				   const struct device_node *np)
////{
////}
////
////static const struct device_node *mtd_get_of_node(struct mtd_info *mtd)
////{
////	return NULL;
////}
//#endif

//static int mtd_is_partition(const struct mtd_info *mtd)
//{
//	return mtd->parent;
//}
//
//static int mtd_has_partitions(const struct mtd_info *mtd)
//{
//	return !list_empty(&mtd->partitions);
//}

int mtd_partitions_used(struct mtd_info *master);

int mtd_ooblayout_ecc(struct mtd_info *mtd, int section,
		      struct mtd_oob_region *oobecc);
int mtd_ooblayout_find_eccregion(struct mtd_info *mtd, int eccbyte,
				 int *section,
				 struct mtd_oob_region *oobregion);
int mtd_ooblayout_get_eccbytes(struct mtd_info *mtd, uint8_t *eccbuf,
			       const uint8_t *oobbuf, int start, int nbytes);
int mtd_ooblayout_set_eccbytes(struct mtd_info *mtd, const uint8_t *eccbuf,
			       uint8_t *oobbuf, int start, int nbytes);
int mtd_ooblayout_free(struct mtd_info *mtd, int section,
		       struct mtd_oob_region *oobfree);
int mtd_ooblayout_get_databytes(struct mtd_info *mtd, uint8_t *databuf,
				const uint8_t *oobbuf, int start, int nbytes);
int mtd_ooblayout_set_databytes(struct mtd_info *mtd, const uint8_t *databuf,
				uint8_t *oobbuf, int start, int nbytes);
int mtd_ooblayout_count_freebytes(struct mtd_info *mtd);
int mtd_ooblayout_count_eccbytes(struct mtd_info *mtd);

static void mtd_set_ooblayout(struct mtd_info *mtd,
				     const struct mtd_ooblayout_ops *ooblayout)
{
	mtd->ooblayout = ooblayout;
}

static uint32_t mtd_oobavail(struct mtd_info *mtd, struct mtd_oob_ops *ops)
{
	return ops->mode == MTD_OPS_AUTO_OOB ? mtd->oobavail : mtd->oobsize;
}

int mtd_erase(struct mtd_info *mtd, struct erase_info *instr);
#ifndef __UBOOT__
int mtd_point(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen,
	      void **virt, resource_size_t *phys);
int mtd_unpoint(struct mtd_info *mtd, loff_t from, size_t len);
#endif
unsigned long mtd_get_unmapped_area(struct mtd_info *mtd, unsigned long len,
				    unsigned long offset, unsigned long flags);
int mtd_read(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen,
	     u_char *buf);
int mtd_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen,
	      const u_char *buf);
int mtd_panic_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen,
		    const u_char *buf);

int mtd_read_oob(struct mtd_info *mtd, loff_t from, struct mtd_oob_ops *ops);
int mtd_write_oob(struct mtd_info *mtd, loff_t to, struct mtd_oob_ops *ops);

int mtd_get_fact_prot_info(struct mtd_info *mtd, size_t len, size_t *retlen,
			   struct otp_info *buf);
int mtd_read_fact_prot_reg(struct mtd_info *mtd, loff_t from, size_t len,
			   size_t *retlen, u_char *buf);
int mtd_get_user_prot_info(struct mtd_info *mtd, size_t len, size_t *retlen,
			   struct otp_info *buf);
int mtd_read_user_prot_reg(struct mtd_info *mtd, loff_t from, size_t len,
			   size_t *retlen, u_char *buf);
int mtd_write_user_prot_reg(struct mtd_info *mtd, loff_t to, size_t len,
			    size_t *retlen, u_char *buf);
int mtd_lock_user_prot_reg(struct mtd_info *mtd, loff_t from, size_t len);

#ifndef __UBOOT__
int mtd_writev(struct mtd_info *mtd, const struct kvec *vecs,
	       unsigned long count, loff_t to, size_t *retlen);
#endif

static void mtd_sync(struct mtd_info *mtd)
{
	if (mtd->_sync)
		mtd->_sync(mtd);
}

int mtd_lock(struct mtd_info *mtd, loff_t ofs, uint64_t len);
int mtd_unlock(struct mtd_info *mtd, loff_t ofs, uint64_t len);
int mtd_is_locked(struct mtd_info *mtd, loff_t ofs, uint64_t len);
int mtd_block_isreserved(struct mtd_info *mtd, loff_t ofs);
int mtd_block_isbad(struct mtd_info *mtd, loff_t ofs);
int mtd_block_markbad(struct mtd_info *mtd, loff_t ofs);

#ifndef __UBOOT__
static int mtd_suspend(struct mtd_info *mtd)
{
	return mtd->_suspend ? mtd->_suspend(mtd) : 0;
}

static void mtd_resume(struct mtd_info *mtd)
{
	if (mtd->_resume)
		mtd->_resume(mtd);
}
#endif

static uint32_t do_div(uint64_t *n, uint32_t base)
{
	uint32_t remainder = *n % base;
	*n = *n / base;
	return remainder;
}

static uint32_t mtd_div_by_eb(uint64_t sz, struct mtd_info *mtd)
{
	if (mtd->erasesize_shift)
		return sz >> mtd->erasesize_shift;
	do_div(& sz, mtd->erasesize);
	return sz;
}

static uint32_t mtd_mod_by_eb(uint64_t sz, struct mtd_info *mtd)
{
	if (mtd->erasesize_shift)
		return sz & mtd->erasesize_mask;
	return do_div(& sz, mtd->erasesize);
}

static uint32_t mtd_div_by_ws(uint64_t sz, struct mtd_info *mtd)
{
	if (mtd->writesize_shift)
		return sz >> mtd->writesize_shift;
	do_div(& sz, mtd->writesize);
	return sz;
}

static uint32_t mtd_mod_by_ws(uint64_t sz, struct mtd_info *mtd)
{
	if (mtd->writesize_shift)
		return sz & mtd->writesize_mask;
	return do_div(& sz, mtd->writesize);
}

static int mtd_has_oob(const struct mtd_info *mtd)
{
	return mtd->_read_oob && mtd->_write_oob;
}

static int mtd_type_is_nand(const struct mtd_info *mtd)
{
	return mtd->type == MTD_NANDFLASH || mtd->type == MTD_MLCNANDFLASH;
}

static int mtd_can_have_bb(const struct mtd_info *mtd)
{
	return !!mtd->_block_isbad;
}

	/* Kernel-side ioctl definitions */

struct mtd_partition;
struct mtd_part_parser_data;

extern int mtd_device_parse_register(struct mtd_info *mtd,
				     const char * const *part_probe_types,
				     struct mtd_part_parser_data *parser_data,
				     const struct mtd_partition *defparts,
				     int defnr_parts);
#define mtd_device_register(master, parts, nr_parts)	\
	mtd_device_parse_register(master, NULL, NULL, parts, nr_parts)
extern int mtd_device_unregister(struct mtd_info *master);
extern struct mtd_info *get_mtd_device(struct mtd_info *mtd, int num);
extern int __get_mtd_device(struct mtd_info *mtd);
extern void __put_mtd_device(struct mtd_info *mtd);
extern struct mtd_info *get_mtd_device_nm(const char *name);
extern void put_mtd_device(struct mtd_info *mtd);


#define EINVAL (1)
#define ENODEV (2)
#define ENXIO	(3)
#define ETIMEDOUT (4)
#define EUCLEAN (5)
#define EBADMSG (6)
#define ENOMEM (7)

#ifndef __UBOOT__
//struct mtd_notifier {
//	void (*add)(struct mtd_info *mtd);
//	void (*remove)(struct mtd_info *mtd);
//	struct list_head list;
//};
//
//
//extern void register_mtd_user (struct mtd_notifier *new);
//extern int unregister_mtd_user (struct mtd_notifier *old);
#endif
void *mtd_kmalloc_up_to(const struct mtd_info *mtd, size_t *size);

#ifdef CONFIG_MTD_PARTITIONS
void mtd_erase_callback(struct erase_info *instr);
#else
static void mtd_erase_callback(struct erase_info *instr)
{
	if (instr->callback)
		instr->callback(instr);
}
#endif

static int mtd_is_bitflip(int err) {
	return err == -EUCLEAN;
}

static int mtd_is_eccerr(int err) {
	return err == -EBADMSG;
}

static int mtd_is_bitflip_or_eccerr(int err) {
	return mtd_is_bitflip(err) || mtd_is_eccerr(err);
}

unsigned mtd_mmap_capabilities(struct mtd_info *mtd);

#ifdef __UBOOT__
/* drivers/mtd/mtdcore.h */
int add_mtd_device(struct mtd_info *mtd);
int del_mtd_device(struct mtd_info *mtd);

#ifdef CONFIG_MTD_PARTITIONS
int add_mtd_partitions(struct mtd_info *, const struct mtd_partition *, int);
int del_mtd_partitions(struct mtd_info *);
#else
static int add_mtd_partitions(struct mtd_info *mtd,
				     const struct mtd_partition *parts,
				     int nparts)
{
	return 0;
}

static int del_mtd_partitions(struct mtd_info *mtd)
{
	return 0;
}
#endif

struct mtd_info *__mtd_next_device(int i);
#define mtd_for_each_device(mtd)			\
	for ((mtd) = __mtd_next_device(0);		\
	     (mtd) != NULL;				\
	     (mtd) = __mtd_next_device(mtd->index + 1))

int mtd_arg_off(const char *arg, int *idx, loff_t *off, loff_t *size,
		loff_t *maxsize, int devtype, uint64_t chipsize);
int mtd_arg_off_size(int argc, char *const argv[], int *idx, loff_t *off,
		     loff_t *size, loff_t *maxsize, int devtype,
		     uint64_t chipsize);

/* drivers/mtd/mtdcore.c */
void mtd_get_len_incl_bad(struct mtd_info *mtd, uint64_t offset,
			  const uint64_t length, uint64_t *len_incl_bad,
			  int *truncated);
int mtd_dev_list_updated(void);

/* drivers/mtd/mtd_uboot.c */
int mtd_search_alternate_name(const char *mtdname, char *altname,
			      unsigned int max_len);

#endif
#endif /* __MTD_MTD_H__ */
/* SPDX-License-Identifier: GPL-2.0+ */
/*
 *  Copyright © 2000-2010 David Woodhouse <dwmw2@infradead.org>
 *                        Steven J. Hill <sjhill@realitydiluted.com>
 *		          Thomas Gleixner <tglx@linutronix.de>
 *
 * Info:
 *	Contains standard defines and IDs for NAND flash devices
 *
 * Changelog:
 *	See git changelog.
 */
#ifndef __LINUX_MTD_RAWNAND_H
#define __LINUX_MTD_RAWNAND_H

//#include <config.h>
//
//#include <dm/device.h>
//#include <linux/compat.h>
//#include <linux/mtd/mtd.h>
//#include <linux/mtd/flashchip.h>
//#include <linux/mtd/bbm.h>
//#include <asm/cache.h>

struct mtd_info;
struct nand_chip;
struct nand_flash_dev;
struct device_node;

/* Get the flash and manufacturer id and lookup if the type is supported. */
struct nand_flash_dev *nand_get_flash_type(struct mtd_info *mtd,
					   struct nand_chip *chip,
					   int *maf_id, int *dev_id,
					   struct nand_flash_dev *type);

/* Scan and identify a NAND device */
int nand_scan(struct mtd_info *mtd, int max_chips);
/*
 * Separate phases of nand_scan(), allowing board driver to intervene
 * and override command or ECC setup according to flash type.
 */
int nand_scan_ident(struct mtd_info *mtd, int max_chips,
			   struct nand_flash_dev *table);
int nand_scan_tail(struct mtd_info *mtd);

/* Free resources held by the NAND device */
void nand_release(struct mtd_info *mtd);

/* Internal helper for board drivers which need to override command function */
void nand_wait_ready(struct mtd_info *mtd);

/*
 * This constant declares the max. oobsize / page, which
 * is supported now. If you add a chip with bigger oobsize/page
 * adjust this accordingly.
 */
#define NAND_MAX_OOBSIZE       1664
#define NAND_MAX_PAGESIZE      16384
#define ARCH_DMA_MINALIGN 4

#define ALIGN(n, g) (n)
/*
 * Constants for hardware specific CLE/ALE/NCE function
 *
 * These are bits which can be or'ed to set/clear multiple
 * bits in one go.
 */
/* Select the chip by setting nCE to low */
#define NAND_NCE		0x01
/* Select the command latch by setting CLE to high */
#define NAND_CLE		0x02
/* Select the address latch by setting ALE to high */
#define NAND_ALE		0x04

#define NAND_CTRL_CLE		(NAND_NCE | NAND_CLE)
#define NAND_CTRL_ALE		(NAND_NCE | NAND_ALE)
#define NAND_CTRL_CHANGE	0x80

/*
 * Standard NAND flash commands
 */
#define NAND_CMD_READ0		0
#define NAND_CMD_READ1		1
#define NAND_CMD_RNDOUT		5
#define NAND_CMD_PAGEPROG	0x10
#define NAND_CMD_READOOB	0x50
#define NAND_CMD_ERASE1		0x60
#define NAND_CMD_STATUS		0x70
#define NAND_CMD_SEQIN		0x80
#define NAND_CMD_RNDIN		0x85
#define NAND_CMD_READID		0x90
#define NAND_CMD_ERASE2		0xd0
#define NAND_CMD_PARAM		0xec
#define NAND_CMD_GET_FEATURES	0xee
#define NAND_CMD_SET_FEATURES	0xef
#define NAND_CMD_RESET		0xff

#define NAND_CMD_LOCK		0x2a
#define NAND_CMD_UNLOCK1	0x23
#define NAND_CMD_UNLOCK2	0x24

/* Extended commands for large page devices */
#define NAND_CMD_READSTART	0x30
#define NAND_CMD_RNDOUTSTART	0xE0
#define NAND_CMD_CACHEDPROG	0x15

/* Extended commands for AG-AND device */
/*
 * Note: the command for NAND_CMD_DEPLETE1 is really 0x00 but
 *       there is no way to distinguish that from NAND_CMD_READ0
 *       until the remaining sequence of commands has been completed
 *       so add a high order bit and mask it off in the command.
 */
#define NAND_CMD_DEPLETE1	0x100
#define NAND_CMD_DEPLETE2	0x38
#define NAND_CMD_STATUS_MULTI	0x71
#define NAND_CMD_STATUS_ERROR	0x72
/* multi-bank error status (banks 0-3) */
#define NAND_CMD_STATUS_ERROR0	0x73
#define NAND_CMD_STATUS_ERROR1	0x74
#define NAND_CMD_STATUS_ERROR2	0x75
#define NAND_CMD_STATUS_ERROR3	0x76
#define NAND_CMD_STATUS_RESET	0x7f
#define NAND_CMD_STATUS_CLEAR	0xff

#define NAND_CMD_NONE		-1

/* Status bits */
#define NAND_STATUS_FAIL	0x01
#define NAND_STATUS_FAIL_N1	0x02
#define NAND_STATUS_TRUE_READY	0x20
#define NAND_STATUS_READY	0x40
#define NAND_STATUS_WP		0x80

#define NAND_DATA_IFACE_CHECK_ONLY	-1

/*
 * Constants for ECC_MODES
 */
typedef enum {
	NAND_ECC_NONE,
	NAND_ECC_SOFT,
	NAND_ECC_HW,
	NAND_ECC_HW_SYNDROME,
	NAND_ECC_HW_OOB_FIRST,
	NAND_ECC_SOFT_BCH,
} nand_ecc_modes_t;

enum nand_ecc_algo {
	NAND_ECC_UNKNOWN,
	NAND_ECC_HAMMING,
	NAND_ECC_BCH,
};

/*
 * Constants for Hardware ECC
 */
/* Reset Hardware ECC for read */
#define NAND_ECC_READ		0
/* Reset Hardware ECC for write */
#define NAND_ECC_WRITE		1
/* Enable Hardware ECC before syndrome is read back from flash */
#define NAND_ECC_READSYN	2

/*
 * Enable generic NAND 'page erased' check. This check is only done when
 * ecc.correct() returns -EBADMSG.
 * Set this flag if your implementation does not fix bitflips in erased
 * pages and you want to rely on the default implementation.
 */
#define NAND_ECC_GENERIC_ERASED_CHECK	NAND_BIT(0)
#define NAND_ECC_MAXIMIZE		NAND_BIT(1)
/*
 * If your controller already sends the required NAND commands when
 * reading or writing a page, then the framework is not supposed to
 * send READ0 and SEQIN/PAGEPROG respectively.
 */
#define NAND_ECC_CUSTOM_PAGE_ACCESS	NAND_BIT(2)

/* Bit mask for flags passed to do_nand_read_ecc */
#define NAND_GET_DEVICE		0x80


/*
 * Option constants for bizarre disfunctionality and real
 * features.
 */
/* Buswidth is 16 bit */
#define NAND_BUSWIDTH_16	0x00000002
/* Device supports partial programming without padding */
#define NAND_NO_PADDING		0x00000004
/* Chip has cache program function */
#define NAND_CACHEPRG		0x00000008
/* Chip has copy back function */
#define NAND_COPYBACK		0x00000010
/*
 * Chip requires ready check on read (for auto-incremented sequential read).
 * True only for small page devices; large page devices do not support
 * autoincrement.
 */
#define NAND_NEED_READRDY	0x00000100

/* Chip does not allow subpage writes */
#define NAND_NO_SUBPAGE_WRITE	0x00000200

/* Device is one of 'new' xD cards that expose fake nand command set */
#define NAND_BROKEN_XD		0x00000400

/* Device behaves just like nand, but is readonly */
#define NAND_ROM		0x00000800

/* Device supports subpage reads */
#define NAND_SUBPAGE_READ	0x00001000

/*
 * Some MLC NANDs need data scrambling to limit bitflips caused by repeated
 * patterns.
 */
#define NAND_NEED_SCRAMBLING	0x00002000

/* Device needs 3rd row address cycle */
#define NAND_ROW_ADDR_3		0x00004000

/* Options valid for Samsung large page devices */
#define NAND_SAMSUNG_LP_OPTIONS NAND_CACHEPRG

/* Macros to identify the above */
#define NAND_HAS_CACHEPROG(chip) ((chip->options & NAND_CACHEPRG))
#define NAND_HAS_SUBPAGE_READ(chip) ((chip->options & NAND_SUBPAGE_READ))
#define NAND_HAS_SUBPAGE_WRITE(chip) !((chip)->options & NAND_NO_SUBPAGE_WRITE)

/* Non chip related options */
/* This option skips the bbt scan during initialization. */
#define NAND_SKIP_BBTSCAN	0x00010000
/*
 * This option is defined if the board driver allocates its own buffers
 * (e.g. because it needs them DMA-coherent).
 */
#define NAND_OWN_BUFFERS	0x00020000
/* Chip may not exist, so silence any errors in scan */
#define NAND_SCAN_SILENT_NODEV	0x00040000
/*
 * Autodetect nand buswidth with readid/onfi.
 * This suppose the driver will configure the hardware in 8 bits mode
 * when calling nand_scan_ident, and update its configuration
 * before calling nand_scan_tail.
 */
#define NAND_BUSWIDTH_AUTO      0x00080000
/*
 * This option could be defined by controller drivers to protect against
 * kmap'ed, vmalloc'ed highmem buffers being passed from upper layers
 */
#define NAND_USE_BOUNCE_BUFFER	0x00100000

/* Options set by nand scan */
/* bbt has already been read */
#define NAND_BBT_SCANNED	0x40000000
/* Nand scan has allocated controller struct */
#define NAND_CONTROLLER_ALLOC	0x80000000

/* Cell info constants */
#define NAND_CI_CHIPNR_MSK	0x03
#define NAND_CI_CELLTYPE_MSK	0x0C
#define NAND_CI_CELLTYPE_SHIFT	2

/* ONFI features */
#define ONFI_FEATURE_16_BIT_BUS		(1 << 0)
#define ONFI_FEATURE_EXT_PARAM_PAGE	(1 << 7)

/* ONFI timing mode, used in both asynchronous and synchronous mode */
#define ONFI_TIMING_MODE_0		(1 << 0)
#define ONFI_TIMING_MODE_1		(1 << 1)
#define ONFI_TIMING_MODE_2		(1 << 2)
#define ONFI_TIMING_MODE_3		(1 << 3)
#define ONFI_TIMING_MODE_4		(1 << 4)
#define ONFI_TIMING_MODE_5		(1 << 5)
#define ONFI_TIMING_MODE_UNKNOWN	(1 << 6)

/* ONFI feature address */
#define ONFI_FEATURE_ADDR_TIMING_MODE	0x1

/* Vendor-specific feature address (Micron) */
#define ONFI_FEATURE_ADDR_READ_RETRY	0x89

/* ONFI subfeature parameters length */
#define ONFI_SUBFEATURE_PARAM_LEN	4

/* ONFI optional commands SET/GET FEATURES supported? */
#define ONFI_OPT_CMD_SET_GET_FEATURES	(1 << 2)

// Taken from
// https://github.com/elvees/u-boot/blob/1ad4d225d4b37bf562cc410ad9db87ce44628c44/include/linux/mtd/rawnand.h
// https://github.com/elvees/u-boot/blob/1ad4d225d4b37bf562cc410ad9db87ce44628c44/drivers/mtd/nand/raw/mcom02-nand.c

typedef uint16_t __le16;
typedef uint32_t __le32;
#define NAND_BIT(pos) (UINT32_C(1) << (pos))

/*
 * Create a contiguous bitmask starting at bit position @l and ending at
 * position @h. For example
 * GENMASK_64(39, 21) gives us the 64bit vector 0x000000ffffe00000.
 */
#if defined(__LINKER__) || defined(__ASSEMBLER__)
#define GENMASK_32(h, l) \
	(((0xFFFFFFFF) << (l)) & (0xFFFFFFFF >> (32 - 1 - (h))))

#define GENMASK_64(h, l) \
	((~0 << (l)) & (~0 >> (64 - 1 - (h))))
#else
#define GENMASK_32(h, l) \
	(((~UINT32_C(0)) << (l)) & (~UINT32_C(0) >> (32 - 1 - (h))))

#define GENMASK_64(h, l) \
	(((~UINT64_C(0)) << (l)) & (~UINT64_C(0) >> (64 - 1 - (h))))
#endif

#ifdef __aarch64__
#define NAND_GENMASK				GENMASK_64
#else
#define NAND_GENMASK				GENMASK_32
#endif

struct nand_onfi_params {
	/* rev info and features block */
	/* 'O' 'N' 'F' 'I'  */
	uint8_t sig[4];
	__le16 revision;
	__le16 features;
	__le16 opt_cmd;
	uint8_t reserved0[2];
	__le16 ext_param_page_length; /* since ONFI 2.1 */
	uint8_t num_of_param_pages;        /* since ONFI 2.1 */
	uint8_t reserved1[17];

	/* manufacturer information block */
	char manufacturer[12];
	char model[20];
	uint8_t jedec_id;
	__le16 date_code;
	uint8_t reserved2[13];

	/* memory organization block */
	__le32 byte_per_page;
	__le16 spare_bytes_per_page;
	__le32 data_bytes_per_ppage;
	__le16 spare_bytes_per_ppage;
	__le32 pages_per_block;
	__le32 blocks_per_lun;
	uint8_t lun_count;
	uint8_t addr_cycles;
	uint8_t bits_per_cell;
	__le16 bb_per_lun;
	__le16 block_endurance;
	uint8_t guaranteed_good_blocks;
	__le16 guaranteed_block_endurance;
	uint8_t programs_per_page;
	uint8_t ppage_attr;
	uint8_t ecc_bits;
	uint8_t interleaved_bits;
	uint8_t interleaved_ops;
	uint8_t reserved3[13];

	/* electrical parameter block */
	uint8_t io_pin_capacitance_max;
	__le16 async_timing_mode;
	__le16 program_cache_timing_mode;
	__le16 t_prog;
	__le16 t_bers;
	__le16 t_r;
	__le16 t_ccs;
	__le16 src_sync_timing_mode;
	uint8_t src_ssync_features;
	__le16 clk_pin_capacitance_typ;
	__le16 io_pin_capacitance_typ;
	__le16 input_pin_capacitance_typ;
	uint8_t input_pin_capacitance_max;
	uint8_t driver_strength_support;
	__le16 t_int_r;
	__le16 t_adl;
	uint8_t reserved4[8];

	/* vendor */
	__le16 vendor_revision;
	uint8_t vendor[88];

	__le16 crc;
} __packed;

#define ONFI_CRC_BASE	0x4F4E

/* Extended ECC information Block Definition (since ONFI 2.1) */
struct onfi_ext_ecc_info {
	uint8_t ecc_bits;
	uint8_t codeword_size;
	__le16 bb_per_lun;
	__le16 block_endurance;
	uint8_t reserved[2];
} __packed;

#define ONFI_SECTION_TYPE_0	0	/* Unused section. */
#define ONFI_SECTION_TYPE_1	1	/* for additional sections. */
#define ONFI_SECTION_TYPE_2	2	/* for ECC information. */
struct onfi_ext_section {
	uint8_t type;
	uint8_t length;
} __packed;

#define ONFI_EXT_SECTION_MAX 8

/* Extended Parameter Page Definition (since ONFI 2.1) */
struct onfi_ext_param_page {
	__le16 crc;
	uint8_t sig[4];             /* 'E' 'P' 'P' 'S' */
	uint8_t reserved0[10];
	struct onfi_ext_section sections[ONFI_EXT_SECTION_MAX];

	/*
	 * The actual size of the Extended Parameter Page is in
	 * @ext_param_page_length of nand_onfi_params{}.
	 * The following are the variable length sections.
	 * So we do not add any fields below. Please see the ONFI spec.
	 */
} __packed;

struct nand_onfi_vendor_micron {
	uint8_t two_plane_read;
	uint8_t read_cache;
	uint8_t read_unique_id;
	uint8_t dq_imped;
	uint8_t dq_imped_num_settings;
	uint8_t dq_imped_feat_addr;
	uint8_t rb_pulldown_strength;
	uint8_t rb_pulldown_strength_feat_addr;
	uint8_t rb_pulldown_strength_num_settings;
	uint8_t otp_mode;
	uint8_t otp_page_start;
	uint8_t otp_data_prot_addr;
	uint8_t otp_num_pages;
	uint8_t otp_feat_addr;
	uint8_t read_retry_options;
	uint8_t reserved[72];
	uint8_t param_revision;
} __packed;

struct jedec_ecc_info {
	uint8_t ecc_bits;
	uint8_t codeword_size;
	__le16 bb_per_lun;
	__le16 block_endurance;
	uint8_t reserved[2];
} __packed;

/* JEDEC features */
#define JEDEC_FEATURE_16_BIT_BUS	(1 << 0)

struct nand_jedec_params {
	/* rev info and features block */
	/* 'J' 'E' 'S' 'D'  */
	uint8_t sig[4];
	__le16 revision;
	__le16 features;
	uint8_t opt_cmd[3];
	__le16 sec_cmd;
	uint8_t num_of_param_pages;
	uint8_t reserved0[18];

	/* manufacturer information block */
	char manufacturer[12];
	char model[20];
	uint8_t jedec_id[6];
	uint8_t reserved1[10];

	/* memory organization block */
	__le32 byte_per_page;
	__le16 spare_bytes_per_page;
	uint8_t reserved2[6];
	__le32 pages_per_block;
	__le32 blocks_per_lun;
	uint8_t lun_count;
	uint8_t addr_cycles;
	uint8_t bits_per_cell;
	uint8_t programs_per_page;
	uint8_t multi_plane_addr;
	uint8_t multi_plane_op_attr;
	uint8_t reserved3[38];

	/* electrical parameter block */
	__le16 async_sdr_speed_grade;
	__le16 toggle_ddr_speed_grade;
	__le16 sync_ddr_speed_grade;
	uint8_t async_sdr_features;
	uint8_t toggle_ddr_features;
	uint8_t sync_ddr_features;
	__le16 t_prog;
	__le16 t_bers;
	__le16 t_r;
	__le16 t_r_multi_plane;
	__le16 t_ccs;
	__le16 io_pin_capacitance_typ;
	__le16 input_pin_capacitance_typ;
	__le16 clk_pin_capacitance_typ;
	uint8_t driver_strength_support;
	__le16 t_adl;
	uint8_t reserved4[36];

	/* ECC and endurance block */
	uint8_t guaranteed_good_blocks;
	__le16 guaranteed_block_endurance;
	struct jedec_ecc_info ecc_info[4];
	uint8_t reserved5[29];

	/* reserved */
	uint8_t reserved6[148];

	/* vendor */
	__le16 vendor_rev_num;
	uint8_t reserved7[88];

	/* CRC for Parameter Page */
	__le16 crc;
} __packed;

/**
 * struct nand_hw_control - Control structure for hardware controller (e.g ECC generator) shared among independent devices
 * @lock:               protection lock
 * @active:		the mtd device which holds the controller currently
 * @wq:			wait queue to sleep on if a NAND operation is in
 *			progress used instead of the per chip wait queue
 *			when a hw controller is available.
 */
//struct nand_hw_control {
//	spinlock_t lock;
//	struct nand_chip *active;
//};
//
//static void nand_hw_control_init(struct nand_hw_control *nfc)
//{
//	nfc->active = NULL;
//	spin_lock_init(&nfc->lock);
//	init_waitqueue_head(&nfc->wq);
//}

/**
 * struct nand_ecc_step_info - ECC step information of ECC engine
 * @stepsize: data bytes per ECC step
 * @strengths: array of supported strengths
 * @nstrengths: number of supported strengths
 */
struct nand_ecc_step_info {
	int stepsize;
	const int *strengths;
	int nstrengths;
};

/**
 * struct nand_ecc_caps - capability of ECC engine
 * @stepinfos: array of ECC step information
 * @nstepinfos: number of ECC step information
 * @calc_ecc_bytes: driver's hook to calculate ECC bytes per step
 */
struct nand_ecc_caps {
	const struct nand_ecc_step_info *stepinfos;
	int nstepinfos;
	int (*calc_ecc_bytes)(int step_size, int strength);
};

/* a shorthand to generate struct nand_ecc_caps with only one ECC stepsize */
#define NAND_ECC_CAPS_SINGLE(__name, __calc, __step, ...)	\
static const int __name##_strengths[] = { __VA_ARGS__ };	\
static const struct nand_ecc_step_info __name##_stepinfo = {	\
	.stepsize = __step,					\
	.strengths = __name##_strengths,			\
	.nstrengths = ARRAY_SIZE(__name##_strengths),		\
};								\
static const struct nand_ecc_caps __name = {			\
	.stepinfos = &__name##_stepinfo,			\
	.nstepinfos = 1,					\
	.calc_ecc_bytes = __calc,				\
}

/**
 * struct nand_ecc_ctrl - Control structure for ECC
 * @mode:	ECC mode
 * @algo:	ECC algorithm
 * @steps:	number of ECC steps per page
 * @size:	data bytes per ECC step
 * @bytes:	ECC bytes per step
 * @strength:	max number of correctible bits per ECC step
 * @total:	total number of ECC bytes per page
 * @prepad:	padding information for syndrome based ECC generators
 * @postpad:	padding information for syndrome based ECC generators
 * @options:	ECC specific options (see NAND_ECC_XXX flags defined above)
 * @layout:	ECC layout control struct pointer
 * @priv:	pointer to private ECC control data
 * @hwctl:	function to control hardware ECC generator. Must only
 *		be provided if an hardware ECC is available
 * @calculate:	function for ECC calculation or readback from ECC hardware
 * @correct:	function for ECC correction, matching to ECC generator (sw/hw).
 *		Should return a positive number representing the number of
 *		corrected bitflips, -EBADMSG if the number of bitflips exceed
 *		ECC strength, or any other error code if the error is not
 *		directly related to correction.
 *		If -EBADMSG is returned the input buffers should be left
 *		untouched.
 * @read_page_raw:	function to read a raw page without ECC. This function
 *			should hide the specific layout used by the ECC
 *			controller and always return contiguous in-band and
 *			out-of-band data even if they're not stored
 *			contiguously on the NAND chip (e.g.
 *			NAND_ECC_HW_SYNDROME interleaves in-band and
 *			out-of-band data).
 * @write_page_raw:	function to write a raw page without ECC. This function
 *			should hide the specific layout used by the ECC
 *			controller and consider the passed data as contiguous
 *			in-band and out-of-band data. ECC controller is
 *			responsible for doing the appropriate transformations
 *			to adapt to its specific layout (e.g.
 *			NAND_ECC_HW_SYNDROME interleaves in-band and
 *			out-of-band data).
 * @read_page:	function to read a page according to the ECC generator
 *		requirements; returns maximum number of bitflips corrected in
 *		any single ECC step, 0 if bitflips uncorrectable, -EIO hw error
 * @read_subpage:	function to read parts of the page covered by ECC;
 *			returns same as read_page()
 * @write_subpage:	function to write parts of the page covered by ECC.
 * @write_page:	function to write a page according to the ECC generator
 *		requirements.
 * @write_oob_raw:	function to write chip OOB data without ECC
 * @read_oob_raw:	function to read chip OOB data without ECC
 * @read_oob:	function to read chip OOB data
 * @write_oob:	function to write chip OOB data
 */
struct nand_ecc_ctrl {
	nand_ecc_modes_t mode;
	enum nand_ecc_algo algo;
	int steps;
	int size;
	int bytes;
	int total;
	int strength;
	int prepad;
	int postpad;
	unsigned int options;
	struct nand_ecclayout	*layout;
	void *priv;
	void (*hwctl)(struct mtd_info *mtd, int mode);
	int (*calculate)(struct mtd_info *mtd, const uint8_t *dat,
			uint8_t *ecc_code);
	int (*correct)(struct mtd_info *mtd, uint8_t *dat, uint8_t *read_ecc,
			uint8_t *calc_ecc);
	int (*read_page_raw)(struct mtd_info *mtd, struct nand_chip *chip,
			uint8_t *buf, int oob_required, int page);
	int (*write_page_raw)(struct mtd_info *mtd, struct nand_chip *chip,
			const uint8_t *buf, int oob_required, int page);
	int (*read_page)(struct mtd_info *mtd, struct nand_chip *chip,
			uint8_t *buf, int oob_required, int page);
	int (*read_subpage)(struct mtd_info *mtd, struct nand_chip *chip,
			uint32_t offs, uint32_t len, uint8_t *buf, int page);
	int (*write_subpage)(struct mtd_info *mtd, struct nand_chip *chip,
			uint32_t offset, uint32_t data_len,
			const uint8_t *data_buf, int oob_required, int page);
	int (*write_page)(struct mtd_info *mtd, struct nand_chip *chip,
			const uint8_t *buf, int oob_required, int page);
	int (*write_oob_raw)(struct mtd_info *mtd, struct nand_chip *chip,
			int page);
	int (*read_oob_raw)(struct mtd_info *mtd, struct nand_chip *chip,
			int page);
	int (*read_oob)(struct mtd_info *mtd, struct nand_chip *chip, int page);
	int (*write_oob)(struct mtd_info *mtd, struct nand_chip *chip,
			int page);
};

static int nand_standard_page_accessors(struct nand_ecc_ctrl *ecc)
{
	return !(ecc->options & NAND_ECC_CUSTOM_PAGE_ACCESS);
}

/**
 * struct nand_buffers - buffer structure for read/write
 * @ecccalc:	buffer pointer for calculated ECC, size is oobsize.
 * @ecccode:	buffer pointer for ECC read from flash, size is oobsize.
 * @databuf:	buffer pointer for data, size is (page size + oobsize).
 *
 * Do not change the order of buffers. databuf and oobrbuf must be in
 * consecutive order.
 */
struct nand_buffers {
	uint8_t	ecccalc[ALIGN(NAND_MAX_OOBSIZE, ARCH_DMA_MINALIGN)];
	uint8_t	ecccode[ALIGN(NAND_MAX_OOBSIZE, ARCH_DMA_MINALIGN)];
	uint8_t databuf[ALIGN(NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE,
			      ARCH_DMA_MINALIGN)];
};

/**
 * struct nand_sdr_timings - SDR NAND chip timings
 *
 * This struct defines the timing requirements of a SDR NAND chip.
 * These information can be found in every NAND datasheets and the timings
 * meaning are described in the ONFI specifications:
 * www.onfi.org/~/media/ONFI/specs/onfi_3_1_spec.pdf (chapter 4.15 Timing
 * Parameters)
 *
 * All these timings are expressed in picoseconds.
 *
 * @tBERS_max: Block erase time
 * @tCCS_min: Change column setup time
 * @tPROG_max: Page program time
 * @tR_max: Page read time
 * @tALH_min: ALE hold time
 * @tADL_min: ALE to data loading time
 * @tALS_min: ALE setup time
 * @tAR_min: ALE to RE# delay
 * @tCEA_max: CE# access time
 * @tCEH_min: CE# high hold time
 * @tCH_min:  CE# hold time
 * @tCHZ_max: CE# high to output hi-Z
 * @tCLH_min: CLE hold time
 * @tCLR_min: CLE to RE# delay
 * @tCLS_min: CLE setup time
 * @tCOH_min: CE# high to output hold
 * @tCS_min: CE# setup time
 * @tDH_min: Data hold time
 * @tDS_min: Data setup time
 * @tFEAT_max: Busy time for Set Features and Get Features
 * @tIR_min: Output hi-Z to RE# low
 * @tITC_max: Interface and Timing Mode Change time
 * @tRC_min: RE# cycle time
 * @tREA_max: RE# access time
 * @tREH_min: RE# high hold time
 * @tRHOH_min: RE# high to output hold
 * @tRHW_min: RE# high to WE# low
 * @tRHZ_max: RE# high to output hi-Z
 * @tRLOH_min: RE# low to output hold
 * @tRP_min: RE# pulse width
 * @tRR_min: Ready to RE# low (data only)
 * @tRST_max: Device reset time, measured from the falling edge of R/B# to the
 *	      rising edge of R/B#.
 * @tWB_max: WE# high to SR[6] low
 * @tWC_min: WE# cycle time
 * @tWH_min: WE# high hold time
 * @tWHR_min: WE# high to RE# low
 * @tWP_min: WE# pulse width
 * @tWW_min: WP# transition to WE# low
 */
struct nand_sdr_timings {
	uint64_t tBERS_max;
	uint32_t tCCS_min;
	uint64_t tPROG_max;
	uint64_t tR_max;
	uint32_t tALH_min;
	uint32_t tADL_min;
	uint32_t tALS_min;
	uint32_t tAR_min;
	uint32_t tCEA_max;
	uint32_t tCEH_min;
	uint32_t tCH_min;
	uint32_t tCHZ_max;
	uint32_t tCLH_min;
	uint32_t tCLR_min;
	uint32_t tCLS_min;
	uint32_t tCOH_min;
	uint32_t tCS_min;
	uint32_t tDH_min;
	uint32_t tDS_min;
	uint32_t tFEAT_max;
	uint32_t tIR_min;
	uint32_t tITC_max;
	uint32_t tRC_min;
	uint32_t tREA_max;
	uint32_t tREH_min;
	uint32_t tRHOH_min;
	uint32_t tRHW_min;
	uint32_t tRHZ_max;
	uint32_t tRLOH_min;
	uint32_t tRP_min;
	uint32_t tRR_min;
	uint64_t tRST_max;
	uint32_t tWB_max;
	uint32_t tWC_min;
	uint32_t tWH_min;
	uint32_t tWHR_min;
	uint32_t tWP_min;
	uint32_t tWW_min;
};

//#define ERR_PTR(p) ((void *) (p))

#define MAX_ERRNO 1000

#define IS_ERR_VALUE(x) ((x) >= (unsigned long)-MAX_ERRNO)

static void *ERR_PTR(long error)
{
	return (void *) error;
}

static long PTR_ERR(const void *ptr)
{
	return (long) ptr;
}

static long IS_ERR(const void *ptr)
{
	return IS_ERR_VALUE((unsigned long)ptr);
}

static int IS_ERR_OR_NULL(const void *ptr)
{
	return !ptr || IS_ERR_VALUE((unsigned long)ptr);
}

/**
 * ERR_CAST - Explicitly cast an error-valued pointer to another pointer type
 * @ptr: The pointer to cast.
 *
 * Explicitly cast an error-valued pointer to another pointer type in such a
 * way as to make it clear that's what's going on.
 */
static void * ERR_CAST( const void *ptr)
{
	/* cast away the const */
	return (void *) ptr;
}

/**
 * enum nand_data_interface_type - NAND interface timing type
 * @NAND_SDR_IFACE:	Single Data Rate interface
 */
enum nand_data_interface_type {
	NAND_SDR_IFACE,
};

/**
 * struct nand_data_interface - NAND interface timing
 * @type:	type of the timing
 * @timings:	The timing, type according to @type
 */
struct nand_data_interface {
	enum nand_data_interface_type type;
	union {
		struct nand_sdr_timings sdr;
	} timings;
};

/**
 * nand_get_sdr_timings - get SDR timing from data interface
 * @conf:	The data interface
 */
static const struct nand_sdr_timings *
nand_get_sdr_timings(const struct nand_data_interface *conf)
{
	if (conf->type != NAND_SDR_IFACE)
		return ERR_PTR(-EINVAL);

	return &conf->timings.sdr;
}

/**
 * struct nand_chip - NAND Private Flash Chip Data
 * @mtd:		MTD device registered to the MTD framework
 * @IO_ADDR_R:		[BOARDSPECIFIC] address to read the 8 I/O lines of the
 *			flash device
 * @IO_ADDR_W:		[BOARDSPECIFIC] address to write the 8 I/O lines of the
 *			flash device.
 * @flash_node:		[BOARDSPECIFIC] device node describing this instance
 * @read_byte:		[REPLACEABLE] read one byte from the chip
 * @read_word:		[REPLACEABLE] read one word from the chip
 * @write_byte:		[REPLACEABLE] write a single byte to the chip on the
 *			low 8 I/O lines
 * @write_buf:		[REPLACEABLE] write data from the buffer to the chip
 * @read_buf:		[REPLACEABLE] read data from the chip into the buffer
 * @select_chip:	[REPLACEABLE] select chip nr
 * @block_bad:		[REPLACEABLE] check if a block is bad, using OOB markers
 * @block_markbad:	[REPLACEABLE] mark a block bad
 * @cmd_ctrl:		[BOARDSPECIFIC] hardwarespecific function for controlling
 *			ALE/CLE/nCE. Also used to write command and address
 * @dev_ready:		[BOARDSPECIFIC] hardwarespecific function for accessing
 *			device ready/busy line. If set to NULL no access to
 *			ready/busy is available and the ready/busy information
 *			is read from the chip status register.
 * @cmdfunc:		[REPLACEABLE] hardwarespecific function for writing
 *			commands to the chip.
 * @waitfunc:		[REPLACEABLE] hardwarespecific function for wait on
 *			ready.
 * @setup_read_retry:	[FLASHSPECIFIC] flash (vendor) specific function for
 *			setting the read-retry mode. Mostly needed for MLC NAND.
 * @ecc:		[BOARDSPECIFIC] ECC control structure
 * @buffers:		buffer structure for read/write
 * @buf_align:		minimum buffer alignment required by a platform
 * @hwcontrol:		platform-specific hardware control structure
 * @erase:		[REPLACEABLE] erase function
 * @scan_bbt:		[REPLACEABLE] function to scan bad block table
 * @chip_delay:		[BOARDSPECIFIC] chip dependent delay for transferring
 *			data from array to read regs (tR).
 * @state:		[INTERN] the current state of the NAND device
 * @oob_poi:		"poison value buffer," used for laying out OOB data
 *			before writing
 * @page_shift:		[INTERN] number of address bits in a page (column
 *			address bits).
 * @phys_erase_shift:	[INTERN] number of address bits in a physical eraseblock
 * @bbt_erase_shift:	[INTERN] number of address bits in a bbt entry
 * @chip_shift:		[INTERN] number of address bits in one chip
 * @options:		[BOARDSPECIFIC] various chip options. They can partly
 *			be set to inform nand_scan about special functionality.
 *			See the defines for further explanation.
 * @bbt_options:	[INTERN] bad block specific options. All options used
 *			here must come from bbm.h. By default, these options
 *			will be copied to the appropriate nand_bbt_descr's.
 * @badblockpos:	[INTERN] position of the bad block marker in the oob
 *			area.
 * @badblockbits:	[INTERN] minimum number of set bits in a good block's
 *			bad block marker position; i.e., BBM == 11110111b is
 *			not bad when badblockbits == 7
 * @bits_per_cell:	[INTERN] number of bits per cell. i.e., 1 means SLC.
 * @ecc_strength_ds:	[INTERN] ECC correctability from the datasheet.
 *			Minimum amount of bit errors per @ecc_step_ds guaranteed
 *			to be correctable. If unknown, set to zero.
 * @ecc_step_ds:	[INTERN] ECC step required by the @ecc_strength_ds,
 *                      also from the datasheet. It is the recommended ECC step
 *			size, if known; if unknown, set to zero.
 * @onfi_timing_mode_default: [INTERN] default ONFI timing mode. This field is
 *			      set to the actually used ONFI mode if the chip is
 *			      ONFI compliant or deduced from the datasheet if
 *			      the NAND chip is not ONFI compliant.
 * @numchips:		[INTERN] number of physical chips
 * @chipsize:		[INTERN] the size of one chip for multichip arrays
 * @pagemask:		[INTERN] page number mask = number of (pages / chip) - 1
 * @pagebuf:		[INTERN] holds the pagenumber which is currently in
 *			data_buf.
 * @pagebuf_bitflips:	[INTERN] holds the bitflip count for the page which is
 *			currently in data_buf.
 * @subpagesize:	[INTERN] holds the subpagesize
 * @onfi_version:	[INTERN] holds the chip ONFI version (BCD encoded),
 *			non 0 if ONFI supported.
 * @jedec_version:	[INTERN] holds the chip JEDEC version (BCD encoded),
 *			non 0 if JEDEC supported.
 * @onfi_params:	[INTERN] holds the ONFI page parameter when ONFI is
 *			supported, 0 otherwise.
 * @jedec_params:	[INTERN] holds the JEDEC parameter page when JEDEC is
 *			supported, 0 otherwise.
 * @read_retries:	[INTERN] the number of read retry modes supported
 * @onfi_set_features:	[REPLACEABLE] set the features for ONFI nand
 * @onfi_get_features:	[REPLACEABLE] get the features for ONFI nand
 * @setup_data_interface: [OPTIONAL] setup the data interface and timing. If
 *			  chipnr is set to %NAND_DATA_IFACE_CHECK_ONLY this
 *			  means the configuration should not be applied but
 *			  only checked.
 * @bbt:		[INTERN] bad block table pointer
 * @bbt_td:		[REPLACEABLE] bad block table descriptor for flash
 *			lookup.
 * @bbt_md:		[REPLACEABLE] bad block table mirror descriptor
 * @badblock_pattern:	[REPLACEABLE] bad block scan pattern used for initial
 *			bad block scan.
 * @controller:		[REPLACEABLE] a pointer to a hardware controller
 *			structure which is shared among multiple independent
 *			devices.
 * @priv:		[OPTIONAL] pointer to private chip data
 * @write_page:		[REPLACEABLE] High-level page write function
 */

struct nand_chip {
	struct mtd_info mtd;
	void /* __iomem */ *IO_ADDR_R;
	void /* __iomem */ *IO_ADDR_W;

	int flash_node;

	uint8_t (*read_byte)(struct mtd_info *mtd);
	uint16_t (*read_word)(struct mtd_info *mtd);
	void (*write_byte)(struct mtd_info *mtd, uint8_t byte);
	void (*write_buf)(struct mtd_info *mtd, const uint8_t *buf, int len);
	void (*read_buf)(struct mtd_info *mtd, uint8_t *buf, int len);
	void (*select_chip)(struct mtd_info *mtd, int chip);
	int (*block_bad)(struct mtd_info *mtd, loff_t ofs);
	int (*block_markbad)(struct mtd_info *mtd, loff_t ofs);
	void (*cmd_ctrl)(struct mtd_info *mtd, int dat, unsigned int ctrl);
	int (*dev_ready)(struct mtd_info *mtd);
	void (*cmdfunc)(struct mtd_info *mtd, unsigned command, int column,
			int page_addr);
	int(*waitfunc)(struct mtd_info *mtd, struct nand_chip *this);
	int (*erase)(struct mtd_info *mtd, int page);
	int (*scan_bbt)(struct mtd_info *mtd);
	int (*write_page)(struct mtd_info *mtd, struct nand_chip *chip,
			uint32_t offset, int data_len, const uint8_t *buf,
			int oob_required, int page, int raw);
	int (*onfi_set_features)(struct mtd_info *mtd, struct nand_chip *chip,
			int feature_addr, uint8_t *subfeature_para);
	int (*onfi_get_features)(struct mtd_info *mtd, struct nand_chip *chip,
			int feature_addr, uint8_t *subfeature_para);
	int (*setup_read_retry)(struct mtd_info *mtd, int retry_mode);
//	int (*setup_data_interface)(struct mtd_info *mtd, int chipnr,
//				    const struct nand_data_interface *conf);


	int chip_delay;
	unsigned int options;
	unsigned int bbt_options;

	int page_shift;
	int phys_erase_shift;
	int bbt_erase_shift;
	int chip_shift;
	int numchips;
	uint64_t chipsize;
	int pagemask;
	int pagebuf;
	unsigned int pagebuf_bitflips;
	int subpagesize;
	uint8_t bits_per_cell;
	uint16_t ecc_strength_ds;
	uint16_t ecc_step_ds;
	int onfi_timing_mode_default;
	int badblockpos;
	int badblockbits;

	int onfi_version;
	int jedec_version;
	struct nand_onfi_params	onfi_params;
	struct nand_jedec_params jedec_params;

	//struct nand_data_interface *data_interface;

	int read_retries;

	flstate_t state;

	uint8_t *oob_poi;
	struct nand_hw_control *controller;
	struct nand_ecclayout *ecclayout;

	struct nand_ecc_ctrl ecc;
	struct nand_buffers *buffers;
	unsigned long buf_align;
	//struct nand_hw_control hwcontrol;

	uint8_t *bbt;
	struct nand_bbt_descr *bbt_td;
	struct nand_bbt_descr *bbt_md;

	struct nand_bbt_descr *badblock_pattern;

//	void *priv;
};

//static void nand_set_flash_node(struct nand_chip *chip,
//				       ofnode node)
//{
//	chip->flash_node = ofnode_to_offset(node);
//}
//
//static ofnode nand_get_flash_node(struct nand_chip *chip)
//{
//	return offset_to_ofnode(chip->flash_node);
//}
//
//static struct nand_chip *mtd_to_nand(struct mtd_info *mtd)
//{
//	return container_of(mtd, struct nand_chip, mtd);
//}
//
//static struct mtd_info *nand_to_mtd(struct nand_chip *chip)
//{
//	return &chip->mtd;
//}
//
//static void *nand_get_controller_data(struct nand_chip *chip)
//{
//	return chip->priv;
//}
//
//static void nand_set_controller_data(struct nand_chip *chip, void *priv)
//{
//	chip->priv = priv;
//}

 static struct nand_chip nand_chip0;
 static struct mcom02_nand_priv nand_priv0;
 static struct mtd_info mtd_info0;
/*
 * NAND Flash Manufacturer ID Codes
 */
#define NAND_MFR_TOSHIBA	0x98
#define NAND_MFR_SAMSUNG	0xec
#define NAND_MFR_FUJITSU	0x04
#define NAND_MFR_NATIONAL	0x8f
#define NAND_MFR_RENESAS	0x07
#define NAND_MFR_STMICRO	0x20
#define NAND_MFR_HYNIX		0xad
#define NAND_MFR_MICRON		0x2c
#define NAND_MFR_AMD		0x01
#define NAND_MFR_MACRONIX	0xc2
#define NAND_MFR_EON		0x92
#define NAND_MFR_SANDISK	0x45
#define NAND_MFR_INTEL		0x89
#define NAND_MFR_ATO		0x9b

/* The maximum expected count of bytes in the NAND ID sequence */
#define NAND_MAX_ID_LEN 8

/*
 * A helper for defining older NAND chips where the second ID byte fully
 * defined the chip, including the geometry (chip size, eraseblock size, page
 * size). All these chips have 512 bytes NAND page size.
 */
#define LEGACY_ID_NAND(nm, devid, chipsz, erasesz, opts)          \
	{ .name = (nm), {{ .dev_id = (devid) }}, .pagesize = 512, \
	  .chipsize = (chipsz), .erasesize = (erasesz), .options = (opts) }

/*
 * A helper for defining newer chips which report their page size and
 * eraseblock size via the extended ID bytes.
 *
 * The real difference between LEGACY_ID_NAND and EXTENDED_ID_NAND is that with
 * EXTENDED_ID_NAND, manufacturers overloaded the same device ID so that the
 * device ID now only represented a particular total chip size (and voltage,
 * buswidth), and the page size, eraseblock size, and OOB size could vary while
 * using the same device ID.
 */
#define EXTENDED_ID_NAND(nm, devid, chipsz, opts)                      \
	{ .name = (nm), {{ .dev_id = (devid) }}, .chipsize = (chipsz), \
	  .options = (opts) }

#define NAND_ECC_INFO(_strength, _step)	\
			{ .strength_ds = (_strength), .step_ds = (_step) }
#define NAND_ECC_STRENGTH(type)		((type)->ecc.strength_ds)
#define NAND_ECC_STEP(type)		((type)->ecc.step_ds)

/**
 * struct nand_flash_dev - NAND Flash Device ID Structure
 * @name: a human-readable name of the NAND chip
 * @dev_id: the device ID (the second byte of the full chip ID array)
 * @mfr_id: manufecturer ID part of the full chip ID array (refers the same
 *          memory address as @id[0])
 * @dev_id: device ID part of the full chip ID array (refers the same memory
 *          address as @id[1])
 * @id: full device ID array
 * @pagesize: size of the NAND page in bytes; if 0, then the real page size (as
 *            well as the eraseblock size) is determined from the extended NAND
 *            chip ID array)
 * @chipsize: total chip size in MiB
 * @erasesize: eraseblock size in bytes (determined from the extended ID if 0)
 * @options: stores various chip bit options
 * @id_len: The valid length of the @id.
 * @oobsize: OOB size
 * @ecc: ECC correctability and step information from the datasheet.
 * @ecc.strength_ds: The ECC correctability from the datasheet, same as the
 *                   @ecc_strength_ds in nand_chip{}.
 * @ecc.step_ds: The ECC step required by the @ecc.strength_ds, same as the
 *               @ecc_step_ds in nand_chip{}, also from the datasheet.
 *               For example, the "4bit ECC for each 512Byte" can be set with
 *               NAND_ECC_INFO(4, 512).
 * @onfi_timing_mode_default: the default ONFI timing mode entered after a NAND
 *			      reset. Should be deduced from timings described
 *			      in the datasheet.
 *
 */
struct nand_flash_dev {
	char *name;
	union {
		struct {
			uint8_t mfr_id;
			uint8_t dev_id;
		};
		uint8_t id[NAND_MAX_ID_LEN];
	};
	unsigned int pagesize;
	unsigned int chipsize;
	unsigned int erasesize;
	unsigned int options;
	uint16_t id_len;
	uint16_t oobsize;
	struct {
		uint16_t strength_ds;
		uint16_t step_ds;
	} ecc;
	int onfi_timing_mode_default;
};

/**
 * struct nand_manufacturers - NAND Flash Manufacturer ID Structure
 * @name:	Manufacturer name
 * @id:		manufacturer ID code of device.
*/
struct nand_manufacturers {
	int id;
	char *name;
};

extern struct nand_flash_dev nand_flash_ids[];
extern struct nand_manufacturers nand_manuf_ids[];

int nand_default_bbt(struct mtd_info *mtd);
int nand_markbad_bbt(struct mtd_info *mtd, loff_t offs);
int nand_isreserved_bbt(struct mtd_info *mtd, loff_t offs);
int nand_isbad_bbt(struct mtd_info *mtd, loff_t offs, int allowbbt);
int nand_erase_nand(struct mtd_info *mtd, struct erase_info *instr,
			   int allowbbt);
int nand_do_read(struct mtd_info *mtd, loff_t from, size_t len,
			size_t *retlen, uint8_t *buf);

/*
* Constants for oob configuration
*/
#define NAND_SMALL_BADBLOCK_POS		5
#define NAND_LARGE_BADBLOCK_POS		0

/**
 * struct platform_nand_chip - chip level device structure
 * @nr_chips:		max. number of chips to scan for
 * @chip_offset:	chip number offset
 * @nr_partitions:	number of partitions pointed to by partitions (or zero)
 * @partitions:		mtd partition list
 * @chip_delay:		R/B delay value in us
 * @options:		Option flags, e.g. 16bit buswidth
 * @bbt_options:	BBT option flags, e.g. NAND_BBT_USE_FLASH
 * @part_probe_types:	NULL-terminated array of probe types
 */
struct platform_nand_chip {
	int nr_chips;
	int chip_offset;
	int nr_partitions;
	struct mtd_partition *partitions;
	int chip_delay;
	unsigned int options;
	unsigned int bbt_options;
	const char **part_probe_types;
};

/* Keep gcc happy */
struct platform_device;

/**
 * struct platform_nand_ctrl - controller level device structure
 * @probe:		platform specific function to probe/setup hardware
 * @remove:		platform specific function to remove/teardown hardware
 * @hwcontrol:		platform specific hardware control structure
 * @dev_ready:		platform specific function to read ready/busy pin
 * @select_chip:	platform specific chip select function
 * @cmd_ctrl:		platform specific function for controlling
 *			ALE/CLE/nCE. Also used to write command and address
 * @write_buf:		platform specific function for write buffer
 * @read_buf:		platform specific function for read buffer
 * @read_byte:		platform specific function to read one byte from chip
 * @priv:		private data to transport driver specific settings
 *
 * All fields are optional and depend on the hardware driver requirements
 */
struct platform_nand_ctrl {
	int (*probe)(struct platform_device *pdev);
	void (*remove)(struct platform_device *pdev);
	void (*hwcontrol)(struct mtd_info *mtd, int cmd);
	int (*dev_ready)(struct mtd_info *mtd);
	void (*select_chip)(struct mtd_info *mtd, int chip);
	void (*cmd_ctrl)(struct mtd_info *mtd, int dat, unsigned int ctrl);
	void (*write_buf)(struct mtd_info *mtd, const uint8_t *buf, int len);
	void (*read_buf)(struct mtd_info *mtd, uint8_t *buf, int len);
	unsigned char (*read_byte)(struct mtd_info *mtd);
	void *priv;
};

/**
 * struct platform_nand_data - container structure for platform-specific data
 * @chip:		chip level chip structure
 * @ctrl:		controller level device structure
 */
struct platform_nand_data {
	struct platform_nand_chip chip;
	struct platform_nand_ctrl ctrl;
};

#ifdef CONFIG_SYS_NAND_ONFI_DETECTION
/* return the supported features. */
static int onfi_feature(struct nand_chip *chip)
{
	return chip->onfi_version ? le16_to_cpu(chip->onfi_params.features) : 0;
}

/* return the supported asynchronous timing mode. */
static int onfi_get_async_timing_mode(struct nand_chip *chip)
{
	if (!chip->onfi_version)
		return ONFI_TIMING_MODE_UNKNOWN;
	return le16_to_cpu(chip->onfi_params.async_timing_mode);
}

/* return the supported synchronous timing mode. */
static int onfi_get_sync_timing_mode(struct nand_chip *chip)
{
	if (!chip->onfi_version)
		return ONFI_TIMING_MODE_UNKNOWN;
	return le16_to_cpu(chip->onfi_params.src_sync_timing_mode);
}
#else
static int onfi_feature(struct nand_chip *chip)
{
	return 0;
}

static int onfi_get_async_timing_mode(struct nand_chip *chip)
{
	return ONFI_TIMING_MODE_UNKNOWN;
}

static int onfi_get_sync_timing_mode(struct nand_chip *chip)
{
	return ONFI_TIMING_MODE_UNKNOWN;
}
#endif

int onfi_init_data_interface(struct nand_chip *chip,
			     struct nand_data_interface *iface,
			     enum nand_data_interface_type type,
			     int timing_mode);

/*
 * Check if it is a SLC nand.
 * The !nand_is_slc() can be used to check the MLC/TLC nand chips.
 * We do not distinguish the MLC and TLC now.
 */
static int nand_is_slc(struct nand_chip *chip)
{
	return chip->bits_per_cell == 1;
}

/**
 * Check if the opcode's address should be sent only on the lower 8 bits
 * @command: opcode to check
 */
static int nand_opcode_8bits(unsigned int command)
{
	switch (command) {
	case NAND_CMD_READID:
	case NAND_CMD_PARAM:
	case NAND_CMD_GET_FEATURES:
	case NAND_CMD_SET_FEATURES:
		return 1;
	default:
		break;
	}
	return 0;
}

/* return the supported JEDEC features. */
static int jedec_feature(struct nand_chip *chip)
{
	return chip->jedec_version ? le16_to_cpu(chip->jedec_params.features)
		: 0;
}

/* Standard NAND functions from nand_base.c */
void nand_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len);
void nand_write_buf16(struct mtd_info *mtd, const uint8_t *buf, int len);
void nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len);
void nand_read_buf16(struct mtd_info *mtd, uint8_t *buf, int len);
uint8_t nand_read_byte(struct mtd_info *mtd);

/* get timing characteristics from ONFI timing mode. */
const struct nand_sdr_timings *onfi_async_timing_mode_to_sdr_timings(int mode);
/* get data interface from ONFI timing mode 0, used after reset. */
const struct nand_data_interface *nand_get_default_data_interface(void);

int nand_check_erased_ecc_chunk(void *data, int datalen,
				void *ecc, int ecclen,
				void *extraoob, int extraooblen,
				int threshold);

int nand_check_ecc_caps(struct nand_chip *chip,
			const struct nand_ecc_caps *caps, int oobavail);

int nand_match_ecc_req(struct nand_chip *chip,
		       const struct nand_ecc_caps *caps,  int oobavail);

int nand_maximize_ecc(struct nand_chip *chip,
		      const struct nand_ecc_caps *caps, int oobavail);

/* Reset and initialize a NAND device */
int nand_reset(struct nand_chip *chip, int chipnr);

/* NAND operation helpers */
int nand_reset_op(struct nand_chip *chip);
int nand_readid_op(struct nand_chip *chip, uint8_t addr, void *buf,
		   unsigned int len);
int nand_status_op(struct nand_chip *chip, uint8_t *status);
int nand_exit_status_op(struct nand_chip *chip);
int nand_erase_op(struct nand_chip *chip, unsigned int eraseblock);
int nand_read_page_op(struct nand_chip *chip, unsigned int page,
		      unsigned int offset_in_page, void *buf, unsigned int len);
int nand_change_read_column_op(struct nand_chip *chip,
			       unsigned int offset_in_page, void *buf,
			       unsigned int len, int force_8bit);
int nand_read_oob_op(struct nand_chip *chip, unsigned int page,
		     unsigned int offset_in_page, void *buf, unsigned int len);
int nand_prog_page_begin_op(struct nand_chip *chip, unsigned int page,
			    unsigned int offset_in_page, const void *buf,
			    unsigned int len);
int nand_prog_page_end_op(struct nand_chip *chip);
int nand_prog_page_op(struct nand_chip *chip, unsigned int page,
		      unsigned int offset_in_page, const void *buf,
		      unsigned int len);
int nand_change_write_column_op(struct nand_chip *chip,
				unsigned int offset_in_page, const void *buf,
				unsigned int len, int force_8bit);
int nand_read_data_op(struct nand_chip *chip, void *buf, unsigned int len,
		      int force_8bit);
int nand_write_data_op(struct nand_chip *chip, const void *buf,
		       unsigned int len, int force_8bit);

#endif /* __LINUX_MTD_RAWNAND_H */

// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2014 - 2015 Xilinx, Inc.
 * Copyright (C) 2015 ELVEES NeoTek, CJSC
 * Copyright 2018 RnD Center "ELVEES", JSC
 * Based on the Linux version of Arasan NFC driver.
 *
 */

//#include <asm/io.h>
//#include <common.h>
//#include <dm.h>
//#include <fdtdec.h>
//#include <nand.h>
//#include <linux/errno.h>

/* Register offsets */
#define PKT_OFST			0x00
#define MEM_ADDR1_OFST			0x04
#define MEM_ADDR2_OFST			0x08
#define CMD_OFST			0x0C
#define PROG_OFST			0x10
#define INTR_STS_EN_OFST		0x14
#define INTR_SIG_EN_OFST		0x18
#define INTR_STS_OFST			0x1C
#define ID1_OFST			0x20
#define ID2_OFST			0x24
#define FLASH_STS_OFST			0x28
#define DATA_PORT_OFST			0x30
#define ECC_OFST			0x34
#define ECC_ERR_CNT_OFST		0x38
#define ECC_SPR_CMD_OFST		0x3C
#define ECC_ERR_CNT_1BIT_OFST		0x40
#define ECC_ERR_CNT_2BIT_OFST		0x44

#define PKT_CNT_SHIFT			12

#define ECC_ENABLE			NAND_BIT(31)
#define PAGE_SIZE_MASK			NAND_GENMASK(25, 23)
#define PAGE_SIZE_SHIFT			23
#define PAGE_SIZE_512			0
#define PAGE_SIZE_2K			1
#define PAGE_SIZE_4K			2
#define PAGE_SIZE_8K			3
#define CMD2_SHIFT			8
#define ADDR_CYCLES_SHIFT		28

#define XFER_COMPLETE			NAND_BIT(2)
#define READ_READY			NAND_BIT(1)
#define WRITE_READY			NAND_BIT(0)
#define MBIT_ERROR			NAND_BIT(3)
#define ERR_INTRPT			NAND_BIT(4)

#define PROG_PGRD			NAND_BIT(0)
#define PROG_ERASE			NAND_BIT(2)
#define PROG_STATUS			NAND_BIT(3)
#define PROG_PGPROG			NAND_BIT(4)
#define PROG_RDID			NAND_BIT(6)
#define PROG_RDPARAM			NAND_BIT(7)
#define PROG_RST			NAND_BIT(8)

#define ONFI_STATUS_FAIL		NAND_BIT(0)
#define ONFI_STATUS_READY		NAND_BIT(6)

#define PG_ADDR_SHIFT			16
#define BCH_MODE_SHIFT			25
#define BCH_EN_SHIFT			25
#define ECC_SIZE_SHIFT			16

#define MEM_ADDR_MASK			NAND_GENMASK(7, 0)
#define BCH_MODE_MASK			NAND_GENMASK(27, 25)

#define CS_MASK				NAND_GENMASK(31, 30)
#define CS_SHIFT			30

#define PAGE_ERR_CNT_MASK		NAND_GENMASK(16, 8)
#define PAGE_ERR_CNT_SHIFT              8
#define PKT_ERR_CNT_MASK		NAND_GENMASK(7, 0)

#define ONFI_ID_ADDR			0x20
#define ONFI_ID_LEN			4
#define MAF_ID_LEN			5
#define DMA_BUFSIZE			SZ_64K
#define TEMP_BUF_SIZE			512
#define SPARE_ADDR_CYCLES		NAND_BIT(29)

#define ARASAN_NAND_POLL_TIMEOUT	1000000
#define STATUS_TIMEOUT			2000

//DECLARE_GLOBAL_DATA_PTR;

struct mcom02_nand_priv {
	//struct udevice *dev;
	//struct nand_chip nand;

	//void /* __iomem */ *regs;

	uint8_t buf[TEMP_BUF_SIZE];

	int bch;
	int err;
	int iswriteoob;
	uint16_t raddr_cycles;
	uint16_t caddr_cycles;
	uint32_t page;
	uint32_t bufshift;
	uint32_t rdintrmask;
	uint32_t pktsize;
	uint32_t ecc_regval;

	int curr_cmd;
	struct nand_ecclayout ecclayout;
};

struct mcom02_nand_ecc_matrix {
	uint32_t pagesize;
	uint32_t codeword_size;
	uint8_t eccbits;
	uint8_t bch;
	uint16_t eccsize;
};

static const struct mcom02_nand_ecc_matrix ecc_matrix[] = {
	{512,	512,	1,	0,	0x3},
	{512,	512,	4,	1,	0x7},
	{512,	512,	8,	1,	0xD},
	/* 2K byte page */
	{2048,	512,	1,	0,	0xC},
	{2048,	512,	8,	1,	0x34},
	{2048,	512,	12,	1,	0x4E},
	{2048,	512,	16,	1,	0x68},
	{2048,	1024,	24,	1,	0x54},
	/* 4K byte page */
	{4096,	512,	1,	0,	0x18},
	{4096,	512,	8,	1,	0x68},
	{4096,	512,	16,	1,	0xD0},
	/* 8K byte page */
	{8192,	512,	1,	0,	0x30},
	{8192,	512,	8,	1,	0xD0},
	{8192,	512,	12,	1,	0x138},
	{8192,	512,	16,	1,	0x1A0},
};

static uint8_t mcom02_nand_page(uint32_t pagesize)
{
	switch (pagesize) {
	case 512:
		return PAGE_SIZE_512;
	case 2048:
		return PAGE_SIZE_2K;
	case 4096:
		return PAGE_SIZE_4K;
	case 8192:
		return PAGE_SIZE_8K;
	default:
		PRINTF("Unsupported page size: %#x\n", (unsigned) pagesize);
		break;
	}

	return 0;
}

static void mcom02_nand_prepare_cmd(struct mcom02_nand_priv *priv, uint8_t cmd1,
				    uint8_t cmd2, uint32_t pagesize, uint8_t addrcycles)
{
	uint32_t regval;

	regval = cmd1 | (cmd2 << CMD2_SHIFT);
	if (addrcycles)
		regval |= addrcycles << ADDR_CYCLES_SHIFT;
	if (pagesize)
		regval |= mcom02_nand_page(pagesize) << PAGE_SIZE_SHIFT;
	writel(regval, NANDMPORT_BASE + CMD_OFST);
}

static void mcom02_nand_setpagecoladdr(struct mcom02_nand_priv *priv,
				       uint32_t page, uint16_t col)
{
	uint32_t val;

	writel(col | (page << PG_ADDR_SHIFT), NANDMPORT_BASE + MEM_ADDR1_OFST);

	val = readl(NANDMPORT_BASE + MEM_ADDR2_OFST);
	val = (val & ~MEM_ADDR_MASK) |
	      ((page >> PG_ADDR_SHIFT) & MEM_ADDR_MASK);
	writel(val, NANDMPORT_BASE + MEM_ADDR2_OFST);
}

static void mcom02_nand_setpktszcnt(struct mcom02_nand_priv *priv,
					   uint32_t pktsize, uint32_t pktcount)
{
	writel(pktsize | (pktcount << PKT_CNT_SHIFT), NANDMPORT_BASE + PKT_OFST);
}

static void mcom02_nand_set_irq_masks(struct mcom02_nand_priv *priv,
					     uint32_t val)
{
	writel(val, NANDMPORT_BASE + INTR_STS_EN_OFST);
}

static void mcom02_nand_wait_for_event_debug(struct mcom02_nand_priv *priv,
				       uint32_t event,
					   const char * file, int line)
{
	uint32_t timeout = ARASAN_NAND_POLL_TIMEOUT;

	while (!(readl(NANDMPORT_BASE + INTR_STS_OFST) & event) && timeout) {
		udelay(1);
		timeout--;
	}

	if (!timeout)
		PRINTF("Event waiting timeout, event=%02x, %s/%d\n", (unsigned) event, file, line);

	writel(event, NANDMPORT_BASE + INTR_STS_OFST);
}

#define mcom02_nand_wait_for_event(p, e) do { mcom02_nand_wait_for_event_debug((p), (e), __FILE__, __LINE__); } while (0)

static void (mcom02_nand_wait_for_event)(struct mcom02_nand_priv *priv,
				       uint32_t event)
{
	uint32_t timeout = ARASAN_NAND_POLL_TIMEOUT;

	while (!(readl(NANDMPORT_BASE + INTR_STS_OFST) & event) && timeout) {
		udelay(1);
		timeout--;
	}

	if (!timeout)
		PRINTF("Event waiting timeout, event=%02x\n", (unsigned) event);

	writel(event, NANDMPORT_BASE + INTR_STS_OFST);
}

static void mcom02_nand_readfifo(struct mcom02_nand_priv *priv, uint32_t prog,
				 uint32_t size)
{
	uint32_t i, *bufptr = (uint32_t *)&priv->buf[0];

	mcom02_nand_set_irq_masks(priv, READ_READY);

	writel(prog, NANDMPORT_BASE + PROG_OFST);
	mcom02_nand_wait_for_event(priv, READ_READY);

	mcom02_nand_set_irq_masks(priv, XFER_COMPLETE);

	for (i = 0; i < size / 4; i++)
		bufptr[i] = readl(NANDMPORT_BASE + DATA_PORT_OFST);

	mcom02_nand_wait_for_event(priv, XFER_COMPLETE);
}

static void mcom02_nand_cmdfunc(struct mtd_info *mtd, unsigned int cmd,
				int column, int page_addr)
{
	struct nand_chip *nand = & nand_chip0; //mtd_to_nand(mtd);
	struct mcom02_nand_priv *priv = & nand_priv0; //nand_get_controller_data(nand);
	int wait = 0 /* false */, read = 0 /* false */, read_id = 0 /* false */;
	uint32_t addrcycles, prog;
	uint32_t *bufptr = (uint32_t *)&priv->buf[0];

	priv->bufshift = 0;
	priv->curr_cmd = cmd;

	if (page_addr == -1)
		page_addr = 0;
	if (column == -1)
		column = 0;

	switch (cmd) {
	case NAND_CMD_RESET:
		mcom02_nand_prepare_cmd(priv, cmd, 0, 0, 0);
		prog = PROG_RST;
		wait = 1 /* true */;
		break;
	case NAND_CMD_SEQIN:
		addrcycles = priv->raddr_cycles + priv->caddr_cycles;
		priv->page = page_addr;
		mcom02_nand_prepare_cmd(priv, cmd, NAND_CMD_PAGEPROG,
					mtd->writesize, addrcycles);
		mcom02_nand_setpagecoladdr(priv, page_addr, column);
		break;
	case NAND_CMD_READOOB:
		column += mtd->writesize;
	case NAND_CMD_READ0:
	case NAND_CMD_READ1:
		addrcycles = priv->raddr_cycles + priv->caddr_cycles;
		mcom02_nand_prepare_cmd(priv, NAND_CMD_READ0,
					NAND_CMD_READSTART,
					mtd->writesize, addrcycles);
		mcom02_nand_setpagecoladdr(priv, page_addr, column);
		break;
	case NAND_CMD_RNDOUT:
		mcom02_nand_prepare_cmd(priv, cmd, NAND_CMD_RNDOUTSTART,
					mtd->writesize, 2);
		mcom02_nand_setpagecoladdr(priv, page_addr, column);
		priv->rdintrmask = READ_READY;
		break;
	case NAND_CMD_PARAM:
		mcom02_nand_prepare_cmd(priv, cmd, 0, 0, 1);
		mcom02_nand_setpagecoladdr(priv, page_addr, column);
		mcom02_nand_setpktszcnt(priv, sizeof(struct nand_onfi_params),
					1);
		mcom02_nand_readfifo(priv, PROG_RDPARAM,
				     sizeof(struct nand_onfi_params));
		break;
	case NAND_CMD_READID:
		mcom02_nand_prepare_cmd(priv, cmd, 0, 0, 1);
		mcom02_nand_setpagecoladdr(priv, page_addr, column);
		if (column == ONFI_ID_ADDR)
			mcom02_nand_setpktszcnt(priv, ONFI_ID_LEN, 1);
		else
			mcom02_nand_setpktszcnt(priv, MAF_ID_LEN, 1);
		prog = PROG_RDID;
		wait = 1 /* true */;
		read_id = 1 /* true */;
		break;
	case NAND_CMD_ERASE1:
		addrcycles = priv->raddr_cycles;
		mcom02_nand_prepare_cmd(priv, cmd, NAND_CMD_ERASE2,
					0, addrcycles);
		column = page_addr & 0xffff;
		page_addr = (page_addr >> PG_ADDR_SHIFT) & 0xffff;
		mcom02_nand_setpagecoladdr(priv, page_addr, column);
		prog = PROG_ERASE;
		wait = 1 /* true */;
		break;
	case NAND_CMD_STATUS:
		mcom02_nand_prepare_cmd(priv, cmd, 0, 0, 0);
		mcom02_nand_setpktszcnt(priv, 1, 1);
		mcom02_nand_setpagecoladdr(priv, page_addr, column);
		prog = PROG_STATUS;
		wait = 1 /* true */;
		read = 1 /* true */;
		break;
	default:
		return;
	}

	if (wait) {
		mcom02_nand_set_irq_masks(priv, XFER_COMPLETE);
		writel(prog, NANDMPORT_BASE + PROG_OFST);
		mcom02_nand_wait_for_event(priv, XFER_COMPLETE);
	}

	if (read)
		bufptr[0] = readl(NANDMPORT_BASE + FLASH_STS_OFST);
	if (read_id) {
		bufptr[0] = readl(NANDMPORT_BASE + ID1_OFST);
		bufptr[1] = readl(NANDMPORT_BASE + ID2_OFST);
		if (column == ONFI_ID_ADDR)
			bufptr[0] = ((bufptr[0] >> 8) | (bufptr[1] << 24));
	}
}

static void mcom02_nand_select_chip(struct mtd_info *mtd, int chip)
{
	struct nand_chip *nand = & nand_chip0; //mtd_to_nand(mtd);
	struct mcom02_nand_priv *priv = & nand_priv0; //nand_get_controller_data(nand);
	uint32_t val;

	if (chip == -1)
		return;

	val = readl(NANDMPORT_BASE + MEM_ADDR2_OFST);
	val = (val & ~(CS_MASK)) | (chip << CS_SHIFT);
	writel(val, NANDMPORT_BASE + MEM_ADDR2_OFST);
}

static uint8_t mcom02_nand_read_byte(struct mtd_info *mtd)
{
	struct nand_chip *nand = & nand_chip0; //mtd_to_nand(mtd);
	struct mcom02_nand_priv *priv = & nand_priv0; //nand_get_controller_data(nand);

	return priv->buf[priv->bufshift++];
}

static void mcom02_nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int size)
{
	struct nand_chip *nand = & nand_chip0; //mtd_to_nand(mtd);
	struct mcom02_nand_priv *priv = & nand_priv0; //nand_get_controller_data(nand);
	uint32_t i, pktcount, buf_rd_cnt = 0, pktsize;
	uint32_t *bufptr = (uint32_t *)buf;

	priv->rdintrmask |= READ_READY;

	if (priv->curr_cmd == NAND_CMD_READ0) {
		pktsize = priv->pktsize;
		if (mtd->writesize % pktsize)
			pktcount = mtd->writesize / pktsize + 1;
		else
			pktcount = mtd->writesize / pktsize;
	} else {
		pktsize = size;
		pktcount = 1;
	}

	mcom02_nand_setpktszcnt(priv, pktsize, pktcount);

	mcom02_nand_set_irq_masks(priv, priv->rdintrmask);
	writel(PROG_PGRD, NANDMPORT_BASE + PROG_OFST);

	while (buf_rd_cnt < pktcount) {
		mcom02_nand_wait_for_event(priv, READ_READY);
		buf_rd_cnt++;

		if (buf_rd_cnt == pktcount)
			mcom02_nand_set_irq_masks(priv, XFER_COMPLETE);

		for (i = 0; i < pktsize / 4; i++)
			bufptr[i] = readl(NANDMPORT_BASE + DATA_PORT_OFST);

		bufptr += (pktsize / 4);

		if (buf_rd_cnt < pktcount)
			mcom02_nand_set_irq_masks(priv, priv->rdintrmask);
	}

	mcom02_nand_wait_for_event(priv, XFER_COMPLETE);
	priv->rdintrmask = 0;
}

// CHANGE READ COLUMN (05h-E0h) command
static void mcom02_nand_set_eccsparecmd(struct mcom02_nand_priv *priv,
					       uint8_t cmd1, uint8_t cmd2)
{
	PRINTF("mcom02_nand_set_eccsparecmd: cmd1=%02X, cmd2=%02x\n", cmd1, cmd2);
	writel(cmd1 | (cmd2 << CMD2_SHIFT) |
	      (priv->caddr_cycles << ADDR_CYCLES_SHIFT) | SPARE_ADDR_CYCLES,
	       NANDMPORT_BASE + ECC_SPR_CMD_OFST);
}

static int mcom02_nand_read_page_hwecc(struct mtd_info *mtd,
				       struct nand_chip *nand, uint8_t *buf,
				       int oob_required, int page)
{
	struct mcom02_nand_priv *priv = & nand_priv0; //nand_get_controller_data(nand);
	uint32_t val;

	mcom02_nand_set_eccsparecmd(priv, NAND_CMD_RNDOUT,
				    NAND_CMD_RNDOUTSTART);
	writel(priv->ecc_regval, NANDMPORT_BASE + ECC_OFST);

	val = readl(NANDMPORT_BASE + CMD_OFST);
	val = val | ECC_ENABLE;
	writel(val, NANDMPORT_BASE + CMD_OFST);

	if (!priv->bch)
		priv->rdintrmask = MBIT_ERROR;

	nand->read_buf(mtd, buf, mtd->writesize);

	val = readl(NANDMPORT_BASE + ECC_ERR_CNT_OFST);
	if (priv->bch) {
		mtd->ecc_stats.corrected +=
			(val & PAGE_ERR_CNT_MASK) >> PAGE_ERR_CNT_SHIFT;
	} else {
		val = readl(NANDMPORT_BASE + ECC_ERR_CNT_1BIT_OFST);
		mtd->ecc_stats.corrected += val;
		val = readl(NANDMPORT_BASE + ECC_ERR_CNT_2BIT_OFST);
		mtd->ecc_stats.failed += val;
		/* clear ecc error count register 1Bit, 2Bit */
		writel(0x0, NANDMPORT_BASE + ECC_ERR_CNT_1BIT_OFST);
		writel(0x0, NANDMPORT_BASE + ECC_ERR_CNT_2BIT_OFST);
	}
	priv->err = 0 /* false */;

	if (oob_required)
		nand->ecc.read_oob(mtd, nand, page);

	return 0;
}

static int mcom02_nand_device_ready(struct mtd_info *mtd,
				    struct nand_chip *nand)
{
	uint8_t status;
	uint32_t timeout = STATUS_TIMEOUT;

	while (timeout--) {
		nand->cmdfunc(mtd, NAND_CMD_STATUS, 0, 0);
		status = nand->read_byte(mtd);

		if (status & ONFI_STATUS_READY) {
			if (status & ONFI_STATUS_FAIL)
				return NAND_STATUS_FAIL;

			return 0;
		}
	}

	PRINTF("Device ready timedout\n");

	return -ETIMEDOUT;
}

static void mcom02_nand_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	struct nand_chip *nand = & nand_chip0; //mtd_to_nand(mtd);
	struct mcom02_nand_priv *priv = & nand_priv0; //nand_get_controller_data(nand);
	uint32_t buf_wr_cnt = 0, pktcount = 1, i, pktsize;
	uint32_t *bufptr = (uint32_t *)buf;

	if (priv->iswriteoob) {
		pktsize = len;
		pktcount = 1;
	} else {
		pktsize = priv->pktsize;
		pktcount = mtd->writesize / pktsize;
	}

	mcom02_nand_setpktszcnt(priv, pktsize, pktcount);

	mcom02_nand_set_irq_masks(priv, WRITE_READY);
	writel(PROG_PGPROG, NANDMPORT_BASE + PROG_OFST);

	while (buf_wr_cnt < pktcount) {
		mcom02_nand_wait_for_event(priv, WRITE_READY);

		buf_wr_cnt++;
		if (buf_wr_cnt == pktcount)
			mcom02_nand_set_irq_masks(priv, XFER_COMPLETE);

		for (i = 0; i < (pktsize / 4); i++)
			writel(bufptr[i], NANDMPORT_BASE + DATA_PORT_OFST);

		bufptr += (pktsize / 4);

		if (buf_wr_cnt < pktcount)
			mcom02_nand_set_irq_masks(priv, WRITE_READY);
	}

	mcom02_nand_wait_for_event(priv, XFER_COMPLETE);
}

static int mcom02_nand_write_page_hwecc(struct mtd_info *mtd,
					struct nand_chip *nand,
					const uint8_t *buf,
					int oob_required, int page)
{
	struct mcom02_nand_priv *priv = & nand_priv0; //nand_get_controller_data(nand);
	uint8_t *ecc_calc = nand->buffers->ecccalc;
	uint32_t *eccpos = nand->ecc.layout->eccpos;
	uint32_t val, i;

	mcom02_nand_set_eccsparecmd(priv, NAND_CMD_RNDIN, 0);
	writel(priv->ecc_regval, NANDMPORT_BASE + ECC_OFST);

	val = readl(NANDMPORT_BASE + CMD_OFST);
	val = val | ECC_ENABLE;
	writel(val, NANDMPORT_BASE + CMD_OFST);

	nand->write_buf(mtd, buf, mtd->writesize);

	if (oob_required) {
		mcom02_nand_device_ready(mtd, nand);
		nand->cmdfunc(mtd, NAND_CMD_READOOB, 0, page);
		nand->read_buf(mtd, ecc_calc, mtd->oobsize);
		for (i = 0; i < nand->ecc.total; i++)
			nand->oob_poi[eccpos[i]] = ecc_calc[eccpos[i]];
		nand->ecc.write_oob(mtd, nand, page);
	}

	return 0;
}

static int mcom02_nand_read_oob(struct mtd_info *mtd, struct nand_chip *nand,
				int page)
{
	nand->cmdfunc(mtd, NAND_CMD_READOOB, 0, page);
	nand->read_buf(mtd, nand->oob_poi, mtd->oobsize);

	return 0;
}

static int mcom02_nand_write_oob(struct mtd_info *mtd, struct nand_chip *nand,
				 int page)
{
	struct mcom02_nand_priv *priv = & nand_priv0; //nand_get_controller_data(nand);

	priv->iswriteoob = 1 /* true */;
	nand->cmdfunc(mtd, NAND_CMD_SEQIN, mtd->writesize, page);
	nand->write_buf(mtd, nand->oob_poi, mtd->oobsize);
	priv->iswriteoob = 0 /* false */;

	return 0;
}

static int mcom02_nand_ecc_init(struct mtd_info *mtd)
{
	struct nand_chip *nand = & nand_chip0; // mtd_to_nand(mtd);
	struct mcom02_nand_priv *priv = & nand_priv0; // nand_get_controller_data(nand);
	uint32_t oob_index, i, regval, eccaddr, bchmode = 0;
	int found = -1;

	nand->ecc.mode = NAND_ECC_HW;
	nand->ecc.read_page = mcom02_nand_read_page_hwecc;
	nand->ecc.write_page = mcom02_nand_write_page_hwecc;
	nand->ecc.write_oob = mcom02_nand_write_oob;
	nand->ecc.read_oob = mcom02_nand_read_oob;

	for (i = 0; i < ARRAY_SIZE(ecc_matrix); i++) {
		if (ecc_matrix[i].pagesize == mtd->writesize) {
			if (ecc_matrix[i].eccbits >=
			    nand->ecc_strength_ds) {
				found = i;
				break;
			}
			found = i;
		}
	}

	if (found < 0) {
		PRINTF("ECC scheme not supported\n");
		return 1;
	}

	if (ecc_matrix[found].bch) {
		switch (ecc_matrix[found].eccbits) {
		case 12:
			bchmode = 0x1;
			break;
		case 8:
			bchmode = 0x2;
			break;
		case 4:
			bchmode = 0x3;
			break;
		case 24:
			bchmode = 0x4;
			break;
		default:
			bchmode = 0x0;
		}
	}

	nand->ecc.strength = ecc_matrix[found].eccbits;
	nand->ecc.size = ecc_matrix[found].codeword_size;

	nand->ecc.steps = ecc_matrix[found].pagesize /
			       ecc_matrix[found].codeword_size;

	nand->ecc.bytes = ecc_matrix[found].eccsize /
			       nand->ecc.steps;

	priv->ecclayout.eccbytes = ecc_matrix[found].eccsize;
	priv->bch = ecc_matrix[found].bch;

	if (mtd->oobsize < ecc_matrix[found].eccsize + 2) {
		PRINTF("OOB too small for ECC scheme\n");
		return 1;
	}
	oob_index = mtd->oobsize - priv->ecclayout.eccbytes;
	eccaddr = mtd->writesize + oob_index;

	for (i = 0; i < nand->ecc.size; i++)
		priv->ecclayout.eccpos[i] = oob_index + i;

	priv->ecclayout.oobfree->offset = 2;
	priv->ecclayout.oobfree->length = oob_index -
		priv->ecclayout.oobfree->offset;

	nand->ecc.layout = &priv->ecclayout;

	regval = eccaddr |
		(ecc_matrix[found].eccsize << ECC_SIZE_SHIFT) |
		(ecc_matrix[found].bch << BCH_EN_SHIFT);
	priv->ecc_regval = regval;
	writel(regval, NANDMPORT_BASE + ECC_OFST);

	regval = readl(NANDMPORT_BASE + MEM_ADDR2_OFST);
	regval = (regval & ~(BCH_MODE_MASK)) | (bchmode << BCH_MODE_SHIFT);
	writel(regval, NANDMPORT_BASE + MEM_ADDR2_OFST);

	if (nand->ecc.size >= 1024)
		priv->pktsize = 1024;
	else
		priv->pktsize = 512;

	return 0;
}
/**
 * nand_init_data_interface - find the best data interface and timings
 * @chip: The NAND chip
 *
 * Find the best data interface and NAND timings supported by the chip
 * and the driver.
 * First tries to retrieve supported timing modes from ONFI information,
 * and if the NAND chip does not support ONFI, relies on the
 * ->onfi_timing_mode_default specified in the nand_ids table. After this
 * function nand_chip->data_interface is initialized with the best timing mode
 * available.
 *
 * Returns 0 for success or negative error code otherwise.
 */
//static int nand_init_data_interface(struct nand_chip *chip)
//{
//	struct mtd_info *mtd = & mtd_info0; //nand_to_mtd(chip);
//	int modes, mode, ret;
//
//	if (!chip->setup_data_interface)
//		return 0;
//
//	/*
//	 * First try to identify the best timings from ONFI parameters and
//	 * if the NAND does not support ONFI, fallback to the default ONFI
//	 * timing mode.
//	 */
//	modes = onfi_get_async_timing_mode(chip);
//	if (modes == ONFI_TIMING_MODE_UNKNOWN) {
//		if (!chip->onfi_timing_mode_default)
//			return 0;
//
//		modes = NAND_GENMASK(chip->onfi_timing_mode_default, 0);
//	}
//
////	chip->data_interface = kzalloc(sizeof(*chip->data_interface),
////				       GFP_KERNEL);
//
////	chip->data_interface = calloc(1, sizeof(*chip->data_interface));
////	if (!chip->data_interface)
////		return -ENOMEM;
////
////	for (mode = fls(modes) - 1; mode >= 0; mode--) {
////		ret = onfi_init_data_interface(chip, chip->data_interface,
////					       NAND_SDR_IFACE, mode);
////		if (ret)
////			continue;
////
////		/* Pass -1 to only */
////		ret = chip->setup_data_interface(mtd,
////						 NAND_DATA_IFACE_CHECK_ONLY,
////						 chip->data_interface);
////		if (!ret) {
////			chip->onfi_timing_mode_default = mode;
////			break;
////		}
////	}
//
//	return 0;
//}

/**
 * nand_setup_data_interface - Setup the best data interface and timings
 * @chip: The NAND chip
 * @chipnr: Internal die id
 *
 * Find and configure the best data interface and NAND timings supported by
 * the chip and the driver.
 * First tries to retrieve supported timing modes from ONFI information,
 * and if the NAND chip does not support ONFI, relies on the
 * ->onfi_timing_mode_default specified in the nand_ids table.
 *
 * Returns 0 for success or negative error code otherwise.
 */
static int nand_setup_data_interface(struct nand_chip *chip, int chipnr)
{
	struct mtd_info *mtd = & mtd_info0; //nand_to_mtd(chip);
	int ret;

//	if (!chip->setup_data_interface || !chip->data_interface)
//		return 0;

	/*
	 * Ensure the timing mode has been changed on the chip side
	 * before changing timings on the controller side.
	 */
	if (chip->onfi_version) {
		uint8_t tmode_param[ONFI_SUBFEATURE_PARAM_LEN] = {
			chip->onfi_timing_mode_default,
		};

		ret = chip->onfi_set_features(mtd, chip,
				ONFI_FEATURE_ADDR_TIMING_MODE,
				tmode_param);
		if (ret)
			goto err;
	}

//	ret = chip->setup_data_interface(mtd, chipnr, chip->data_interface);
err:
	return ret;
}


/* Set default functions */
static void nand_set_defaults(struct nand_chip *chip, int busw)
{
//	/* check for proper chip_delay setup, set 20us if not */
//	if (!chip->chip_delay)
//		chip->chip_delay = 20;
//
//	/* check, if a user supplied command function given */
//	if (chip->cmdfunc == NULL)
//		chip->cmdfunc = nand_command;
//
//	/* check, if a user supplied wait function given */
//	if (chip->waitfunc == NULL)
//		chip->waitfunc = nand_wait;
//
//	if (!chip->select_chip)
//		chip->select_chip = nand_select_chip;
//
//	/* set for ONFI nand */
//	if (!chip->onfi_set_features)
//		chip->onfi_set_features = nand_onfi_set_features;
//	if (!chip->onfi_get_features)
//		chip->onfi_get_features = nand_onfi_get_features;
//
//	/* If called twice, pointers that depend on busw may need to be reset */
//	if (!chip->read_byte || chip->read_byte == nand_read_byte)
//		chip->read_byte = busw ? nand_read_byte16 : nand_read_byte;
//	if (!chip->read_word)
//		chip->read_word = nand_read_word;
//	if (!chip->block_bad)
//		chip->block_bad = nand_block_bad;
//	if (!chip->block_markbad)
//		chip->block_markbad = nand_default_block_markbad;
//	if (!chip->write_buf || chip->write_buf == nand_write_buf)
//		chip->write_buf = busw ? nand_write_buf16 : nand_write_buf;
//	if (!chip->write_byte || chip->write_byte == nand_write_byte)
//		chip->write_byte = busw ? nand_write_byte16 : nand_write_byte;
//	if (!chip->read_buf || chip->read_buf == nand_read_buf)
//		chip->read_buf = busw ? nand_read_buf16 : nand_read_buf;
//	if (!chip->scan_bbt)
//		chip->scan_bbt = nand_default_bbt;
//
//	if (!chip->controller) {
//		chip->controller = &chip->hwcontrol;
//		spin_lock_init(&chip->controller->lock);
//		init_waitqueue_head(&chip->controller->wq);
//	}

	if (!chip->buf_align)
		chip->buf_align = 1;
}

/**
 * single_erase - [GENERIC] NAND standard block erase command function
 * @mtd: MTD device structure
 * @page: the page address of the block which will be erased
 *
 * Standard erase command for NAND chips. Returns NAND status.
 */
//static int single_erase(struct mtd_info *mtd, int page)
//{
//	struct nand_chip *chip = & nand_chip0; //mtd_to_nand(mtd);
//	unsigned int eraseblock;
//
//	/* Send commands to erase a block */
//	eraseblock = page >> (chip->phys_erase_shift - chip->page_shift);
//
//	return nand_erase_op(chip, eraseblock);
//}

/**
 * nand_reset - Reset and initialize a NAND device
 * @chip: The NAND chip
 * @chipnr: Internal die id
 *
 * Returns 0 for success or negative error code otherwise
 */
//int nand_reset(struct nand_chip *chip, int chipnr)
//{
//	struct mtd_info *mtd = & mtd_info0; //nand_to_mtd(chip);
//	int ret;
//
////	ret = nand_reset_data_interface(chip, chipnr);
////	if (ret)
////		return ret;
//
//	/*
//	 * The CS line has to be released before we can apply the new NAND
//	 * interface settings, hence this weird ->select_chip() dance.
//	 */
//	chip->select_chip(mtd, chipnr);
//	ret = nand_reset_op(chip);
//	chip->select_chip(mtd, -1);
//	if (ret)
//		return ret;
//
//	chip->select_chip(mtd, chipnr);
//	ret = nand_setup_data_interface(chip, chipnr);
//	chip->select_chip(mtd, -1);
//	if (ret)
//		return ret;
//
//	return 0;
//}
/*
 * Get the flash and manufacturer id and lookup if the type is supported.
 */
struct nand_flash_dev *nand_get_flash_type(struct mtd_info *mtd,
						  struct nand_chip *chip,
						  int *maf_id, int *dev_id,
						  struct nand_flash_dev *type)
{
	int busw, ret;
	int maf_idx;
	uint8_t id_data[8];
//
//	/*
//	 * Reset the chip, required by some chips (e.g. Micron MT29FxGxxxxx)
//	 * after power-up.
//	 */
//	ret = nand_reset(chip, 0);
//	if (ret)
//		return ERR_PTR(ret);
//
//	/* Select the device */
//	chip->select_chip(mtd, 0);
//
//	/* Send the command for reading device ID */
//	ret = nand_readid_op(chip, 0, id_data, 2);
//	if (ret)
//		return ERR_PTR(ret);
//
//	/* Read manufacturer and device IDs */
//	*maf_id = id_data[0];
//	*dev_id = id_data[1];
//
//	/*
//	 * Try again to make sure, as some systems the bus-hold or other
//	 * interface concerns can cause random data which looks like a
//	 * possibly credible NAND flash to appear. If the two results do
//	 * not match, ignore the device completely.
//	 */
//
	/* Read entire ID string */
	ret = nand_readid_op(chip, 0, id_data, 8);
	if (ret)
		return ERR_PTR(ret);

	if (id_data[0] != *maf_id || id_data[1] != *dev_id) {
		PRINTF("second ID read did not match %02x,%02x against %02x,%02x\n",
			*maf_id, *dev_id, id_data[0], id_data[1]);
		return ERR_PTR(-ENODEV);
	}
//
//	if (!type)
//		type = nand_flash_ids;
//
////	for (; type->name != NULL; type++) {
////		if (is_full_id_nand(type)) {
////			if (find_full_id_nand(mtd, chip, type, id_data, &busw))
////				goto ident_done;
////		} else if (*dev_id == type->dev_id) {
////			break;
////		}
////	}
//
//	chip->onfi_version = 0;
////	if (!type->name || !type->pagesize) {
////		/* Check if the chip is ONFI compliant */
////		if (nand_flash_detect_onfi(mtd, chip, &busw))
////			goto ident_done;
////
////		/* Check if the chip is JEDEC compliant */
////		if (nand_flash_detect_jedec(mtd, chip, &busw))
////			goto ident_done;
////	}
//
//	if (!type->name)
//		return ERR_PTR(-ENODEV);
//
//	if (!mtd->name)
//		mtd->name = type->name;
//
//	chip->chipsize = (uint64_t)type->chipsize << 20;
//
////	if (!type->pagesize) {
////		/* Decode parameters from extended ID */
////		nand_decode_ext_id(mtd, chip, id_data, &busw);
////	} else {
////		nand_decode_id(mtd, chip, type, id_data, &busw);
////	}
//	/* Get chip options */
//	chip->options |= type->options;
//
//	/*
//	 * Check if chip is not a Samsung device. Do not clear the
//	 * options for chips which do not have an extended id.
//	 */
//	if (*maf_id != NAND_MFR_SAMSUNG && !type->pagesize)
//		chip->options &= ~NAND_SAMSUNG_LP_OPTIONS;
//ident_done:
//
//	/* Try to identify manufacturer */
//	for (maf_idx = 0; nand_manuf_ids[maf_idx].id != 0x0; maf_idx++) {
//		if (nand_manuf_ids[maf_idx].id == *maf_id)
//			break;
//	}
//
//	if (chip->options & NAND_BUSWIDTH_AUTO) {
//		//WARN_ON(chip->options & NAND_BUSWIDTH_16);
//		chip->options |= busw;
//		nand_set_defaults(chip, busw);
//	} else if (busw != (chip->options & NAND_BUSWIDTH_16)) {
//		/*
//		 * Check, if buswidth is correct. Hardware drivers should set
//		 * chip correct!
//		 */
//		PRINTF("device found, Manufacturer ID: 0x%02x, Chip ID: 0x%02x\n",
//			*maf_id, *dev_id);
//		PRINTF("%s %s\n", nand_manuf_ids[maf_idx].name, mtd->name);
//		PRINTF("bus width %d instead %d bit\n",
//			   (chip->options & NAND_BUSWIDTH_16) ? 16 : 8,
//			   busw ? 16 : 8);
//		return ERR_PTR(-EINVAL);
//	}
//
////	nand_decode_bbm_options(mtd, chip, id_data);
//
//	/* Calculate the address shift from the page size */
//	chip->page_shift = ffs(mtd->writesize) - 1;
//	/* Convert chipsize to number of pages per chip -1 */
//	chip->pagemask = (chip->chipsize >> chip->page_shift) - 1;
//
//	chip->bbt_erase_shift = chip->phys_erase_shift =
//		ffs(mtd->erasesize) - 1;
//	if (chip->chipsize & 0xffffffff)
//		chip->chip_shift = ffs((unsigned)chip->chipsize) - 1;
//	else {
//		chip->chip_shift = ffs((unsigned)(chip->chipsize >> 32));
//		chip->chip_shift += 32 - 1;
//	}
//
//	if (chip->chip_shift - chip->page_shift > 16)
//		chip->options |= NAND_ROW_ADDR_3;
//
//	chip->badblockbits = 8;
//	chip->erase = single_erase;
//
//	/* Do not replace user supplied command function! */
////	if (mtd->writesize > 512 && chip->cmdfunc == nand_command)
////		chip->cmdfunc = nand_command_lp;
//
//	PRINTF("device found, Manufacturer ID: 0x%02x, Chip ID: 0x%02x\n",
//		*maf_id, *dev_id);
//
//#ifdef CONFIG_SYS_NAND_ONFI_DETECTION
//	if (chip->onfi_version)
//		PRINTF("%s %s\n", nand_manuf_ids[maf_idx].name,
//				chip->onfi_params.model);
//	else if (chip->jedec_version)
//		PRINTF("%s %s\n", nand_manuf_ids[maf_idx].name,
//				chip->jedec_params.model);
//	else
//		PRINTF("%s %s\n", nand_manuf_ids[maf_idx].name,
//				type->name);
//#else
//	if (chip->jedec_version)
//		PRINTF("%s %s\n", nand_manuf_ids[maf_idx].name,
//				chip->jedec_params.model);
//	else
//		PRINTF("%s %s\n", nand_manuf_ids[maf_idx].name,
//				type->name);
//
//	PRINTF("%s %s\n", nand_manuf_ids[maf_idx].name,
//		type->name);
//#endif
//
	PRINTF("%d MiB, %s, erase size: %d KiB, page size: %d, OOB size: %d\n",
		(int)(chip->chipsize >> 20), nand_is_slc(chip) ? "SLC" : "MLC",
				(int) (mtd->erasesize >> 10), (int) mtd->writesize, (int) mtd->oobsize);
	return type;
}

//int nand_scan_ident(struct mtd_info *mtd, int maxchips,
//		    struct nand_flash_dev *table)
//{
//	int i, nand_maf_id, nand_dev_id;
//	struct nand_chip *chip = & nand_chip0; //mtd_to_nand(mtd);
//	struct nand_flash_dev *type;
//	int ret = 0;
//
////	if (chip->flash_node) {
////		ret = nand_dt_init(mtd, chip, chip->flash_node);
////		if (ret)
////			return ret;
////	}
//
////	/* Set the default functions */
////	nand_set_defaults(chip, chip->options & NAND_BUSWIDTH_16);
////
////	/* Read the flash type */
////	type = nand_get_flash_type(mtd, chip, &nand_maf_id,
////				   &nand_dev_id, table);
//
////	if (IS_ERR(type)) {
////		if (!(chip->options & NAND_SCAN_SILENT_NODEV))
////			PRINTF("No NAND device found\n");
////		chip->select_chip(mtd, -1);
////		return PTR_ERR(type);
////	}
//
////	/* Initialize the ->data_interface field. */
////	ret = nand_init_data_interface(chip);
////	if (ret)
////		return ret;
//
//	/*
//	 * Setup the data interface correctly on the chip and controller side.
//	 * This explicit call to nand_setup_data_interface() is only required
//	 * for the first die, because nand_reset() has been called before
//	 * ->data_interface and ->default_onfi_timing_mode were set.
//	 * For the other dies, nand_reset() will automatically switch to the
//	 * best mode for us.
//	 */
//	ret = nand_setup_data_interface(chip, 0);
//	if (ret)
//		return ret;
//
//	chip->select_chip(mtd, -1);
//
//	/* Check for a chip array */
//	for (i = 1; i < maxchips; i++) {
//		uint8_t id[2];
//
//		/* See comment in nand_get_flash_type for reset */
//		nand_reset(chip, i);
//
//		chip->select_chip(mtd, i);
//		/* Send the command for reading device ID */
//		nand_readid_op(chip, 0, id, sizeof(id));
//
//		/* Read manufacturer and device IDs */
//		if (nand_maf_id != id[0] || nand_dev_id != id[1]) {
//			chip->select_chip(mtd, -1);
//			break;
//		}
//		chip->select_chip(mtd, -1);
//	}
//
//#if 1//def DEBUG
//	if (i > 1)
//		PRINTF("%d chips detected\n", i);
//#endif
//
//	/* Store the number of chips and calc total size for mtd */
//	chip->numchips = i;
//	mtd->size = i * chip->chipsize;
//
//	return 0;
//}

static int mcom02_nand_probe(void)
{
	struct mcom02_nand_priv *priv = & nand_priv0; //dev_get_priv(dev);
	struct nand_chip *nand = & nand_chip0;
	struct mtd_info *mtd;
	int ret;
	mtd = & mtd_info0;//nand_to_mtd(nand);

	//priv->regs = NANDMPORT;//(void *)devfdt_get_addr(dev);
	//nand_set_controller_data(nand, priv);
	//nand->priv = priv;

	/* Set the driver entry points for MTD */
	nand->cmdfunc = mcom02_nand_cmdfunc;
	nand->select_chip = mcom02_nand_select_chip;
	nand->read_byte = mcom02_nand_read_byte;
	nand->waitfunc = mcom02_nand_device_ready;
	nand->chip_delay = 30;

	/* Buffer read/write routines */
	nand->read_buf = mcom02_nand_read_buf;
	nand->write_buf = mcom02_nand_write_buf;
	nand->options = NAND_BUSWIDTH_AUTO
			| NAND_NO_SUBPAGE_WRITE
			| NAND_USE_BOUNCE_BUFFER;
	nand->bbt_options = NAND_BBT_USE_FLASH;

	priv->rdintrmask = 0;

	priv->pktsize = 4;

	mtd->writebufsize = 2048;
	mtd->writesize = 2048;
	mtd->erasesize = 2048;
	mtd->oobsize = 224;

//	ret = nand_scan_ident(mtd, 1, NULL);
//	if (ret)
//		return ret;

	if (mtd->writesize > SZ_8K) {
		PRINTF("Page size too big for controller\n");
		return -EINVAL;
	}

	if (!nand->onfi_params.addr_cycles) {
		/* Good estimate in case ONFI ident doesn't work */
		priv->raddr_cycles = 3;
		priv->caddr_cycles = 2;
	} else {
		priv->raddr_cycles = nand->onfi_params.addr_cycles & 0xF;
		priv->caddr_cycles =
			(nand->onfi_params.addr_cycles >> 4) & 0xF;
	}

	if (mcom02_nand_ecc_init(mtd))
		return -ENXIO;

//	ret = nand_scan_tail(mtd);
//	if (ret)
//		return ret;

//	ret = nand_register(0, mtd);
//	if (ret)
//		return ret;

	return 0;
}
//
//static const struct udevice_id mcom02_nand_dt_ids[] = {
//	{ .compatible = "arasan,nfc-v2p99" },
//	{ },
//};
//
//U_BOOT_DRIVER(mcom02_nand) = {
//	.name = "mcom02-nand",
//	.id = UCLASS_MTD,
//	.of_match = mcom02_nand_dt_ids,
//	.probe = mcom02_nand_probe,
//	.priv_auto_alloc_size = sizeof(struct mcom02_nand_priv),
//};

//void board_nand_init(void)
//{
//	struct udevice *dev;
//	int ret;
//
//	ret = uclass_get_device_by_driver(UCLASS_MTD,
//					  DM_GET_DRIVER(mcom02_nand), &dev);
//	if (ret && ret != -ENODEV)
//		PRINTF("Failed to initialize %s, error %d\n", dev->name, ret);
//}

// MT29F32G08AFACBWP-ITZ
#define SYSTEM_BUS_CLK 144
#define NAND_BUS_FREQ  96

void vm41nandtest(void)
{
	int ec;
	unsigned i;

	CMCTR->GATE_SYS_CTR &= ~(1u << 21);
	CMCTR->DIV_NFC_CTR = SYSTEM_BUS_CLK / NAND_BUS_FREQ;
	//setSystemFreq(SYSTEM_BUS_CLK);
	CMCTR->GATE_SYS_CTR |= (1u << 21);
	PRINTF("NAND test commands: z, i, n, p, e, r, w, L\n");
	unsigned sector = 0;
	static uint8_t buff [2048];
	memset(buff, 0, sizeof buff);
	mcom02_nand_probe();
	mcom02_nand_cmdfunc(&mtd_info0, NAND_CMD_RESET, 0, 0);
	mcom02_nand_device_ready(&mtd_info0, &nand_chip0);
	PRINTF("NAND test device ready\n");
	mcom02_nand_cmdfunc(&mtd_info0, NAND_CMD_READID, 0x00, 0);
	printhex(0x00000, nand_priv0.buf, 8);
	mcom02_nand_cmdfunc(&mtd_info0, NAND_CMD_READID, ONFI_ID_ADDR, 0);
	printhex(ONFI_ID_ADDR, nand_priv0.buf, 8);
	PRINTF("NAND test device read id passed\n");

	for (;;)
	{
		char c;
		uint_fast16_t kbch;
		uint_fast8_t kbready;
		processmessages(& kbch, & kbready);
		if (dbg_getchar(& c))
		{
			switch (c)
			{
			case 'z':
				PRINTF("Reset\n");
				//vm14nand_reset();
				mcom02_nand_cmdfunc(&mtd_info0, NAND_CMD_RESET, 0, 0);
				mcom02_nand_device_ready(&mtd_info0, &nand_chip0);
				break;
			case 'i':
				PRINTF("Read IDs\n");
//				vm14nand_readid(0x00);
//				vm14nand_readid(0x20);
//				vm14nand_readstatus();
				mcom02_nand_cmdfunc(&mtd_info0, NAND_CMD_READID, 0x00, 0);
				printhex(0x00000, nand_priv0.buf, 8);
				mcom02_nand_cmdfunc(&mtd_info0, NAND_CMD_READID, ONFI_ID_ADDR, 0);
				printhex(ONFI_ID_ADDR, nand_priv0.buf, 8);
				break;
			case 'n':
				sector += 1;
				PRINTF("sector = %u\n", sector);
				break;
			case 'p':
				if (sector)
					sector -= 1;
				PRINTF("sector = %u\n", sector);
				break;
			case 'e':
				PRINTF("Erase block %u\n", sector);
				mcom02_nand_cmdfunc(&mtd_info0, NAND_CMD_ERASE1, 0, sector);
				mcom02_nand_device_ready(&mtd_info0, &nand_chip0);
				break;
			case 'r':
				PRINTF("Read sector %u\n", sector);
				//memset(buff, 0xDE, sizeof buff);
				mcom02_nand_cmdfunc(&mtd_info0, NAND_CMD_READ0, 0, sector);
				mcom02_nand_read_buf(&mtd_info0, buff, sizeof buff);
				printhex(0, buff, sizeof buff / 1);
				break;
			case 'w':
				PRINTF("Write sector %u\n", sector);
				memset(buff, 0xE5, sizeof buff);
				for (i = 0; i < sizeof buff / sizeof buff [0]; i += 32)
				{
					local_snprintf_P((char *) buff + i, 32, "s=%u o=%04x ", sector, (unsigned) (buff + i + 8));
				}
				mcom02_nand_cmdfunc(&mtd_info0, NAND_CMD_SEQIN, 0, sector);
				mcom02_nand_write_buf(&mtd_info0, buff, sizeof buff);
				mcom02_nand_device_ready(&mtd_info0, &nand_chip0);
				break;
			case 'W':
				PRINTF("Write sector %u (data set 2)\n", sector);
				memset(buff, 0xFF, sizeof buff);
				for (i = 0; i < sizeof buff / sizeof buff [0]; i += 32)
				{
					buff [i + 0] = sector >> 8;
					buff [i + 1] = sector;
					buff [i + 2] = 'G';
					buff [i + 3] = 'Z';
					buff [i + 4] = ' ';
					buff [i + 5] = 'v';
					buff [i + 6] = 'e';
					buff [i + 7] = 'i';
					buff [i + 8] = 'f';
					buff [i + 8] = 'y';
				}
				mcom02_nand_cmdfunc(&mtd_info0, NAND_CMD_SEQIN, 0, sector);
				mcom02_nand_write_buf(&mtd_info0, buff, sizeof buff);
				mcom02_nand_device_ready(&mtd_info0, &nand_chip0);
				break;
			default:
				PRINTF("Undefined command %02X\n", c);
				break;
			}
		}
	}
}

#endif /* CPUSTYLE_VM14 */

#if CPUSTYLE_ALLWINNER && 0


static void de_dump(void)
{
	memset(DE_BASE, 0xFF, 4 * 1024 * 1024);
	int skip = 0;
	static uint8_t pattern [256];
	unsigned offs;
	for (offs = 0; offs < 4 * 1024 * 1024; offs += 4096)
	{
		* (volatile uint32_t *) (DE_BASE + offs) |= 1;
		if (memcmp(pattern, (void *) (DE_BASE + offs), sizeof pattern) == 0)
		{
			++ skip;
			PRINTF(".");
			continue;
		}
		if (skip)
		{
			skip = 0;
			PRINTF("\n");
		}
		printhex32(DE_BASE + offs, (void *) (DE_BASE + offs), 256);
		PRINTF("---\n");
	}
}

int wrongbase(uintptr_t base)
{
//	if (base >= 0x05102800 && base < 0x05200000)
//		return 1;
//	if (base < 0x01300000)
//		return 0;
//	if (base < (0x01300000 + 0xA0000))
//		return 1;
//	if (base >= (0x01300000 + 0))
//		return 1;
//	PRINTF("pr=0x%08X ", base);
	return 0;
}

static DE_BLD_TypeDef * blds [32];
unsigned nblds = 0;
void testde(void)
{
//	{
//		uint32_t volatile * const p = (uint32_t volatile *) DE_VI1_BASE;
//		uint32_t v = * p;
//		* p = ~ 0u;
//		PRINTF("VI SIGN=%08X\n", * p);
//		* p = v;
//	}
//	{
//		uint32_t volatile * const p = (uint32_t volatile *) DE_UI1_BASE;
//		uint32_t v = * p;
//		* p = ~ 0u;
//		PRINTF("UI SIGN=%08X\n", * p);
//		* p = v;
//	}
//	{
//		uint32_t volatile * const p = (uint32_t volatile *) (DE_BLD_BASE + 0x80);
//		//uint32_t v = * p;
//		//* p = ~ 0u;
//		PRINTF("BLD SIGN=%08X\n", * p);
//		//* p = v;
//	}
	PRINTF("UI1:\n");
	memset(DE_UI1_BASE, 255, 256);
	printhex32(DE_UI1_BASE, DE_UI1_BASE, 256);
//	PRINTF("UI2:\n");
//	memset(DE_UI2_BASE, 255, 256);
//	printhex32(DE_UI2_BASE, DE_UI2_BASE, 256);
//	PRINTF("UI3:\n");
//	memset(DE_UI3_BASE, 255, 256);
//	printhex32(DE_UI3_BASE, DE_UI3_BASE, 256);
	PRINTF("VI1:\n");
	memset(DE_VI1_BASE, 255, 256);
	printhex32(DE_VI1_BASE, DE_VI1_BASE, 256);

	//printhex32(DE_BASE, DE_BASE, 4 * 1024 * 1024);
//	const uint32_t signUI = 0xFFB31F17;	// UI signature
//	const uint32_t signVI = 0xFFB39F17;	// VI signature
	const uint32_t signUI = 0xFFB3BF1F;	// UI signature 113
	const uint32_t signVI = 0xFFB3BF1F;	// VI signature 113
	uintptr_t base = DE_BASE;
	uintptr_t top = base + 4 * 1024 * 1024;
	unsigned bld = 0;
	for (;base < top; base += 4096)
	{
		if (wrongbase(base))
			continue;
		uint32_t volatile * const p = (uint32_t volatile *) base;
//		uint32_t v = * p;
//		* p = ~ 0u;
//		uint32_t dprobe = * p;
//		* p = v;
//		TP();
		if (* (uint32_t volatile *) (base + 0x80) == 0x00543210)
		{
			++ bld;
			PRINTF("found BLD6 at 0x%08X\n", (unsigned) base);
			((DE_BLD_TypeDef *) base)->BKCOLOR = bld;
			blds [nblds ++] = ((DE_BLD_TypeDef *) base);
		}
		else if (* (uint32_t volatile *) (base + 0x80) == 0x00003210)
		{
			++ bld;
			PRINTF("found BLD4 at 0x%08X\n", (unsigned) base);
			((DE_BLD_TypeDef *) base)->BKCOLOR = bld;
			blds [nblds ++] = ((DE_BLD_TypeDef *) base);
		}
//		if (bld == 4)
//			break;
//		else if (signUI == dprobe)
//		{
//			PRINTF("found UI at 0x%08X\n", (unsigned) base);
//		}
//		else if (signVI == dprobe)
//		{
//			PRINTF("found VI at 0x%08X\n", (unsigned) base);
//		}
//		else if ((dprobe & 0xFFF00000) == 0xFFB00000)
//		{
//			PRINTF("found XX (%08X) at 0x%08X\n", (unsigned) dprobe, (unsigned) base);
//		}
//		TP();
	}
	unsigned i;
	for (i = 0; i < nblds; ++ i)
	{
		PRINTF("naskr bld%u (%p)= %08X\n", i, blds [i], blds [i]->BKCOLOR);
	}
}
#endif

#if 0
// Benewake TFmini Plus LIDAR
static void lidar_parse(unsigned char c)
{
	static unsigned pos = 0;
	static unsigned cks;
	static unsigned char buff [6];

	switch (pos)
	{
	case 0:
		cks = c;
		if (c == 0x59)
			pos = 1;
		break;
	case 1:
		if (c == 0x59)
		{
			pos = 2;
			cks += c;
		}
		else
		{
			// 2-nd signature byte wrong
			pos = 0;
		}
		break;
	default:
		// 2..7
		buff [pos ++ - 2] = c;
		cks += c;
		break;
	case 8:
		if (c == (cks & 0xFF))
		{
			// Use valid data
			unsigned Dist = buff [0] + buff [1] * 256;	// Distanc, cm
			unsigned Strength = buff [2] + buff [3] * 256;	// When the signal strength is lower than 100 or equal to 65535, the detection is unreliable
			unsigned Temp = buff [4] + buff [5] * 256;	//Temp(Temperature): Represents the chip temperature of TFmini Plus. Degree centigrade = Temp / 8 -256
			int Temperature = (int) (Temp / 8) - 256;
			PRINTF("Dist=%u cm, Streingth=%u, Temperature=%+d C\n", Dist, Strength, Temperature);
			//printhex(0, buff, ARRAY_SIZE(buff));
		}
		pos = 0;
		break;
	}
}
#endif

// p15, 1, <Rt>, c15, c3, 0; -> __get_CP64(15, 1, result, 15);  Read CBAR into Rt
// p15, 1, <Rt>, <Rt2>, c15; -> __get_CP64(15, 1, result, 15);

#if WITHLVGL

#include "lvgl.h"
#include "../demos/lv_demos.h"
#include "layouts/grid/lv_grid.h"
//#include "../demos/vector_graphic/lv_demo_vector_graphic.h"
//#include "src/lvgl_gui/styles.h"

/**
 * Draw a line to the canvas
 */
static void lv_example_canvas_7(lv_obj_t * parent)
{
	enum { CANVAS_WIDTH = 300, CANVAS_HEIGHT = 300 };


    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.radius = 10;
    rect_dsc.bg_opa = LV_OPA_COVER;
    rect_dsc.bg_grad.dir = LV_GRAD_DIR_VER;
    rect_dsc.bg_grad.stops[0].color = lv_palette_main(LV_PALETTE_RED);
    rect_dsc.bg_grad.stops[0].opa = LV_OPA_100;
    rect_dsc.bg_grad.stops[1].color = lv_palette_main(LV_PALETTE_BLUE);
    rect_dsc.bg_grad.stops[1].opa = LV_OPA_50;
    rect_dsc.border_width = 2;
    rect_dsc.border_opa = LV_OPA_90;
    rect_dsc.border_color = lv_color_white();
    rect_dsc.shadow_width = 5;
    rect_dsc.shadow_offset_x = 5;
    rect_dsc.shadow_offset_y = 5;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_palette_main(LV_PALETTE_ORANGE);
    label_dsc.text = "Some text on text canvas";
    /*Create a buffer for the canvas*/
//    LV_DRAW_BUF_DEFINE_STATIC(draw_buf_16bpp, CANVAS_WIDTH, CANVAS_HEIGHT, LV_COLOR_FORMAT_RGB565);
//    LV_DRAW_BUF_INIT_STATIC(draw_buf_16bpp);
	static RAMFRAMEBUFF __ALIGNED(64) uint8_t db1 [GXSIZE(CANVAS_WIDTH, CANVAS_HEIGHT) * 2];
	static lv_draw_buf_t draw_buf_16bpp;
    lv_draw_buf_init(& draw_buf_16bpp, CANVAS_WIDTH, CANVAS_HEIGHT, LV_COLOR_FORMAT_RGB565, 2 * GXADJ(CANVAS_WIDTH), db1, sizeof (db1)); \
    lv_draw_buf_set_flag(& draw_buf_16bpp, LV_IMAGE_FLAGS_MODIFIABLE);

    {
        lv_obj_t * canvas1 = lv_canvas_create(parent);

        lv_canvas_set_draw_buf(canvas1, & draw_buf_16bpp);
        lv_obj_center(canvas1);
        lv_canvas_fill_bg(canvas1, lv_palette_lighten(LV_PALETTE_GREY, 3), LV_OPA_COVER);

        lv_layer_t layer;
        lv_canvas_init_layer(canvas1, &layer);

        lv_area_t coords_rect = {30, 20, 100, 70};
        lv_draw_rect(&layer, &rect_dsc, &coords_rect);

        lv_area_t coords_text = {40, 80, 100, 120};
        lv_draw_label(&layer, &label_dsc, &coords_text);

        lv_canvas_finish_layer(canvas1, &layer);

        //lv_obj_delete(canvas1);
		//lv_obj_set_flag(canvas1, LV_OBJ_FLAG_HIDDEN, 1);
     }
    /*Test the rotation. It requires another buffer where the original image is stored.
     *So use previous canvas as image and rotate it to the new canvas*/
//    LV_DRAW_BUF_DEFINE_STATIC(draw_buf_32bpp, CANVAS_WIDTH, CANVAS_HEIGHT, LV_COLOR_FORMAT_ARGB8888);
//    LV_DRAW_BUF_INIT_STATIC(draw_buf_32bpp);
	static RAMFRAMEBUFF __ALIGNED(64) uint8_t db2 [GXSIZE(CANVAS_WIDTH, CANVAS_HEIGHT) * 4];
	static lv_draw_buf_t draw_buf_32bpp;
    lv_draw_buf_init(& draw_buf_32bpp, CANVAS_WIDTH, CANVAS_HEIGHT, LV_COLOR_FORMAT_ARGB8888, 4 * GXADJ(CANVAS_WIDTH), db2, sizeof (db2)); \
    lv_draw_buf_set_flag(& draw_buf_32bpp, LV_IMAGE_FLAGS_MODIFIABLE);
    {
        /*Create a canvas2 and initialize its palette*/
        lv_obj_t * canvas2 = lv_canvas_create(parent);

        lv_canvas_set_draw_buf(canvas2, &draw_buf_32bpp);
        lv_canvas_fill_bg(canvas2, lv_color_hex3(0xccc), LV_OPA_COVER);
        lv_obj_center(canvas2);

        lv_canvas_fill_bg(canvas2, lv_palette_lighten(LV_PALETTE_GREY, 1), LV_OPA_COVER);

        lv_layer_t layer;
        lv_canvas_init_layer(canvas2, &layer);
        lv_image_dsc_t img;
        lv_draw_buf_to_image(&draw_buf_16bpp, &img);	// копируктся ранее нарисованный буфер
        lv_draw_image_dsc_t img_dsc;
        lv_draw_image_dsc_init(&img_dsc);
        img_dsc.rotation = 120;
        img_dsc.src = &img;
        img_dsc.pivot.x = CANVAS_WIDTH / 2;
        img_dsc.pivot.y = CANVAS_HEIGHT / 2;

        lv_area_t coords_img = {0, 0, CANVAS_WIDTH - 1, CANVAS_HEIGHT - 1};
        lv_draw_image(&layer, &img_dsc, &coords_img);

        lv_canvas_finish_layer(canvas2, &layer);
        //lv_obj_delete(canvas2);
		//lv_obj_set_flag(canvas2, LV_OBJ_FLAG_HIDDEN, 1);

    }
}
#endif /* WITHLVGL */

#if 0

// Очередь символов для передачи в канал обмена
static u8queue_t txq;
// Очередь принятых симвоов из канала обменна
static u8queue_t rxq;

// передача символа в канал. Ожидание, если очередь заполнена
static int nmeaX_putc(int c)
{
	IRQL_t oldIrql;
	uint_fast8_t f;

	do {
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		f = uint8_queue_put(& txq, c);
		hardware_uart4_enabletx(1);
		LowerIrql(oldIrql);
	} while (! f);
	return c;
}

// Передача в канал указанного массива. Ожидание, если очередь заполнена
static void uartX_write(const uint8_t * buff, size_t n)
{
	while (n --)
	{
		const uint8_t c = * buff ++;
		nmeaX_putc(c);
	}
}

static void uartX_format(const char * format, ...)
{
	char b [256];
	int n, i;
	va_list	ap;
	va_start(ap, format);

	n = vsnprintf(b, sizeof b / sizeof b [0], format, ap);

	for (i = 0; i < n; ++ i)
		nmeaX_putc(b [i]);

	va_end(ap);
}

// callback по принятому символу. сохранить в очередь для обработки в user level
void user_uart4_onrxchar(uint_fast8_t c)
{
	IRQL_t oldIrql;

	RiseIrql(IRQL_SYSTEM, & oldIrql);
	uint8_queue_put(& rxq, c);
	LowerIrql(oldIrql);
}

// callback по готовности последовательного порта к пердаче
void user_uart4_ontxchar(void * ctx)
{
	uint_fast8_t c;
	if (uint8_queue_get(& txq, & c))
	{
		hardware_uart4_tx(ctx, c);
		if (uint8_queue_empty(& txq))
			hardware_uart4_enabletx(0);
	}
	else
	{
		hardware_uart4_enabletx(0);
	}
}


/////////////
///
///
///

static uint_fast8_t crc8update(const uint_fast8_t v, uint8_t crc)
{

	static const uint8_t crc8tab [256] =
	{
	    0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54, 0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
	    0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06, 0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
	    0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0, 0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
	    0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2, 0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
	    0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9, 0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
	    0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B, 0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
	    0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D, 0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
	    0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F, 0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
	    0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB, 0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
	    0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9, 0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
	    0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F, 0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
	    0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D, 0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
	    0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26, 0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
	    0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74, 0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
	    0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82, 0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
	    0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0, 0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9
	};

	return crc8tab [(crc ^ v) & 0xFF];
}

enum csrf_states
{
	CSRF_WAIT_SYNC, ///< CSRF_WAIT_SYNC
	CSRF_FRAME_LEN, ///< CSRF_FRAME_LEN
	CSRF_FRAME_TYPE,///< CSRF_FRAME_TYPE
	CSRF_ORIG_ADDR,
	CSRF_DEST_ADDR,
	CSRF_PAYLOAD,   ///< CSRF_PAYLOAD
	CSRF_CRC        ///< CSRF_CRC

};

static enum csrf_states state = CSRF_WAIT_SYNC;

static uint_fast8_t crsf_type;
static uint_fast8_t crsf_framelen;
static uint_fast8_t crsf_crc;
static uint_fast8_t crsf_dest_addr;
static uint_fast8_t crsf_orig_addr;
static uint_fast8_t crsf_lenbyte;	// необработанный байт длинны
static uint_fast8_t crsf_broadcast;
static uint8_t crsf_payload [255];
static unsigned crsf_payload_ix;

static void crsf_parser(const uint_fast8_t c)
{
	//PRINTF("%02X ", c);
	crsf_crc = crc8update(c, crsf_crc);
	switch (state)
	{
	case CSRF_WAIT_SYNC:
		if (c == 0xC8)
		{
			state = CSRF_FRAME_LEN;
		}
		break;

	case CSRF_FRAME_LEN:
		if (c >= 2 && c <= 0x62)
		{
			// Broadcast Frame: Type + Payload + CRC,
			// Extended header frame: Type + Destination address + Origin address + Payload + CRC
//			crsf_framelen = c - 2;
			crsf_lenbyte = c;
			state = CSRF_FRAME_TYPE;
			crsf_crc = 0x00;	// initial CRC accum
		}
		else
		{
			PRINTF("drop (bad packet length) ");
			state = CSRF_WAIT_SYNC;
		}
		break;

	case CSRF_FRAME_TYPE:
		crsf_type = c;
		crsf_payload_ix = 0;
		if (crsf_type < 0x27)
		{
			crsf_framelen = crsf_lenbyte - 2;
			crsf_broadcast = 1;
			state = CSRF_PAYLOAD;
		}
		else
		{
			crsf_framelen = crsf_lenbyte - 4;
			crsf_broadcast = 0;
			state = CSRF_DEST_ADDR;
		}
		break;

	case CSRF_DEST_ADDR:
		crsf_dest_addr = c;
		state = CSRF_ORIG_ADDR;
		break;

	case CSRF_ORIG_ADDR:
		crsf_orig_addr = c;
		state = CSRF_PAYLOAD;
		break;

	case CSRF_PAYLOAD:
		if (crsf_payload_ix < ARRAY_SIZE(crsf_payload))
		{
			crsf_payload [crsf_payload_ix] = c;
			if (++ crsf_payload_ix >= crsf_framelen)
				state = CSRF_CRC;
		}
		else
		{
			PRINTF("drop (payload overflow) ");
			state = CSRF_WAIT_SYNC;
		}
		break;

	case CSRF_CRC:
		state = CSRF_WAIT_SYNC;
		if (crsf_crc == 0)
		{
			//PRINTF("bc=%d lb=%02X ty=%02X ", crsf_broadcast, crsf_lenbyte, crsf_type);
			dbg_putchar(crsf_broadcast ? '*' : '-');
			//printhex(0, crsf_payload, crsf_framelen);
		}
		else
		{
			PRINTF("[bc=%d lb=%02X  ty=%02X ", crsf_broadcast, crsf_lenbyte, crsf_type);
			PRINTF("drop (bad CRC)] ");
			//printhex(0, crsf_payload, crsf_framelen);
		}
		break;

	default:
		break;

	}

}

static void csrftest(void)
{
	static uint8_t txb [512];
	uint8_queue_init(& txq, txb, ARRAY_SIZE(txb));
	static uint8_t rxb [512];
	uint8_queue_init(& rxq, rxb, ARRAY_SIZE(rxb));

	const uint_fast32_t baudrate = 420000;
	hardware_uart4_initialize(0, baudrate, 8, 0, 0);
	hardware_uart4_set_speed(baudrate);



	hardware_uart4_enablerx(1);
	hardware_uart4_enabletx(0);

	PRINTF("Test device\n");
	for (;;)
	{
		/* Обеспечение работы USER MODE DPC */
		uint_fast16_t kbch;
		uint_fast8_t kbready;
		processmessages(& kbch, & kbready);

		IRQL_t oldIrql;
		uint_fast8_t f;
		uint_fast8_t c;
		/* Отладочные функции */
		if (kbready)
			PRINTF("bkbch=%02x\n", kbch);

		RiseIrql(IRQL_SYSTEM, & oldIrql);
		f = uint8_queue_get(& rxq, & c);
		LowerIrql(oldIrql);
		if (f)
		{
			crsf_parser(c);
		}
	}
}

#endif

#if 0

static uint8_t data [6];
static volatile uint_fast32_t pd;
static volatile uint_fast32_t pv;
static volatile uint_fast32_t zpd;
static volatile uint_fast32_t crc;

// callback по принятому символу. сохранить в очередь для обработки в user level
void user_uart1_onrxchar(uint_fast8_t c)
{
	static int state = 0;
	if (c & 0x80)
		state = 0;

	data [state ++] = c;
	if (state >= 6)
	{
		state = 0;
		pv = (data [0] >> 6) & 0x01;
		zpd = (data [0] >> 5) & 0x01;
		pd =
			(data [0] & 0x07) * (UINT32_C(1) << 19) +
			(data [1] & 0x7F) * (UINT32_C(1) << 12) +
			(data [2] & 0x7F) * (UINT32_C(1) << 5) +
			((data [3] >> 2) & 0x3F) * (UINT32_C(1) << 0) +
			0;
		crc =
			(data [3] & 0x03) * (UINT32_C(1) << 14) +
			(data [4] & 0x7F) * (UINT32_C(1) << 7) +
			(data [5] & 0x7F) * (UINT32_C(1) << 0) +
			0;
	}
}

// callback по готовности последовательного порта к пердаче
void user_uart1_ontxchar(void * ctx)
{
#if WITHUART1HW
	hardware_uart1_enabletx(0);
#endif /* WITHUART1HW */
}

//	C0 03 2C 13 62 0B
//	C0 03 2C 13 62 0B
//	C0 03 2C 13 62 0B
//	C0 03 2C 13 62 0B

static void enctest(void)
{
	const uint_fast32_t baudrate = 230400;
	hardware_uart1_initialize(0, baudrate, 8, 0, 0);
	hardware_uart1_set_speed(baudrate);



	hardware_uart1_enablerx(1);
	hardware_uart1_enabletx(0);

	PRINTF("Test device\n");
	for (;;)
	{
		/* Обеспечение работы USER MODE DPC */
		uint_fast16_t kbch;
		uint_fast8_t kbready;
		processmessages(& kbch, & kbready);

		IRQL_t oldIrql;
		uint_fast8_t f;
		uint_fast8_t c;
		/* Отладочные функции */
		if (kbready)
			PRINTF("bkbch=%02x\n", kbch);

		unsigned angle100 = 36000 * (pd & 0x3FFF) / 16384;
		PRINTF("pv=%u zpd=%u pd=%04X angle=%3u.%02u\n", (unsigned) pv, (unsigned) zpd, (unsigned) pd, angle100 / 100, angle100 % 100);
//		RiseIrql(IRQL_SYSTEM, & oldIrql);
//		f = uint8_queue_get(& rxq, & c);
//		LowerIrql(oldIrql);
//		if (f)
//		{
//			PRINTF("%02X ", c & 0xFF);
//			//crsf_parser(c);
//		}
	}
}

#endif

void hightests(void)
{
#if LCDMODE_LTDC
	gxdrawb_t dbv;
	gxdrawb_initialize(& dbv, colmain_fb_draw(), DIM_X, DIM_Y);
	gxstyle_t dbstylev;
	gxstyle_initialize(& dbstylev);
#endif /* LCDMODE_LTDC */

#if WITHLTDCHW && LCDMODE_LTDC
	{
		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
		colpip_fillrect(& dbv, 0, 0, DIM_X, DIM_Y, display2_getbgcolor());
		display_text(& dbv, 0, 0, "Start...", 10, 16, & dbstylev);
		colmain_nextfb();
	}
#endif /* WITHLTDCHW && LCDMODE_LTDC */
#if 0 && WITHLTDCHW && LCDMODE_LTDC
	{
		// Font test
		// Fonts test
		// Font drawing tests
		gxdrawb_t dbv;
		gxdrawb_initialize(& dbv, colmain_fb_draw(), DIM_X, DIM_Y);
		colpip_fillrect(& dbv, 0, 0, DIM_X, DIM_Y, display2_getbgcolor());
		//display_text(& dbv, 0, 0, "Start2...", 10, 16, & dbstylev);
		static const char msg [] = "HELLO! test: 0123456789+-.#";
		//static const char msg [] = "HELLO! test";
		static const unifont_t * const fonts [] =
		{
			& unifont_small,
			& unifont_small2,
			& unifont_small3,	// шрифт, используемый при отриосовке надписей на шкале
			//& unifont_small_x2,	// vtty_x2.c
			//& unifont_Tahoma_Regular_88x77,	// CP Font Generator support
//			& unifont_roboto32,	// aptech
//			& unifont_helvNeueTh70,	// aptech
			& unifont_FreeMono9pt7b,
			& unifont_FreeMono12pt7b,
			& unifont_FreeMono18pt7b,
			& unifont_FreeMono24pt7b,
			& unifont_FreeSans12pt7b,
			& unifont_gothic_11x13,
			& unifont_gothic_12x16p,
//			& unifont_msgothic_10x13_prop,
//			& unifont_msgothic_11x13_mono,
//			& unifont_msgothic_13x16_prop,
//			& unifont_msgothic_15x17_prop,
//			& unifont_msgothic_15x17_mono,
			& unifont_big,
			& unifont_half,
		};
		unsigned row;
		uint_fast16_t xpix = 0;
		uint_fast16_t ypix = 0;
		for (row = 0; row < ARRAY_SIZE(fonts); ++ row)
		{
			uint_fast16_t h;
			const unifont_t * const font = fonts [row];
			uint_fast16_t w = unifont_textsize(font, msg, TEZXTSZIE_AUTO, & h);
			PRINTF("%s: w/h=%u/%u\n", font->label, w, h);
			colpip_rectangle(& dbv, xpix, ypix, w, h, row % 2 ? COLOR_DARKCYAN : COLOR_GRAY, 0, 0);
			unifont_text(& dbv, xpix, ypix, font, msg, TEZXTSZIE_AUTO, COLOR_BLACK);
			ypix += h;
		}
		colmain_nextfb();
		for (;;)
	    	testsloopprocessing();
	}
#endif
#if 0
	{
		PRINTF("Malloc test\n");
		void * p = malloc(100);
		p = realloc(p, 200);
		free(p);
		PRINTF("Malloc test done\n");
	}
#endif
#if 0
	{
		enctest();
	}
#endif
#if 0 && WITHLVGL
	{
		lv_example_canvas_7(lv_screen_active());
		for (;;)
			lv_timer_handler();
	}
#endif
#if 0
	{
		csrftest();
	}
#endif
#if 0 && WITHLVGL && LV_BUILD_DEMOS
	{
		//lv_demo_vector_graphic_not_buffered();

	//	//char s1 [] = "stress";
	//	char s1 [] = "widgets";
	//	char * demo [] = { s1, };
	//    lv_demos_create(demo, 1);
	//
	    lv_demo_widgets();
	    lv_demo_widgets_start_slideshow();
	    for (;;)
	    {
	    	testsloopprocessing();
	    	lv_timer_handler();
	    }
	}
#endif
#if 0
	{
		for (;;)
		{
			unsigned adci;
			for (adci = 0; adci < 8; ++ adci)
			{
				uint_fast8_t valid;
				unsigned v = mcp3208_read(targetadc2, 0, adci, & valid);
				PRINTF("%4u ", v);
			}
			PRINTF("\n");
		}
	}
#endif
#if 0 && defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U)
	{
		unsigned core;
		for (core = 0; core < arm_hardware_clustersize(); ++ core)
		{
			const uintptr_t base = (uintptr_t) (GICV + core);
			PRINTF("GICV%u:\n", core);
			printhex32(base, (void *) base, 512);
		}
	}
#endif
#if 0
	{
		// Test PD0..PD13 relays
		const portholder_t allmask = UINT32_C(0x03FFF);
		arm_hardware_piod_outputs(allmask, 0 * allmask);
		for (;;)
		{
			unsigned i;
			for (i = 0; i < 14; ++ i)
			{
				const portholder_t mask = UINT32_C(1) << i;
				gpioX_setstate(GPIOD, allmask, mask);
				local_delay_ms(500);
				gpioX_setstate(GPIOD, allmask, 0);
				local_delay_ms(500);
				TP();
			}
		}
	}
#endif
#if 0 && WITHDEBUG
	{
		dcdcsynctest();
//		hardware_bl_pwm_set_duty(HARDWARE_DCDC_PWMCH, 1000000, 50);
//		for (;;)
//			;
	}
#endif
#if 0 && LINUX_SUBSYSTEM && WITHAD9363IIO
	{
		int ad9363_iio_test (const char * uri);
		ad9363_iio_test("usb:");
	}
#endif
#if 0 && (CFG_TUH_ENABLED && CFG_TUH_HID)
	{
		void hidparsertest(void);
		hidparsertest();
	}
#endif
#if 0 && WITHHDMITVHW
	{
		hardware_edid_test();
	}
#endif
#if 0 && ! WITHISBOOTLOADER
	{
		//TP();
		unsigned i;
		for (i = 0; i < 2000; ++ i)
		{
			testsloopprocessing();		// обработка отложенного вызова user mode функций
			local_delay_ms(1);
		}
		TP();
		enum { N = 10 };
		int filen = 0;
		int files = 0;
		void * buffers [N];
		for (filen = 0; filen < 99 && files < N; ++ filen)
		{
			char fname [64];
			if (filen == 0)
				local_snprintf_P(fname, 64, "demo.png");
			else
				local_snprintf_P(fname, 64, "demo%d.png", filen);

			// Тест чтения с FatFS данных
			do {
				static FIL pngfile; /* Описатель открытого файла - нельзя располагать в Cortex-M4 CCM */
				static FATFS fs; /* File system object  - нельзя располагать в Cortex-M4 CCM */
				const size_t pngbufsize = 2 * 1024 * 1024;
				buffers [files] = malloc(pngbufsize);
				if (buffers [files] == NULL)
					break;
				FRESULT rc;
				UINT nread;

				rc = f_mount(& fs, "", 0);
				if (rc != FR_OK) {
					PRINTF("SD card not found\n");
					break;
				}
				rc = f_open(&pngfile, fname, FA_OPEN_EXISTING | FA_READ);
				if (rc != FR_OK) {
					PRINTF("Key file '%s' not found\n", fname);
					break;
				}
				rc = f_read(& pngfile, buffers [files], pngbufsize, & nread);
				if (rc != FR_OK) {
					PRINTF("Key file '%s' can not be read\n", fname);
					f_close(&pngfile);
					break;
				}
				//PRINTF("Key file '%s' (%u bytes):\n", fname, (unsigned) nread);
				//printhex(0, data, nread);
				f_close(& pngfile);

				++ files;
				break;

			} while (0);
		}

		int seefile = 0;
		if (files)
			testpng_no_stretch(buffers [seefile], 0);	// становить формат DE2_FORMAT_XBGR_8888
		for (;files;)
		{
			for (;;)
			{
				uint_fast16_t kbch;
				uint_fast8_t kbready;
				processmessages(& kbch, & kbready);
				if (kbready)
				{
					switch (kbch)
					{
					case KBD_CODE_CWMSG2:	// F2
					case KBD_CODE_BAND_DOWN:
						if (seefile > 0)
							-- seefile;
						else
							seefile = files - 1;
						testpng_no_stretch(buffers [seefile], 0);	// становить формат DE2_FORMAT_XBGR_8888
						continue;

					case KBD_CODE_CWMSG3:	// F3
					case KBD_CODE_BAND_UP:
						if ((seefile + 1) < files)
							++ seefile;
						else
							seefile = 0;
						testpng_no_stretch(buffers [seefile], 0);	// становить формат DE2_FORMAT_XBGR_8888
						continue;

					case KBD_CODE_DISPMODE:
					case KBD_CODE_CWMSG1:	// F1
						break;

					default:
						continue;
					}
					break;
				}
				else
				{
					continue;
				}
			}

		}
		for (i = 0; i < files; ++ i)
		{
			free(buffers [i]);
		}
	}
#endif
#if 0 && WITHLTDCHW && LCDMODE_LTDC
	{
		static RAMNC PACKEDCOLORPIP_T buffer [GXSIZE(DIM_X, DIM_Y)];
		//PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
		const uint_fast16_t dx = DIM_X;
		const uint_fast16_t dy = DIM_Y;

		memset(buffer, 0x55, sizeof buffer);
		unsigned opaque;

		opaque = 255;
		colpip_fillrect(& dbv, 0, 0, DIM_X, DIM_Y, TFTALPHA(opaque, COLOR_RED));
		PRINTF("Background (opaque=%u)\n", opaque);
		printhex32(0, buffer, 16);
		//display_text(db, 0, 0, "Start...", & dbstylev);
		//display_text(100 / GRID2X(1), 100 / GRID2Y(1), "test", & dbstylev);

		opaque = 128;
		colpip_rectangle(& dbv, 0, 0, 100, 100, COLOR_BLUE, FILL_FLAG_MIXBG, opaque);
		PRINTF("blue (opaque=%u)\n", opaque);
		printhex32(0, buffer, 16);

		opaque = 128;
		colpip_rectangle(& dbv, 0, 0, 100, 100, COLOR_GREEN, FILL_FLAG_MIXBG, opaque);
		PRINTF("green (opaque=%u)\n", opaque);
		printhex32(0, buffer, 16);
		//colmain_nextfb();
		for (;;)
			;
	}
#endif /* WITHLTDCHW && LCDMODE_LTDC */
#if 0 && ! WITHISBOOTLOADER
	{
		// Test lidar
		// Benewake TFmini Plus LIDAR
		// https://amperka.ru/product/lidar-tfmini-plus
		// https://wiki.amperka.ru/products:lidar-tfmini-plus

		//	RED		5V
		//	BLACK	GND
		//	GREEN	TX
		//	WHITE	RX

		// 115200 8-N-1
		//	(count, data) = pi.bb_serial_read(RX)
		//	if count > 8:
		//	for i in range(0, count - 9):
		//	if data[i] == 89 and data[i + 1] == 89:
		//	  checksum = 0
		//	  for j in range(0, 8):
		//		checksum = checksum + data[i + j]
		//	  checksum = checksum % 256
		//	  if checksum == data[i + 8]:
		//		distance = data[i + 2] + data[i + 3] * 256
		//		return distance

		//	59 59 E5 01 6A 04 40 09 4F
		//	59 59 E6 01 72 04 40 09 58
		//	59 59 E7 01 6A 04 40 09 51
		//	59 59 E5 01 69 04 40 09 4E
		//	59 59 E6 01 6A 04 40 09 50
		//	59 59 E6 01 6D 04 40 09 53
		//	59 59 E5 01 70 04 40 09 55

		PRINTF("Benewake TFmini Plus LIDAR test\n");
		const uint_fast32_t baudrate = 115200;
		hardware_uart1_initialize(1, baudrate, 8, 0, 0);
		hardware_uart1_set_speed(baudrate);
		hardware_uart1_enablerx(1);
		for (;;)
		{
			char c;
			if (hardware_uart1_getchar(& c))
				lidar_parse(c);
			testsloopprocessing();		// обработка отложенного вызова user mode функций
		}
		static const uint8_t data [] =
		{
				1,2,3,4,5,6,7,8,9,0,
			0x59, 0x59, 0xE5, 0x01, 0x6A, 0x04, 0x40, 0x09, 0x4F,
			0x59, 0x59, 0xE6, 0x01, 0x72, 0x04, 0x40, 0x09, 0x58,
			0x59, 0x59, 0xE7, 0x01, 0x6A, 0x04, 0x40, 0x09, 0x51,
			0x59, 0x59, 0xE5, 0x01, 0x69, 0x04, 0x40, 0x09, 0x4E,
			0x59, 0x59, 0xE6, 0x01, 0x6A, 0x04, 0x40, 0x09, 0x50,
			0x59, 0x59, 0xE6, 0x01, 0x6D, 0x04, 0x40, 0x09, 0x53,
			0x59, 0x59, 0xE5, 0x01, 0x70, 0x04, 0x40, 0x09, 0x55,
		};
		unsigned i;
		for (i = 0; i < ARRAY_SIZE(data); ++ i)
		{
			lidar_parse(data [i]);
		}
	}
#endif
#if 0
	{
		// Video Encoding test
#if CPUSTYLE_V3S

		CCU->VE_CLK_REG |= UINT32_C(1) << 31;	// VE_SCLK_GATING
		(void) CCU->VE_CLK_REG;

		CCU->BUS_CLK_GATING_REG1 |= UINT32_C(1) << 0;	// VE_GATING
		(void) CCU->BUS_CLK_GATING_REG1;
		CCU->BUS_SOFT_RST_REG1 &= ~ UINT32_C(1) << 0;	// VE_RST
		(void) CCU->BUS_SOFT_RST_REG1;
		CCU->BUS_SOFT_RST_REG1 |= UINT32_C(1) << 0;	// VE_RST
		(void) CCU->BUS_SOFT_RST_REG1;

#else /* CPUSTYLE_V3S */

		CCU->VE_CLK_REG |= UINT32_C(1) << 31;	// VE_SCLK_GATING
		(void) CCU->VE_CLK_REG;

		CCU->VE_BGR_REG |= UINT32_C(1) << 0;	// VE_GATING
		(void) CCU->VE_BGR_REG;
		CCU->VE_BGR_REG &= ~ (UINT32_C(1) << 16);	// VE_RST
		(void) CCU->VE_BGR_REG;
		CCU->VE_BGR_REG |= (UINT32_C(1) << 16);	// VE_RST
		(void) CCU->VE_BGR_REG;

#endif /* CPUSTYLE_V3S */

		PRINTF("VE_VERSION=%08X\n", (unsigned) VENCODER->VE_VERSION);
		printhex32(VENCODER_BASE, VENCODER, sizeof * VENCODER);

	}
#endif
#if 0 && LCDMODE_LTDC && WITHLTDCHW
	{
		// "Squash" test
		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
		TP();
	#if defined (TCONTV_PTR)
		{
			static const uint8_t picture0 [] =
			{
				#include "src/testdata/picture.h"
			};
			PACKEDTVBUFF_T * const fb = tvout_fb_draw();
			memcpy(fb, picture0, sizeof picture0);
			//memcpy(fb, picture0, datasize_dmabuffer1fb());
			tvout_nextfb();
			TP();
			for (;0;)
			{
				testsloopprocessing();		// обработка отложенного вызова user mode функций
				char c;
				if (dbg_getchar(& c))
				{
					PRINTF("pkey=%02X\n", (unsigned char) c);
				}
			}

		}
	#endif /* defined (TCONTV_PTR) */
		unsigned count = 100;		// количество смен направления до оконяания теста
		const int rectX = DIM_X / 8;
		const int rectY = DIM_Y / 4;
		int stepX = 1;
		int stepY = 1;
		int posX = DIM_X / 2;
		int posY = DIM_Y / 2;
		unsigned steps = 0;
		for (;;)
		{
			gxdrawb_t dbv;	// framebuffer для выдачи диагностических сообщений
			gxdrawb_initialize(& dbv, colmain_fb_draw(), DIM_X, DIM_Y);
//			colpip_fill(fb, dx, dy, COLOR_GRAY);
//			colpip_fillrect(fb, dx, dy, 50, 50, 50, 50, TFTRGB(230, 128, 128));
//			hardware_ltdc_main_set4((uintptr_t) fb, 0, 0, 0);
//			for (;;)
//				;
			char s [32];
			const size_t len = snprintf(s, ARRAY_SIZE(s), "%u", steps ++);
			// Erase background
			//colpip_fillrect(fb, DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, display2_getbgcolor());
			colpip_fillrect(& dbv, 0, 0, DIM_X, DIM_Y, TFTRGB(255, 255, 255));
			// Draw rextangle
			colpip_fillrect(& dbv, posX, posY, rectX, rectY, TFTRGB(0, 0, 0));
			unifont_text(& dbv, posX, posY + rectY / 2, & unifont_small, s, len, TFTRGB(255, 255, 255));

			colmain_nextfb();


			testsloopprocessing();		// обработка отложенного вызова user mode функций
			char c;
			if (dbg_getchar(& c))
			{
				PRINTF("skey=%02X\n", (unsigned char) c);
				static unsigned code;
				switch (c)
				{
				case '+':
					code ++;
					break;
				case '-':
					code --;
					break;
				}

//				DISPLAY_TOP->DE_PORT_PERH_SEL = code;
//				PRINTF("DISPLAY_TOP->DE_PORT_PERH_SEL=%08X\n", (unsigned) DISPLAY_TOP->DE_PORT_PERH_SEL);

//				DE_TOP->SEL_CFG = code;
//				PRINTF("DE_TOP->SEL_CFG=%08X\n", (unsigned) DE_TOP->SEL_CFG);

			}
			int change = 0;

			// X limits check
			if (stepX > 0 && posX + rectX >= DIM_X)
			{
				stepX = - 1;
				change = 1;
			}
			else if (stepX < 0 && posX == 0)
			{
				stepX = + 1;
				change = 1;
			}
			// Y limits check
			if (stepY > 0 && posY + rectY >= DIM_Y)
			{
				stepY = - 1;
				change = 1;
			}
			else if (stepY < 0 && posY == 0)
			{
				stepY = + 1;
				change = 1;
			}

			if (change && -- count == 0)
				break;

			posX += stepX;
			posY += stepY;
		}
		PRINTF("Done squash test\n");
	}
#endif
#if 0 && WITHTVDHW && LCDMODE_LTDC && WITHLTDCHW
	{
		cap_test();

	}
#endif
#if 0
	{
		// i2c bus test i2c test twi bus test twi test
		unsigned n = 3;
		for (;n --;)
		{
			unsigned addr;
			PRINTF("I2C bus scan:\n");
			for (addr = 2; addr < 254; addr += 2)
			{
				uint8_t v = 0xFF;
				int err = i2chw_read(addr | 0x01, & v, 1);
				if (err == 0)
				{
					PRINTF("addr8bit=0x%02X, addr7bit=0x%02X\n", addr, addr / 2);
				}
			}
		}
		PRINTF("I2C bus scan done\n");
	}
#endif
#if 0
	{
		// i2c bus test i2c test twi bus test twi test
		unsigned n = 3;
		for (;n --;)
		{
			unsigned addr;
			PRINTF("I2C_2 bus scan:\n");
			for (addr = 2; addr < 254; addr += 2)
			{
				uint8_t v = 0xFF;
				int err = i2chw2_read(addr | 0x01, & v, 1);
				if (err == 0)
				{
					PRINTF("addr8bit=0x%02X, addr7bit=0x%02X\n", addr, addr / 2);
				}
			}
		}
		PRINTF("I2C_2 bus scan done\n");
	}
#endif
#if 0 && CPUSTYLE_T507
	// Allwinner T507 Thermal sensor test
    PRCM->VDD_SYS_PWROFF_GATING_REG |= (UINT32_C(1) << 4); // ANA_VDDON_GATING
    local_delay_ms(10);

	CCU->THS_BGR_REG |= (1 << 0); // THS_GATING
	CCU->THS_BGR_REG |= (1 << 16); // THS_RESET
	THS->THS_EN &= ~ (1 << 0);
	THS->THS_CTRL = (0x1df << 16) | 0x2f;
	THS->THS_PER = 0x3a << 12;
	THS->THS_FILTER = (1 << 2) | 1;

	//	03006200: 33007C00 FC004808 01488718 14771E8F 00000000 FB6B10F0 BB706B6C 196DA6DB
	//	03006220: 5A5B5C5C 00065C5C 1B809432 02810000 00000000 00000000 00000000 00000000

	const unsigned THS0_CDATA = (* (volatile uint32_t *) (SID_BASE + 0x220) >> 0) & 0x0FFF;
	const unsigned THS1_CDATA = 0x0C5C;
	const unsigned THS2_CDATA = 0x0C5C;
	const unsigned THS3_CDATA = 0x0C5C;

	THS->THSx_CDATA[0] = (THS1_CDATA << 16) | THS0_CDATA;
	THS->THSx_CDATA[1] = (THS3_CDATA << 16) | THS2_CDATA;

	THS->THS_EN |= (1 << 0);

//	printhex32(SID_BASE, SID, sizeof * SID);
//	printhex32(THS_BASE, THS, sizeof * THS);
#endif
#if CPUSTYLE_VM14 && 1
	{
		vm41nandtest();
	}
#endif
#if 0
	{

		extern uint32_t __RAMNC_BASE;
		extern uint32_t __RAMNC_TOP;
		const uintptr_t __ramnc_base = (uintptr_t) & __RAMNC_BASE;
		const uintptr_t __ramnc_top = (uintptr_t) & __RAMNC_TOP;

		PRINTF("__ramnc_base=0x%08X, __ramnc_top=0x%08X (%u MB)\n", (unsigned) __ramnc_base, (unsigned) __ramnc_top, (unsigned) ((__ramnc_top - __ramnc_base) / 1024 / 1024));
	}
#endif
#if 0
	{
		const spitarget_t target = targetadc2;	// PH5
		for (;;)
		{
			uint_fast8_t valid;
			PRINTF("ADC0..4: 0x%03X 0x%03X 0x%03X 0x%03X 0x%03X\n",
					(unsigned) mcp3208_read(target, 0, 0, & valid),
					(unsigned) mcp3208_read(target, 0, 1, & valid),
					(unsigned) mcp3208_read(target, 0, 2, & valid),
					(unsigned) mcp3208_read(target, 0, 3, & valid),
					(unsigned) mcp3208_read(target, 0, 4, & valid)
					);
			testsloopprocessing();		// обработка отложенного вызова user mode функций
		}
	}
#endif
#if 0
	{
		// cache line size test
		static __ALIGNED(256) uint8_t data [256];
		memset(data, 0xE5, sizeof data);
		dcache_clean_all();
		memset(data, 0x00, sizeof data);
		__DSB();
		__set_DCIMVAC((uintptr_t) data);	// Invalidate data cache line by address.
		//dcache_invalidate((uintptr_t) data, 8);
		//dcache_clean_all();
		printhex((uintptr_t) data, data, sizeof data);
		for (;;)
			;

	}
#endif
#if 0 && CPUSTYLE_T113
	{
		// HiFI4 DSP start test

		static const uint8_t dsp_code [] =
		{
		#include "../build/HiFi4/app/main.txt"
		};
		//	SRAM A1			0x00020000---0x00027FFF		32 KB

		//	DSP0 IRAM		0x00028000---0x00037FFF		64 KB		The local sram is switched to system boot.
		//	DSP0 DRAM0		0x00038000---0x0003FFFF		32 KB		The local sram is switched to system boot.
		//	DSP0 DRAM1		0x00040000---0x00047FFF		32 KB		The local sram is switched to system boot.
		//
		//	DSP0 IRAM (local sram)		0x00400000---0x0040FFFF		64 KB		The local sram is switched to DSP.
		//	DSP0 DRAM0 (local sram)		0x00420000---0x00427FFF		32 KB		The local sram is switched to DSP.
		//	DSP0 DRAM1 (local sram)		0x00440000---0x00447FFF		32 KB		The local sram is switched to DSP.

		const uintptr_t remap_cpu = (uintptr_t) 0x00028000;

		enum { M = 2 };
		CCU->DSP_CLK_REG = (CCU->DSP_CLK_REG & ~ ((0x07 << 24) | (0x1F << 0))) |
				(0x04u << 24) |	// src: PLL_AUDIO1(DIV2)
				((M - 1) << 0) |
				0;

		CCU->DSP_CLK_REG |= (UINT32_C(1) << 31);	// DSP_CLK_GATING


		CCU->DSP_BGR_REG |= (UINT32_C(1) << 1);	// DSP_CFG_GATING
		CCU->DSP_BGR_REG |= (UINT32_C(1) << 17);	// DSP_CFG_RST

		CCU->DSP_BGR_REG &= ~ (UINT32_C(1) << 16);	// DSP_RST

		// Map local sram to CPU
		SYS_CFG->DSP_BOOT_RAMMAP_REG = 0x01;	/* DSP BOOT SRAM REMAP ENABLE 1: DSP 128K Local SRAM Remap for System Boot */

		// https://github.com/YuzukiHD/FreeRTOS-HIFI4-DSP/blob/164696d952116d20100daefd7a475d2ede828eb0/host/uboot-driver/dsp/sun8iw20/dsp_reg.h#L33C1-L39C65
		//xtest();
		PRINTF("allwnr_t113_get_dsp_freq()=%" PRIuFAST32 "\n", allwnr_t113_get_dsp_freq());
		//PRINTF("DSP_ALT_RESET_VEC_REG=%08" PRIX32 "\n", DSP0_CFG->DSP_ALT_RESET_VEC_REG);
		//PRINTF("DSP_STAT_REG=%08" PRIX32 "\n", DSP0_CFG->DSP_STAT_REG);
		//local_delay_ms(300);

		//memset((void *) remap_cpu, 0xE5, 128 * 1024);
		//memcpy((void *) remap_cpu, dsp_code, sizeof dsp_code);
//		for (unsigned i = 0; i < (128 * 1024) / 4; ++ i)
//		{
//			volatile uint32_t * const p = (void *) remap_cpu;
//			p [xlate_dsp2mpu(i * 4) / 4] = i * 4;
//		}
		const size_t dsp_code_size = sizeof dsp_code;
		copy2dsp((void *) remap_cpu, dsp_code, 0, dsp_code_size);
		zero2dsp((void *) remap_cpu, dsp_code_size, (128 * 1024u) - dsp_code_size);
		dcache_clean(remap_cpu, 128 * 1024);
		//printhex(remap_cpu, (void *) dsp_code + (64 * 1024), 256);
		//PRINTF("Map local sram to DSP\n");
		// Map local sram to DSP
		SYS_CFG->DSP_BOOT_RAMMAP_REG = 0x00;	/* DSP BOOT SRAM REMAP ENABLE 0: DSP 128K Local SRAM Remap for DSP_SYS */

		// DSP Start address change
		DSP0_CFG->DSP_ALT_RESET_VEC_REG = 0x00028800; //0x400000 if non-cached need
		//DSP0_CFG->DSP_ALT_RESET_VEC_REG = 0x20028000; //0x400000 if non-cached need
		DSP0_CFG->DSP_CTRL_REG0 |= (UINT32_C(1) << 1);	// BIT_START_VEC_SEL
		//PRINTF("DSP_ALT_RESET_VEC_REG=%08" PRIX32 "\n", DSP0_CFG->DSP_ALT_RESET_VEC_REG);

		DSP0_CFG->DSP_CTRL_REG0 |= (UINT32_C(1) << 0);	// Set runstall

		DSP0_CFG->DSP_CTRL_REG0 |= (UINT32_C(1) << 2);	/* set dsp clken */
		CCU->DSP_BGR_REG |= (UINT32_C(1) << 16);	// DSP_RST
		DSP0_CFG->DSP_CTRL_REG0 &= ~ (UINT32_C(1) << 0);	// Clear runstall
		(void) DSP0_CFG->DSP_CTRL_REG0;

		/*
		 * DSP STAT Register
		 */
//		#define BIT_PFAULT_INFO_VALID (0)
//		#define BIT_PFAULT_ERROR (1)
//		#define BIT_DOUBLE_EXCE_ERROR (2)
//		#define BIT_XOCD_MODE (3)
//		#define BIT_DEBUG_MODE (4)
//		#define BIT_PWAIT_MODE (5)
//		#define BIT_IRAM0_LOAD_STORE (6)

		unsigned sss = DSP0_CFG->DSP_STAT_REG;

		local_delay_ms(1300);
		//PRINTF("DSP_STAT_REG=%08" PRIX32 "\n", DSP0_CFG->DSP_STAT_REG);
		for (;;)
			;
	}
#endif
#if 0 && (CPUSTYLE_T113 || CPUSTYLE_F133)
	{
		CCU->GPADC_BGR_REG |= (UINT32_C(1) << 16); 	// 1: De-assert reset  HOSC
		CCU->GPADC_BGR_REG |= (UINT32_C(1) << 0); 	// 1: Pass clock
		GPADC->GP_SR_CON |= (0x2fu << 0);	// set the acquiring time of ADC
		GPADC->GP_SR_CON |= (0x1dfu << 16);	// set the ADC sample frequency divider
		GPADC->GP_CTRL |= (0x2u << 18); 		// set the continuous conversion mode
		GPADC->GP_CS_EN |= (UINT32_C(1) << 0); 		// enable the analog input channel
		GPADC->GP_DATA_INTC |= (UINT32_C(1) << 0);	// enable the GPADC data interrupt
		arm_hardware_set_handler_system(GPADC_IRQn, gpadc_inthandler);
		GPADC->GP_CTRL |= (UINT32_C(1) << 16);		// enable the ADC function
		for(;;) {}
	}
#endif /* #if (CPUSTYLE_T113 || CPUSTYLE_F133) */
#if 0
	{
		PRINTF("chipid=%08X\n", (unsigned) allwnr_t113_get_chipid());
	}
#endif
#if 0
	{
		//printhex32(SID_BASE, SID, 1024);
		PRINTF("SID memory dump (direct access):\n");
		printhex32(0, & SID->SID_DATA, 64);
		PRINTF("SID memory dump (indirect access):\n");
		unsigned offs;
		for (offs = 0; offs < 256; offs += 4)
		{
			PRINTF("SID[0x%02X]=%08X\n", offs, (unsigned) allwnr_sid_read(offs));
		}

	}
#endif
#if 0 && (CPUSTYLE_T113) && WITHDEBUG
	{
//		uint32_t midr;
//		asm volatile("mrc p15, 0, %0, c0, c0, 0" : "=r" (midr));
//		// T113M4: IIDR=0200143B, midr=410FC075 xfel sid: 934072002c0048140105061c54731853
//		// T113S3: IIDR=0200143B, midr=410FC075 xfel sid: 934060000c00481401464015586213cc
//		PRINTF("IIDR=%08X, midr=%08X\n", (unsigned) GICDistributor->IIDR, (unsigned) midr);
		PRINTF("chipid=%08X\n", (unsigned) allwnr_t113_get_chipid());
		if (allwnr_t113_get_chipid() == CHIPID_T113M4020DC0)
		{
			PRINTF("freq = %u MHz, PLL_CPU_CTRL_REG=%08X,CPU_AXI_CFG_REG=%08X\n", (unsigned) (allwnr_t113_get_pll_cpu_freq() / 1000 / 1000), (unsigned) CCU->PLL_CPU_CTRL_REG, (unsigned) CCU->CPU_AXI_CFG_REG);
			dbg_flush();	/* for see rv64 running effects on UART0 */
			//	la	a0, 0x02500000
			//	la	a1, 0x23
			//	sb	a1, 0 (a0)
			//	xxx:	j	xxx
			// Test: write byte 0x23 ('#') to 0x02500000 = UART0 data register
			static const uint32_t rv64code [] = {
					0x02500537, // 37 05 50 02
					0x0230059B,	// 9B 05 30 02
					0x00B50023, // 23 00 B5 00
					0x0000006F, // 6F 00 00 00,
			};
			dcache_clean_all();

			/* reset RISC-V core */
			CCU->MBUS_MAT_CLK_GATING_REG |= (UINT32_C(1) << 11);				// RISC-V_MCLK_EN
			CCU->RISC_CFG_BGR_REG |= (UINT32_C(1) << 16) | (UINT32_C(1) << 0);
			CCU->RISC_RST_REG = (UINT32_C(0x16AA) << 16) | 0 * ((UINT32_C(1) << 0));	/* Assert rv64 reset */

			/* setup RISC-V CPU & AXI clock */
			//	RISC-V Clock = Clock Source/M.
			//	RISC-V_AXI Clock = RISC-V Clock/N.
			const uint_fast32_t M = 1;	// FACTOR_N is from 1 to 3.
			const uint_fast32_t N = 3;	// FACTOR_M is from 0 to 31.
			CCU->RISC_CLK_REG =
					0x05 * (UINT32_C(1) << 24) |	// PLL_CPU
					(N - 1) * (UINT32_C(1) << 8) |	// RISC-V_DIV_CFG
					(M - 1) * (UINT32_C(1) << 0) |	// RISC-V_AXI_DIV_CFG
					0;
			CCU->RISC_CLK_REG |= (UINT32_C(1) << 31);

			CCU->RISC_GATING_REG = (UINT32_C(1) << 31) | (UINT32_C(0x16AA) << 0);	/* key required for modifications (d1-h_user_manual_v1.0.pdf, page 152). */
			RISC_CFG->RISC_STA_ADD0_REG = (uintptr_t) rv64code; //ptr_lo32((uintptr_t) rv64code);
			RISC_CFG->RISC_STA_ADD1_REG = 0;//ptr_hi32((uintptr_t) rv64code);
			CCU->RISC_RST_REG = (UINT32_C(0x16AA) << 16) | 1 * ((UINT32_C(1) << 0));	/* De-assert rv64 reset */

			local_delay_ms(100);	/* see '#' on serial port UART0 */
			CCU->RISC_RST_REG = (UINT32_C(0x16AA) << 16) | 0 * ((UINT32_C(1) << 0));	/* Assert rv64 reset */
		}
	}
#endif
#if 0 && CPUSTYLE_CA53
	{
		// H9.2.46 MIDR_EL1, Main ID Register
	#if defined(__aarch64__)
		const unsigned midr = __get_MIDR_EL1();
	#else
		const unsigned midr = __get_MIDR();
	#endif
		const unsigned partNum = (midr >> 4) & 0xFFF;
		// Allwinner A64	- midr=0x410FD034 (partNum=0xD03 (3331)
		// Allwinner T507-H	- midr=0x410FD034 (partNum=0xD03 (3331)
		PRINTF("midr=0x%08X (partNum=0x%03X (%u)\n", midr, partNum, partNum);
	}
#endif
#if 0 && CPUSTYLE_CA53
	{
#if defined(__aarch64__)
		const uint_fast32_t ca53_cbar = __get_CA53_CBAR();
		PRINTF("__get_CBAR()=%08X\n", ca53_cbar);
		PRINTF("__get_CPUACTLR_EL1()=%08X\n", (unsigned) __get_CPUACTLR_EL1());
		PRINTF("__get_CPUECTLR_EL1()=%08X\n", (unsigned) __get_CPUECTLR_EL1());
#else
		const uint_fast32_t ca53_cbar = __get_CA53_CBAR();
		PRINTF("__get_CBAR()=%08X\n", ca53_cbar);
		PRINTF("__get_CPUACTLR()=%08X\n", (unsigned) __get_CPUACTLR());
		PRINTF("__get_CPUECTLR()=%08X\n", (unsigned) __get_CPUECTLR());
#endif
		const uint64_t periphbase = (uint64_t) (ca53_cbar & UINT32_C(0xFFFC0000)) | ((uint64_t) (ca53_cbar & UINT32_C(0xFF)) << 32);
		PRINTF("periphbase=%016" PRIX64 "\n", periphbase);			/* SYS_CFG_BASE */


		PRINTF("GIC_DISTRIBUTOR_BASE=%08X\n", (unsigned) GIC_DISTRIBUTOR_BASE);
		PRINTF("GIC_INTERFACE_BASE=%08X\n", (unsigned) GIC_INTERFACE_BASE);
		//printhex(ca53_cbar, ca53_cbar, 256);

		ASSERT(GIC_DISTRIBUTOR_BASE == (periphbase + 0x81000));
		ASSERT(GIC_INTERFACE_BASE == (periphbase + 0x82000));
	}
#endif
#if 0 && (__CORTEX_A == 7U)
	{
		const uint_fast32_t cbar = __get_CBAR();
		PRINTF("__get_CBAR()=%08X\n", (unsigned) __get_CBAR());

	}
#endif
#if 0 && CPUSTYLE_A64 && WITHDEBUG
	{
		PRINTF("C0_CPUX_CFG->C_CTRL_REG0=%08X\n", (unsigned) C0_CPUX_CFG->C_CTRL_REG0);
		PRINTF("C0_CPUX_CFG->GENER_CTRL_REG0=%08X\n", (unsigned) C0_CPUX_CFG->GENER_CTRL_REG0);
		PRINTF("C0_CPUX_CFG->C_CPU_STATUS=%08X\n", (unsigned) C0_CPUX_CFG->C_CPU_STATUS);

		C0_CPUX_CFG->GENER_CTRL_REG0 &= ~ (UINT32_C(1) << 4);	// GICCDISABLE
		C0_CPUX_CFG->C_CPU_STATUS |= (0x0Fu << 24);		// SMP
		C0_CPUX_CFG->C_CTRL_REG0 |= (0x0Fu << 24);		// AA64nAA32 1: AArch64

		PRINTF("C0_CPUX_CFG->C_CTRL_REG0=%08X\n", (unsigned) C0_CPUX_CFG->C_CTRL_REG0);
		PRINTF("C0_CPUX_CFG->GENER_CTRL_REG0=%08X\n", (unsigned) C0_CPUX_CFG->GENER_CTRL_REG0);
		PRINTF("C0_CPUX_CFG->C_CPU_STATUS=%08X\n", (unsigned) C0_CPUX_CFG->C_CPU_STATUS);
	}
#endif
#if 0 && LCDMODE_LTDC
	{
		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
		TP();

		#include "src/testdata/Cobra.png.h"

		testpng(Cobra_png);
		for (;;)
		{
			testsloopprocessing();		// обработка отложенного вызова user mode функций
		}
	}
#endif
#if 0 && LCDMODE_LTDC
	{
		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
		TP();

		static const unsigned char png [] =
		{
			#include "src/testdata/1920x1080.h"

		};

		testpng_no_stretch(png, 0);	// становить формат DE2_FORMAT_XBGR_8888
		for (;;)
		{
			testsloopprocessing();		// обработка отложенного вызова user mode функций
		}
	}
#endif
#if 0 && (CPUSTYLE_H3) && WITHDEBUG
	{
		PRINTF("CPU_FREQ=%u MHz\n", (unsigned) (CPU_FREQ / 1000 / 1000));
//		PRINTF("allwnr_h3_get_axi_freq()=%u MHz\n", (unsigned) (allwnr_h3_get_axi_freq() / 1000 / 1000));
//		PRINTF("allwnr_h3_get_mbus_freq()=%u MHz\n", (unsigned) (allwnr_h3_get_mbus_freq() / 1000 / 1000));
//		PRINTF("allwnr_h3_get_psi_ahb1_ahb2_freq()=%u MHz\n", (unsigned) (allwnr_h3_get_psi_ahb1_ahb2_freq() / 1000 / 1000));
//		PRINTF("allwnr_h3_get_apb2_freq()=%u MHz\n", (unsigned) (allwnr_h3_get_apb2_freq() / 1000 / 1000));
//		PRINTF("allwnr_h3_get_apb1_freq()=%u MHz\n", (unsigned) (allwnr_h3_get_apb1_freq() / 1000 / 1000));
//		PRINTF("allwnr_h3_get_pll_peri0_x1_freq()=%u MHz\n", (unsigned) (allwnr_h3_get_pll_peri0_x1_freq() / 1000 / 1000));
//		PRINTF("allwnr_h3_get_pll_peri1_x1_freq()=%u MHz\n", (unsigned) (allwnr_h3_get_pll_peri1_x1_freq() / 1000 / 1000));
//		PRINTF("allwnr_h3_get_ahbs_freq()=%u MHz\n", (unsigned) (allwnr_h3_get_ahbs_freq() / 1000 / 1000));
//		PRINTF("allwnr_h3_get_apbs1_freq()=%u MHz\n", (unsigned) (allwnr_h3_get_apbs1_freq() / 1000 / 1000));
		//t507_hdmi_edid_test();
	}
#endif
#if 0 && (CPUSTYLE_T507) && WITHDEBUG
	{
		PRINTF("CPU_FREQ=%u MHz\n", (unsigned) (CPU_FREQ / 1000 / 1000));
		PRINTF("allwnr_t507_get_axi_freq()=%u MHz\n", (unsigned) (allwnr_t507_get_axi_freq() / 1000 / 1000));
		PRINTF("allwnr_t507_get_apb_freq()=%u MHz\n", (unsigned) (allwnr_t507_get_apb_freq() / 1000 / 1000));
		PRINTF("allwnr_t507_get_apb1_freq()=%u MHz\n", (unsigned) (allwnr_t507_get_apb1_freq() / 1000 / 1000));
		PRINTF("allwnr_t507_get_apb2_freq()=%u MHz\n", (unsigned) (allwnr_t507_get_apb2_freq() / 1000 / 1000));
		PRINTF("allwnr_t507_get_mbus_freq()=%u MHz\n", (unsigned) (allwnr_t507_get_mbus_freq() / 1000 / 1000));
		PRINTF("allwnr_t507_get_psi_ahb1_ahb2_freq()=%u MHz\n", (unsigned) (allwnr_t507_get_psi_ahb1_ahb2_freq() / 1000 / 1000));
		PRINTF("allwnr_t507_get_pll_peri0_x1_freq()=%u MHz\n", (unsigned) (allwnr_t507_get_pll_peri0_x1_freq() / 1000 / 1000));
		PRINTF("allwnr_t507_get_pll_peri1_x1_freq()=%u MHz\n", (unsigned) (allwnr_t507_get_pll_peri1_x1_freq() / 1000 / 1000));
		PRINTF("allwnr_t507_get_ahbs_freq()=%u MHz\n", (unsigned) (allwnr_t507_get_ahbs_freq() / 1000 / 1000));
		PRINTF("allwnr_t507_get_apbs1_freq()=%u MHz\n", (unsigned) (allwnr_t507_get_apbs1_freq() / 1000 / 1000));
	}
#endif
#if 0 && (CPUSTYLE_T113 || CPUSTYLE_F133) && WITHDEBUG
	{
		PRINTF("CPU_FREQ=%u MHz\n", (unsigned) (CPU_FREQ / 1000 / 1000));
		PRINTF("allwnr_t113_get_axi_freq()=%u MHz\n", (unsigned) (allwnr_t113_get_axi_freq() / 1000 / 1000));
		PRINTF("allwnr_t113_get_mbus_freq()=%u MHz\n", (unsigned) (allwnr_t113_get_mbus_freq() / 1000 / 1000));
		PRINTF("allwnr_t113_get_psi_freq()=%u MHz\n", (unsigned) (allwnr_t113_get_psi_freq() / 1000 / 1000));
		PRINTF("allwnr_t113_get_ahb0_freq()=%u MHz\n", (unsigned) (allwnr_t113_get_ahb0_freq() / 1000 / 1000));
		PRINTF("allwnr_t113_get_apb1_freq()=%u MHz\n", (unsigned) (allwnr_t113_get_apb1_freq() / 1000 / 1000));
		PRINTF("allwnr_t113_get_apb0_freq()=%u MHz\n", (unsigned) (allwnr_t113_get_apb0_freq() / 1000 / 1000));
	}
#endif
#if 0 && CPUSTYLE_V3S
	{
		// V3s clocks information print
		PRINTF("allwnr_v3s_get_cpu_freq()=%u MHz\n", (unsigned) (allwnr_v3s_get_cpu_freq() / 1000 / 1000));
		PRINTF("allwnr_v3s_get_axi_freq()=%u MHz\n", (unsigned) (allwnr_v3s_get_axi_freq() / 1000 / 1000));
		PRINTF("allwnr_v3s_get_sysapb_freq()=%u MHz\n", (unsigned) (allwnr_v3s_get_sysapb_freq() / 1000 / 1000));
		PRINTF("allwnr_v3s_get_ahb2_freq()=%u MHz\n", (unsigned) (allwnr_v3s_get_ahb2_freq() / 1000 / 1000));
		PRINTF("allwnr_v3s_get_ahb1_freq()=%u MHz\n", (unsigned) (allwnr_v3s_get_ahb1_freq() / 1000 / 1000));
		PRINTF("allwnr_v3s_get_apb2_freq()=%u MHz\n", (unsigned) (allwnr_v3s_get_apb2_freq() / 1000 / 1000));
		PRINTF("allwnr_v3s_get_apb1_freq()=%u MHz\n", (unsigned) (allwnr_v3s_get_apb1_freq() / 1000 / 1000));
	}
#endif
#if 0
	{
		TP();
		//de_dump();
		testde();
		TP();
	}
#endif
#if 0 && LCDMODE_LTDC && LCDMODE_ARGB8888
	{
		enum { picy = 110, picx = 150 };
		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
		TP();
		static PACKEDCOLORPIP_T layer0_a [GXSIZE(DIM_X, DIM_Y)];
		static PACKEDCOLORPIP_T layer0_b [GXSIZE(DIM_X, DIM_Y)];
		static PACKEDCOLORPIP_T layer1 [GXSIZE(DIM_X, DIM_Y)];
		static PACKEDCOLORPIP_T layer2 [GXSIZE(DIM_X, DIM_Y)];
		static PACKEDCOLORPIP_T layer3 [GXSIZE(DIM_X, DIM_Y)];
		static PACKEDCOLORPIP_T fbpic [GXSIZE(picx, picy)];
		static PACKEDCOLORPIP_T fbpic2 [GXSIZE(picx, picy)];
		static PACKEDCOLORPIP_T fbpic3 [GXSIZE(picx, picy)];

		gxdrawb_t dbv_layer0_a;
		gxdrawb_t dbv_layer0_b;
		gxdrawb_t dbv_layer1;
		gxdrawb_t dbv_layer2;
		gxdrawb_t dbv_layer3;
		gxdrawb_t dbv_fbpic;
		gxdrawb_t dbv_fbpic2;
		gxdrawb_t dbv_fbpic3;

		gxdrawb_initialize(& dbv_layer0_a, layer0_a, DIM_X, DIM_Y);
		gxdrawb_initialize(& dbv_layer0_b, layer0_b, DIM_X, DIM_Y);
		gxdrawb_initialize(& dbv_layer1, layer1, DIM_X, DIM_Y);
		gxdrawb_initialize(& dbv_layer2, layer2, DIM_X, DIM_Y);
		gxdrawb_initialize(& dbv_layer3, layer3, DIM_X, DIM_Y);
		gxdrawb_initialize(& dbv_fbpic, fbpic, picx, picy);
		gxdrawb_initialize(& dbv_fbpic2, fbpic2, picx, picy);
		gxdrawb_initialize(& dbv_fbpic3, fbpic3, picx, picy);


//		dcache_clean_invalidate((uintptr_t) layer0, sizeof layer0);
//		dcache_clean_invalidate((uintptr_t) layer1, sizeof layer1);
//		dcache_clean_invalidate((uintptr_t) fbpic, sizeof fbpic);

	#ifdef RTMIXIDTV
		hardware_ltdc_main_set4(RTMIXIDTV, (uintptr_t) layer0_a, (uintptr_t) layer1, 1*(uintptr_t) layer2, 1*(uintptr_t) layer3);
	#endif
	#ifdef RTMIXIDLCD
		hardware_ltdc_main_set4(RTMIXIDLCD, (uintptr_t) layer0_a, (uintptr_t) layer1, 1*(uintptr_t) layer2, 1*(uintptr_t) layer3);
	#endif

		/* Тестовое изображение для заполнения с color key (с фоном в этом цвете) */
		COLORPIP_T keycolor = COLORPIP_KEY;
//		PRINTF("test: keycolor=%08X\n", keycolor);
//		PRINTF("test: a=%08X\n", COLORPIP_A(keycolor));
//		PRINTF("test: r=%08X\n", COLORPIP_R(keycolor));
//		PRINTF("test: g=%08X\n", COLORPIP_G(keycolor));
//		PRINTF("test: b=%08X\n", COLORPIP_B(keycolor));

		unsigned picalpha = 128;
		colpip_fillrect(& dbv_fbpic, 0, 0, picx, picy, TFTALPHA(picalpha, keycolor));	/* при alpha==0 все биты цвета становятся 0 */
		colpip_fillrect(& dbv_fbpic, picx / 4, picy / 4, picx / 2, picy / 2, TFTALPHA(picalpha, COLORPIP_WHITE));
		colpip_line(& dbv_fbpic, 0, 0, picx - 1, picy - 1, TFTALPHA(picalpha, COLORPIP_WHITE), 0);
		colpip_line(& dbv_fbpic, 0, picy - 1, picx - 1, 0, TFTALPHA(picalpha, COLORPIP_WHITE), 0);
		colpip_fillrect(& dbv_fbpic, picx / 4 + 5, picy / 4 + 5, picx / 2 - 10, picy / 2 - 10, TFTALPHA(0 * picalpha, COLORPIP_WHITE));
		colpip_string_small(& dbv_fbpic, 5, 6, "Hello!", TFTALPHA(picalpha, COLORPIP_WHITE));
		dcache_clean(dbv_fbpic.cachebase, dbv_fbpic.cachesize);

		unsigned pic2alpha = 44;
		colpip_fillrect(& dbv_fbpic2, 0, 0, picx, picy, TFTALPHA(pic2alpha, keycolor));	/* при alpha==0 все биты цвета становятся 0 */
		colpip_fillrect(& dbv_fbpic2, picx / 4, picy / 4, picx / 2, picy / 2, TFTALPHA(pic2alpha, COLORPIP_WHITE));
		colpip_line(& dbv_fbpic2, 0, 0, picx - 1, picy - 1, TFTALPHA(pic2alpha, COLORPIP_WHITE), 0);
		colpip_line(& dbv_fbpic2, 0, picy - 1, picx - 1, 0, TFTALPHA(pic2alpha, COLORPIP_WHITE), 0);
		colpip_string_small(& dbv_fbpic2, 5, 6, "LY2", TFTALPHA(pic2alpha, COLORPIP_WHITE));
		dcache_clean(dbv_fbpic2.cachebase, dbv_fbpic2.cachesize);

		unsigned pic3alpha = 33;
		colpip_fillrect(& dbv_fbpic3, 0, 0, picx, picy, TFTALPHA(pic3alpha, keycolor));	/* при alpha==0 все биты цвета становятся 0 */
		colpip_fillrect(& dbv_fbpic3, picx / 4, picy / 4, picx / 2, picy / 2, TFTALPHA(pic3alpha, COLORPIP_WHITE));
		colpip_line(& dbv_fbpic3, 0, 0, picx - 1, picy - 1, TFTALPHA(pic3alpha, COLORPIP_WHITE), 0);
		colpip_line(& dbv_fbpic3, 0, picy - 1, picx - 1, 0, TFTALPHA(pic3alpha, COLORPIP_WHITE), 0);
		colpip_string_small(& dbv_fbpic3, 5, 6, "LY3", TFTALPHA(pic3alpha, COLORPIP_WHITE));
		dcache_clean(dbv_fbpic3.cachebase, dbv_fbpic3.cachesize);

		/* непрозрачный фон */
		unsigned bgalpha = 255;
		colpip_fillrect(& dbv_layer0_a, 0, 0, DIM_X, DIM_Y, TFTALPHA(bgalpha, COLORPIP_BLACK));	/* opaque color transparent black */
		colpip_fillrect(& dbv_layer0_b, 0, 0, DIM_X, DIM_Y, TFTALPHA(bgalpha, COLORPIP_BLACK));	/* opaque color transparent black */
		/* непрозрачный прямоугольник на фоне */
		colpip_fillrect(& dbv_layer0_a, 10, 10, 400, 300, TFTALPHA(bgalpha, COLORPIP_RED));	// RED - нижний слой не учитывает прозрачность
		colpip_fillrect(& dbv_layer0_b, 10, 10, 400, 300, TFTALPHA(bgalpha, COLORPIP_RED));	// RED - нижний слой не учитывает прозрачность

		/* полупрозрачный фон */
		unsigned fgalpha = 128;
		colpip_fillrect(& dbv_layer1, 0, 0, DIM_X, DIM_Y, TFTALPHA(0, COLORPIP_GREEN));	/* opaque color transparent black */
		colpip_string_small(& dbv_layer1, 0, DIM_Y - 16 * 1, "ly1", TFTALPHA(255, COLORPIP_GREEN));
		colpip_fillrect(& dbv_layer1, 110, 110, DIM_X - 200, DIM_Y - 200, TFTALPHA(fgalpha, COLORPIP_BLUE));	/* transparent black */
		/* полупрозрачный прямоугольник на фоне */
		colpip_fillrect(& dbv_layer1, 120, 120, 200, 200, TFTALPHA(fgalpha, COLORPIP_GREEN));	// GREEN
		/* прозрачный слой */
		unsigned l2alpha = 0;
		colpip_fillrect(& dbv_layer2, 0, 0, DIM_X, DIM_Y, TFTALPHA(l2alpha, COLORPIP_RED));	/* opaque color transparent black */
		colpip_string_small(& dbv_layer2, 0, DIM_Y - 16 * 2, "ly2", TFTALPHA(255, COLORPIP_RED));
		/* прозрачный слой */
		unsigned l3alpha = 0;
		colpip_fillrect(& dbv_layer3, 0, 0, DIM_X, DIM_Y, TFTALPHA(l2alpha, COLORPIP_GREEN));	/* opaque color transparent black */
		colpip_string_small(& dbv_layer3, 0, DIM_Y - 16 * 3, "ly3", TFTALPHA(255, COLORPIP_GREEN));

		TP();
		/* копируем изображение в верхний слой с цветовым ключем */
		colpip_bitblt(
				dbv_layer1.cachebase, dbv_layer1.cachesize,
				& dbv_layer1,
				220, 220,
				dbv_fbpic.cachebase, dbv_fbpic.cachesize,
				& dbv_fbpic,
				0, 0,	// координаты окна источника
				picx, picy, // размер окна источника
				BITBLT_FLAG_NONE | BITBLT_FLAG_CKEY, keycolor
				);

		TP();
		/* копируем изображение в верхний слой БЕЗ цветового ключа */
		colpip_bitblt(
				dbv_layer1.cachebase, dbv_layer1.cachesize,
				& dbv_layer1,
				350, 250,
				dbv_fbpic.cachebase, dbv_fbpic.cachesize,
				& dbv_fbpic,
				0, 0,	// координаты окна источника
				picx, picy, // размер окна источника
				BITBLT_FLAG_NONE, keycolor
				);

		TP();
		/* копируем изображение в верхний слой БЕЗ цветового ключа */
		colpip_stretchblt(
				dbv_layer1.cachebase, dbv_layer1.cachesize,
				& dbv_layer1,
				40, 20, picx * 5 / 2, picy,
				dbv_fbpic.cachebase, dbv_fbpic.cachesize,
				& dbv_fbpic,
				0, 0,	// координаты источника
				picx, picy,	// размеры источника
				BITBLT_FLAG_NONE | BITBLT_FLAG_CKEY, keycolor
				);

		TP();
		/* копируем изображение в верхний слой БЕЗ цветового ключа */
		colpip_stretchblt(
				dbv_layer1.cachebase, dbv_layer1.cachesize,
				& dbv_layer1,
				450, 250, picx * 3 / 2, picy * 3 / 2,
				dbv_fbpic.cachebase, dbv_fbpic.cachesize,
				& dbv_fbpic,
				0, 0,	/* координаты источника */
				picx, picy,	// размеры источника
				BITBLT_FLAG_NONE | BITBLT_FLAG_CKEY, keycolor
				);

		TP();
		/* копируем изображение в верхний слой БЕЗ цветового ключа */
		colpip_stretchblt(
				dbv_layer1.cachebase, dbv_layer1.cachesize,
				& dbv_layer1,
				170, 220, picx * 2 / 3, picy * 2 / 3,
				dbv_fbpic.cachebase, dbv_fbpic.cachesize,
				& dbv_fbpic,
				0, 0,	/* координаты источника */
				picx, picy,	// размеры источника
				BITBLT_FLAG_NONE | BITBLT_FLAG_CKEY, keycolor
				);

		TP();
		/* копируем изображение в верхний слой с цветовым ключем */
		colpip_bitblt(
				dbv_layer1.cachebase, dbv_layer1.cachesize,
				& dbv_layer1,
				90, 90,
				dbv_fbpic.cachebase, dbv_fbpic.cachesize,
				& dbv_fbpic,
				0, 0,	// координаты окна источника
				picx, picy, // размер окна источника
				BITBLT_FLAG_NONE | BITBLT_FLAG_CKEY, keycolor
				);

		TP();
		/* копируем изображение в 2-й слой с цветовым ключем */
		colpip_bitblt(
				dbv_layer2.cachebase, dbv_layer2.cachesize,
				& dbv_layer2,
				30, 330,
				dbv_fbpic2.cachebase, dbv_fbpic2.cachesize,
				& dbv_fbpic2,
				0, 0,	// координаты окна источника
				picx, picy, // размер окна источника
				BITBLT_FLAG_NONE | BITBLT_FLAG_CKEY, keycolor
				);

		TP();
		/* копируем изображение в 3-й слой с цветовым ключем */
		colpip_bitblt(
				dbv_layer3.cachebase, dbv_layer3.cachesize,
				& dbv_layer3,
				370, 20,
				dbv_fbpic3.cachebase, dbv_fbpic3.cachesize,
				& dbv_fbpic3,
				0, 0,	// координаты окна источника
				picx, picy, // размер окна источника
				BITBLT_FLAG_NONE | BITBLT_FLAG_CKEY, keycolor
				);

		// нужно если программно заполняли
		dcache_clean(dbv_layer0_a.cachebase, dbv_layer0_a.cachesize);
		dcache_clean(dbv_layer0_b.cachebase, dbv_layer0_b.cachesize);
		dcache_clean(dbv_layer1.cachebase, dbv_layer1.cachesize);
		dcache_clean(dbv_layer2.cachebase, dbv_layer2.cachesize);
		dcache_clean(dbv_layer3.cachebase, dbv_layer3.cachesize);
//
//		printhex32((uintptr_t) layer0, layer0, 64);
//		printhex32((uintptr_t) layer1, layer1, 64);

		int phase = 0;
		unsigned c = 0;	// cycle
		while(1)
		{
			int cycles = 1024;
			int y = 50;
			int x0 = 270;
			int h = 120;
			int w = DIM_X - x0;
			int xpos = (c * (w - 1)) / (cycles - 1);	/* позиция маркера */

			gxdrawb_t * const drawlayer = phase ? & dbv_layer0_a : & dbv_layer0_b;

			colpip_fillrect(drawlayer, x0, y, w, h, TFTALPHA(bgalpha, COLORPIP_BLACK));
			/* линия в один пиксель рисуется прораммно - за ней требуется flush,
			 * поскольку потом меняется еще аппаратурой - invalidate
			 * */
			colpip_fillrect(drawlayer, x0 + xpos, y, 1, h, TFTALPHA(bgalpha, COLORPIP_WHITE));
			dcache_clean_invalidate(drawlayer->cachebase, drawlayer->cachesize);

		#ifdef RTMIXIDTV
			hardware_ltdc_main_set4(RTMIXIDTV, (uintptr_t) drawlayer->buffer, 1*(uintptr_t) dbv_layer1.buffer, 1*(uintptr_t) dbv_layer2.buffer, 1*(uintptr_t) dbv_layer3.buffer);
		#endif
		#ifdef RTMIXIDLCD
			hardware_ltdc_main_set4(RTMIXIDLCD, (uintptr_t) drawlayer->buffer, 1*(uintptr_t) dbv_layer1.buffer, 1*(uintptr_t) dbv_layer2.buffer, 1*(uintptr_t) dbv_layer3.buffer);
		#endif

			phase = ! phase;
			c = (c + 1) % cycles;
			testsloopprocessing();		// обработка отложенного вызова user mode функций
		}
		for (;;)
		{
			testsloopprocessing();		// обработка отложенного вызова user mode функций
		}
	}
#endif
#if 0 && LCDMODE_LTDC
	{
		// colpip_copyrotate test
		COLORPIP_T keycolor = COLORPIP_KEY;
		enum { picx = 100, picy = 50 };
		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
		TP();
		static PACKEDCOLORPIP_T layer0 [GXSIZE(DIM_X, DIM_Y)];
		static PACKEDCOLORPIP_T fgpic [GXSIZE(picx, picy)];

		/* непрозрачный фон */
		unsigned bgalpha = 255;
		colpip_fillrect(layer0, DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, TFTALPHA(bgalpha, COLORPIP_BLACK));	/* opaque color transparent black */

		/* тестовое изображение */
		unsigned fgalpha = 255;
		colpip_fillrect(fgpic, picx, picy, 0, 0, picx, picy, TFTALPHA(fgalpha, COLORPIP_BLUE));	/* opaque color transparent black */
		colpip_string_small(fgpic, picx, picy, 0, 0, "F", TFTALPHA(fgalpha, COLORPIP_RED));
		dcache_clean((uintptr_t) fgpic, GXSIZE(picx, picy) * sizeof fgpic [0]);

		unsigned grid = picx + 5;
		unsigned tpos;
		for (tpos = 0; tpos < 7; ++ tpos)
		{
			const uint_fast16_t x = tpos * grid;
			const uint_fast16_t y = grid;
			/* рисуем прямоугольники под размещение повёрнутых изображений */
			colpip_fillrect(layer0, DIM_X, DIM_Y, x, y, grid - 3, grid - 3, TFTALPHA(fgalpha, COLORPIP_WHITE));	/* opaque color transparent black */

			if (tpos >= 7)
			{
				/* копируем изображение в верхний слой БЕЗ цветового ключа */
				colpip_bitblt(
						(uintptr_t) layer0, GXSIZE(DIM_X, DIM_Y) * sizeof layer0 [0],
						layer0, DIM_X, DIM_Y,
						x + 1, y + 1,	// получатель Позиция
						(uintptr_t) fgpic, GXSIZE(picx, picy) * sizeof fgpic [0],
						fgpic, picx, picy,
						0, 0,	// координаты окна источника
						picx, picy, // размер окна источника
						BITBLT_FLAG_NONE, keycolor
						);
			}
			else if (tpos >= 4)
			{
				colpip_copyrotate(
					(uintptr_t) layer0, GXSIZE(DIM_X, DIM_Y) * sizeof layer0 [0],
					layer0, DIM_X, DIM_Y,
					x + 1, y + 1,	// получатель Позиция
					(uintptr_t) fgpic, GXSIZE(picx, picy) * sizeof fgpic [0],
					fgpic, picx, picy,	// буфер источника
					0, 0,	// координаты окна источника
					picx, picy, // размер окна источника
					tpos == 4,	// X mirror flag
					tpos == 5,	// Y mirror flag
					0 * tpos	// positive CCW angle
					);

			}
			else
			{
				colpip_copyrotate(
					(uintptr_t) layer0, GXSIZE(DIM_X, DIM_Y) * sizeof layer0 [0],
					layer0, DIM_X, DIM_Y,
					x + 1, y + 1,	// получатель Позиция
					(uintptr_t) fgpic, GXSIZE(picx, picy) * sizeof fgpic [0],
					fgpic, picx, picy,	// буфер источника
					0, 0,	// координаты окна источника
					picx, picy, // размер окна источника
					0,	// X mirror flag
					0,	// Y mirror flag
					90 * tpos	// positive CCW angle
					);

			}
		}


		hardware_ltdc_main_set4((uintptr_t) layer0, (uintptr_t) 0, (uintptr_t) 0, (uintptr_t) 0);
		for (;;)
		{
			testsloopprocessing();		// обработка отложенного вызова user mode функций
		}
	}
#endif
#if 0 && CPUSTYLE_T113
	{
		//	#define DSP0_IRAM_BASE 			((uintptr_t) 0x00028000)			/* 32KB */
		//	#define DSP0_DRAM_BASE 			((uintptr_t) 0x00030000)			/* 32KB */
		// При 0 видим память DSP
		// При 1 видим память что была при загрузке
		// 0: DSP 128K Local SRAM Remap for DSP_SYS
		// 1: DSP 128K Local SRAM Remap for System Boot
		// After system boots up, this bit must be set to 0 before using DSP
		PRINTF("SYS_CFG->DSP_BOOT_RAMMAP_REG=%08" PRIX32 "\n", SYS_CFG->DSP_BOOT_RAMMAP_REG);
		SYS_CFG->DSP_BOOT_RAMMAP_REG = 1;
		PRINTF("SYS_CFG->DSP_BOOT_RAMMAP_REG=%08" PRIX32 "\n", SYS_CFG->DSP_BOOT_RAMMAP_REG);

		uint8_t * const irambase = (void *) DSP0_IRAM_BASE;
		TP();
		irambase [0] = 0xDE;
		irambase [1] = 0xAD;
		irambase [2] = 0xBE;
		irambase [3] = 0xEF;
		printhex(DSP0_IRAM_BASE, irambase, 64);

		uint8_t * const drambase = (void *) DSP0_DRAM_BASE;
		TP();
		drambase [0] = 0xAB;
		drambase [1] = 0xBA;
		drambase [2] = 0x19;
		drambase [3] = 0x80;
		printhex(DSP0_DRAM_BASE, drambase, 64);
		TP();
		PRINTF("allwnr_t113_get_dsp_freq()=%" PRIuFAST32 "\n", allwnr_t113_get_dsp_freq());

//		CCU->DSP_BGR_REG |= UINT32_C(1) << 18;	// DSP_DBG_RST 1: De-assert
//		CCU->DSP_BGR_REG |= UINT32_C(1) << 17;	// DSP_CFG_RST 1: De-assert
//		CCU->DSP_BGR_REG |= UINT32_C(1) << 16;	// DSP_RST 1: De-assert
//		CCU->DSP_BGR_REG |= UINT32_C(1) << 1;	// DSP_CFG_GATING 1: Pass
	}
#endif
#if 0 && (CPUSTYLE_T113 || CPUSTYLE_F133)
	{
		PRINTF("allwnr_t113_get_pll_cpu_freq()=%" PRIuFAST64 "\n", allwnr_t113_get_pll_cpu_freq());
		PRINTF("allwnr_t113_get_pll_ddr_freq()=%" PRIuFAST64 "\n", allwnr_t113_get_pll_ddr_freq());
		PRINTF("allwnr_t113_get_g2d_freq()=%" PRIuFAST32 "\n", allwnr_t113_get_g2d_freq());
		PRINTF("allwnr_t113_get_de_freq()=%" PRIuFAST32 "\n", allwnr_t113_get_de_freq());
		PRINTF("allwnr_t113_get_ce_freq()=%" PRIuFAST32 "\n", allwnr_t113_get_ce_freq());
		PRINTF("allwnr_t113_get_ve_freq()=%" PRIuFAST32 "\n", allwnr_t113_get_ve_freq());
		PRINTF("allwnr_t113_get_di_freq()=%" PRIuFAST32 "\n", allwnr_t113_get_di_freq());
	}
#endif
#if 0 && defined (CLINT) && CPUSTYLE_F133
	{
		TP();
		//csr_set_bits_mcounteren(MCOUNTEREN_CY_BIT_MASK | MCOUNTEREN_TM_BIT_MASK | MCOUNTEREN_IR_BIT_MASK);

		PRINTF("mtimer_get_raw_time_cmp = 0x%" PRIX64 "\n", mtimer_get_raw_time_cmp());
		PRINTF("mtimer_get_raw_time_cmp = %" PRIu64 "\n", mtimer_get_raw_time_cmp());

		mtimer_set_raw_time_cmp(1000);
	    //mtimer_set_raw_time_cmp(mtimer_get_raw_time() + 24000000);
		PRINTF("mtimer_get_raw_time_cmp = 0x%" PRIX64 "\n", mtimer_get_raw_time_cmp());
		PRINTF("mtimer_get_raw_time_cmp = %" PRIu64 "\n", mtimer_get_raw_time_cmp());

		mtimer_set_raw_time_cmp(0x12345678DEADBEEF);
		PRINTF("mtimer_get_raw_time_cmp = 0x%" PRIX64 "\n", mtimer_get_raw_time_cmp());
		PRINTF("mtimer_get_raw_time_cmp = %" PRIu64 "\n", mtimer_get_raw_time_cmp());

		// https://chromitem-soc.readthedocs.io/en/latest/clint.html
		PRINTF("mtimecmp=0x%08" PRIX32 "%08" PRIX32"\n",  CLINT->mtimecmphi,  CLINT->mtimecmplo);

	    mtimer_set_raw_time_cmp(csr_read_mcycle() + 20ll * CPU_FREQ);
		PRINTF("mtimer_get_raw_time_cmp = 0x%" PRIX64 "\n", mtimer_get_raw_time_cmp());
		PRINTF("mtimer_get_raw_time_cmp = %" PRIu64 "\n", mtimer_get_raw_time_cmp());
		TP();
		const uintptr_t a = CLINT_BASE + 0xB000;
		PRINTF("mtimecmp=0x%08" PRIX32 "%08" PRIX32"\n",  CLINT->mtimecmphi,  CLINT->mtimecmplo);
		//printhex32(a, (void *) a, 0x10000);
		// https://chromitem-soc.readthedocs.io/en/latest/clint.html
		PRINTF("mtimecmp=0x%08" PRIX32 "%08" PRIX32"\n",  CLINT->mtimecmphi,  CLINT->mtimecmplo);
		//PRINTF("mtime=0x%08" PRIX32 "%08" PRIX32 "\n",  CLINT->mtimehi,  CLINT->mtimelo);
	}
#endif
#if 0 && (CPUSTYLE_T113 || CPUSTYLE_F133)
	{
		/* Allwinner G2D tests */
		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
		TP();

		display2_fillbg(db);

		/* Запуск теста одного из */

		void g2d_main_layers_alpha(void);
		void g2d_main0(void);

		//g2d_main0();
		g2d_main_layers_alpha();
	}
#endif
#if 0 && LCDMODE_COLORED && ! DSTYLE_G_DUMMY
	{
		const COLORPIP_T bg = display2_getbgcolor();
		PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();

		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
//		TP();
//		colpip_fillrect(buffer, DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, 0xFF00);
//		TP();
//		colpip_fillrect(buffer, DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, 0xFF00);
//		TP();
//		colpip_fillrect(buffer, DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, 0xFF00);
//		for (;;)
//			;

		for (int pos = 0; pos < 24; ++ pos)
		{
			COLORPIP_T c = TFTALPHA(255, UINT32_C(1) << pos);
			colpip_fillrect(buffer, DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, c);
			PRINTF("color=%08X pos=%d\n", (unsigned) c, pos);
			local_delay_ms(2000);
		}
		for (;;)
			;

	}
#endif
#if 0 && WITHDEBUG
	{
		PTT_INITIALIZE();
		for (;;)
		{
			unsigned ptt = (PTT_TARGET_PIN & PTT_BIT_PTT) == 0;
			unsigned ptt2 = (PTT2_TARGET_PIN & PTT2_BIT_PTT) == 0;
			unsigned ptt3 = (PTT3_TARGET_PIN & PTT3_BIT_PTT) == 0;
			PRINTF("ptt=%u ptt2=%u, ptt3=%u\n", ptt, ptt2, ptt3);
		}
	}
#endif
#if 0 && WITHDEBUG && (CPUSTYLE_T113 || CPUSTYLE_F133)
	{
		// Allwinner t113-s3 boot mode display

//		CCU->CE_CLK_REG |= (1uL << 31);	// CE_CLK_GATING
//		CCU->MBUS_MAT_CLK_GATING_REG |= (UINT32_C(1) << 2);	// CE_MCLK_EN
		// bits 27:16: eFUSE boot select status,
		// bit 0: 0: GPIO boot select, 1: eFuse boot select
		// The status of the GPIO boot select pin can be read by the bit[12:11] of the system configuration module (register: 0x03000024).
		PRINTF("SID->BOOT_MODE=0x%08X, SYS_CFG->VER_REG=0x%08X\n", (unsigned) SID->BOOT_MODE, (unsigned) SYS_CFG->VER_REG);
		PRINTF("SID->SID_THS=0x%08X\n", (unsigned) SID->SID_THS);
		PRINTF("BOOT_MODE=%u, BOOT_SEL_PAD_STA=0%u FEL_SEL_PAD_STA=%u\n", (unsigned) (SID->BOOT_MODE >> 0) & 0x01, (unsigned) (SYS_CFG->VER_REG >> 11) & 0x03, (unsigned) (SYS_CFG->VER_REG >> 8) & 0x01);
		//printhex32(SID_BASE, SID, sizeof * SID);
	}
#endif
#if 0 && (CPUSTYLE_T113 || CPUSTYLE_F133)
	{
		RTC->FBOOT_INFO_REG0 = (UINT32_C(1) << 28);	// Try process: SMHC0->SPI NOR->SPI NAND->EMMC2_USER->EMMC2_BOOT.
	}
#endif
#if 0
	{
		// SPI test
		uint8_t b0 [32];
		uint8_t b1 [32];
		memset(b0, 0x00, sizeof b0);
		memset(b1, 0xFF, sizeof b1);
		for (;;)
		{
			prog_spi_io(targetctl1, CTLREG_SPEEDC, SPIC_MODE3, b0, sizeof b0, NULL, 0, NULL, 0);
			local_delay_ms(750);
			prog_spi_io(targetctl1, CTLREG_SPEEDC, SPIC_MODE3, b1, sizeof b1, NULL, 0, NULL, 0);
			local_delay_ms(750);
		}
	}
#endif
#if 0 && (WIHSPIDFSW || WIHSPIDFHW || WIHSPIDFOVERSPI)
	{
		// QSPI test
		unsigned char b [64];

		testchipDATAFLASH();	// устанока кодов опрерации для скоростных режимов

		memset(b, 0xE5, sizeof b);
		readDATAFLASH(0x000000, b, ARRAY_SIZE(b));
		printhex(0, b, ARRAY_SIZE(b));

		testchipDATAFLASH();	// устанока кодов опрерации для скоростных режимов

		memset(b, 0xE5, sizeof b);
		readDATAFLASH(0x00040000, b, ARRAY_SIZE(b));
		printhex(0x00080000, b, ARRAY_SIZE(b));

		PRINTF("Data flash erase start...\n");
		fullEraseDATAFLASH();	// стереть
		PRINTF("Data flash erase done.\n");

		testchipDATAFLASH();	// устанока кодов опрерации для скоростных режимов

		memset(b, 0xE5, sizeof b);
		readDATAFLASH(0x000000, b, ARRAY_SIZE(b));
		printhex(0, b, ARRAY_SIZE(b));

		testchipDATAFLASH();	// устанока кодов опрерации для скоростных режимов

		memset(b, 0xE5, sizeof b);
		readDATAFLASH(0x00040000, b, ARRAY_SIZE(b));
		printhex(0x00080000, b, ARRAY_SIZE(b));

	}
#endif
#if 0 && WITHTWISW && WITHDEBUG
	{
		PRINTF("I2C wires test\n");
		TWISOFT_INITIALIZE();
		for (;;)
		{
			SET_TWCK();
			SET_TWD();
			local_delay_ms(1000);
			CLR_TWCK();
			SET_TWD();
			local_delay_ms(1000);
			SET_TWCK();
			CLR_TWD();
			local_delay_ms(1000);
			CLR_TWCK();
			CLR_TWD();
			local_delay_ms(1000);
		}
	}
#endif
#if 0
	{
		// Test for ADIS16IMU1/PCB
		// SYS_MODE_CURR (Страница 1, адрес 0x36)

		// Wait for module ready
		for (;;)
		{
			const unsigned SYS_E_FLAG = adis161xx_read16(0x00, 0x08);
			const unsigned PROD_ID = adis161xx_read16(0x00, 0x7E);
			if (PROD_ID == 0x4060 && SYS_E_FLAG == 0)
				break;
			PRINTF("SYS_E_FLAG=%04X, Waiting for PROD_ID=0x4060 (%04X)\n", SYS_E_FLAG, PROD_ID);

		}
		for (;;)
		{
			enum { TEMP_BP = 5 };	// binary point of temperature
			const unsigned TEMP_OUT = 0x297B;//adis161xx_read16(0x00, 0x0E);
			const unsigned SYS_E_FLAG = adis161xx_read16(0x00, 0x08);
			const unsigned FIRM_Y = adis161xx_read16(0x03, 0x7C);
			const unsigned FIRM_DM = adis161xx_read16(0x03, 0x7A);
			const unsigned FIRM_REV = adis161xx_read16(0x03, 0x78);
			const unsigned SERIAL_NUM = adis161xx_read16(0x04, 0x20);

			PRINTF("DECLN_ANGL=%04X, SYS_E_FLAG=%04X, FIRM_Y=%04X, FIRM_DM=%04X, FIRM_REV=%04X, SERIAL_NUM=%04X, TEMP_OUT=%d / 10\n", adis161xx_read16(0x03, 0x54), adis161xx_read16(0x00, 0x08), FIRM_Y, FIRM_DM, FIRM_REV, SERIAL_NUM, TEMP_OUT, ((int16_t) TEMP_OUT + (0*250 << TEMP_BP)) >> TEMP_BP);

			for (;;)
			{
				// Получение компонент кватерниона ориентации
				int16_t Q0_C11_OUT =  adis161xx_read16(0x00, 0x60);	// Компонент λ0 кватерниона ориентации
				int16_t Q1_C12_OUT =  adis161xx_read16(0x00, 0x62);	// Компонент λ1 кватерниона ориентации
				int16_t Q2_C13_OUT =  adis161xx_read16(0x00, 0x64);	// Компонент λ2 кватерниона ориентации
				int16_t Q3_C21_OUT =  adis161xx_read16(0x00, 0x66);	// Компонент λ3 кватерниона ориентации

				int16_t ROLL_C23_OUT = adis161xx_read16(0x00, 0x6A);
				int16_t PITCH_C31_OUT = adis161xx_read16(0x00, 0x6C);
				int16_t YAW_C32_OUT = adis161xx_read16(0x00, 0x6E);

				//PRINTF("Q0=%f, Q1=%f, Q2=%f, Q3=%f\n", Q0_C11_OUT / 32768.0f, Q1_C12_OUT / 32768.0f, Q2_C13_OUT / 32768.0f, Q3_C21_OUT / 32768.0f);

				float x = Q0_C11_OUT / 32768.0f;
				float y = Q1_C12_OUT / 32768.0f;
				float z = Q2_C13_OUT / 32768.0f;
				float w = Q3_C21_OUT / 32768.0f;

				// https://coderoad.ru/53033620/%D0%9A%D0%B0%D0%BA-%D0%BF%D1%80%D0%B5%D0%BE%D0%B1%D1%80%D0%B0%D0%B7%D0%BE%D0%B2%D0%B0%D1%82%D1%8C-%D1%83%D0%B3%D0%BB%D1%8B-%D0%AD%D0%B9%D0%BB%D0%B5%D1%80%D0%B0-%D0%B2-%D0%BA%D0%B2%D0%B0%D1%82%D0%B5%D1%80%D0%BD%D0%B8%D0%BE%D0%BD%D1%8B-%D0%B8-%D0%BF%D0%BE%D0%BB%D1%83%D1%87%D0%B8%D1%82%D1%8C-%D1%82%D0%B5-%D0%B6%D0%B5-%D1%83%D0%B3%D0%BB%D1%8B-%D0%AD%D0%B9%D0%BB%D0%B5%D1%80%D0%B0
		        float t0 = +2.0f * (w * x + y * z);
				float t1 = +1.0f - 2.0f * (x * x + y * y);
		        float X = atan2f(t0, t1);

		        float t2;
		        t2 = +2.0f * (w * y - z * x);
		        t2 = t2 > +1.0f ? +1.0f : t2;
		        t2 = t2 < -1.0f ? -1.0f : t2;
		        float Y = asinf(t2);

		        float t3 = +2.0f * (w * z + x * y);
		        float t4 = +1.0f - 2.0f * (y * y + z * z);
		        float Z = atan2f(t3, t4);

		        //PRINTF("X=%f, Y=%f, Z=%f\n", X * (180 / M_PI), Y * (180 / M_PI), Z * (180 / M_PI));
		        PRINTF("HABS=%f, ROLL=%f, PITCH=%f, YAW=%f, BAROM=%f\n",
						(int32_t) adis161xx_read32(0x01, 0x14) / 65536.0f,
		        		ROLL_C23_OUT / 32768.0f * 180.0f,
						PITCH_C31_OUT / 32768.0f * 180.0f,
						YAW_C32_OUT / 32768.0f * 180.0f,
						(int32_t) adis161xx_read32(0x00, 0x2E) / 65536.0f / 25000
						);

			}
			{
				unsigned PG = 0x01;
				//unsigned AE = 0x10;	// LATITUDE_LOW, LATITUDE_OUT
				unsigned AE = 0x24;	// HABS_LOW, HABS_OUT
//				adis161xx_write16(PG, AE, 0xDEAD);
//				adis161xx_write16(PG, AE + 2, 0xBEEF);
				const unsigned LATITUDE_LOW = adis161xx_read16(PG, AE);
				const unsigned LATITUDE_OUT = adis161xx_read16(PG, AE + 2);
				const unsigned LATITUDE4 = adis161xx_read32(PG, AE);
				PRINTF("LOW=%04X, OUT=%04X, 32W=%08X\n", LATITUDE_LOW, LATITUDE_OUT, LATITUDE4);
			}
			unsigned PG = 0x02;
			for (PG = 0; PG < 4; PG += 1)
			{
				unsigned AE;
				for (AE = 0; AE < 128; AE += 4)
				{
					//unsigned AE = 0x10;	// LATITUDE_LOW, LATITUDE_OUT
	//				adis161xx_write16(PG, AE, 0xDEAD);
	//				adis161xx_write16(PG, AE + 2, 0xBEEF);
					const unsigned LATITUDE_LOW = adis161xx_read16(PG, AE);
					const unsigned LATITUDE_OUT = adis161xx_read16(PG, AE + 2);
					const unsigned LATITUDE4 = adis161xx_read32(PG, AE);
					PRINTF("pg=%02X, ae=%02X: LOW=%04X, OUT=%04X, 32W=%08X\n", PG, AE, LATITUDE_LOW, LATITUDE_OUT, LATITUDE4);
				}
			}

//			PRINTF("Write DECLN_ANGL\n");
//			PRINTF("DECLN_ANGL=%04X, SYS_E_FLAG=%04X\n",  adis161xx_read16(0x03, 0x54), adis161xx_read16(0x00, 0x08));
//			adis161xx_write16(0x03, 0x54, 0x0777);
//			PRINTF("DECLN_ANGL=%04X, SYS_E_FLAG=%04X\n",  adis161xx_read16(0x03, 0x54), adis161xx_read16(0x00, 0x08));
//			adis161xx_write16(0x03, 0x54, 0x0888);
//			PRINTF("DECLN_ANGL=%04X, SYS_E_FLAG=%04X\n",  adis161xx_read16(0x03, 0x54), adis161xx_read16(0x00, 0x08));

			char c;
			while (dbg_getchar(& c) == 0)
				;

//			PRINTF("EXT_DATA_SRC #1=%04X\n", adis161xx_read16(0x01, 0x2C));
//			adis161xx_write16(0x01, 0x2C, 0x01);
//			local_delay_ms(10);
//			PRINTF("EXT_DATA_SRC #2=%04X\n", adis161xx_read16(0x01, 0x2C));
//			adis161xx_write16(0x01, 0x2C, 0x03);
//			local_delay_ms(10);
//			PRINTF("EXT_DATA_SRC #3=%04X\n", adis161xx_read16(0x01, 0x2C));

			for (;;)
				;

//
//
//
			local_delay_ms(500);
		}

	}
#endif
#if 0 && WITHDEBUG && WITHSMPSYSTEM
	{
		PRINTF("main: gARM_BASEPRI_ALL_ENABLED=%02X, %02X, %02X, bpr=%02X\n", gARM_BASEPRI_ALL_ENABLED, GIC_ENCODE_PRIORITY(PRI_USER), GIC_GetInterfacePriorityMask(), GIC_GetBinaryPoint());
		enum { TGCPUMASK1 = UINT32_C(1) << 1 };
		enum { TGCPUMASK0 = UINT32_C(1) << 0 };
		const int cpu = arm_hardware_cpuid();

		PRINTF("Main thread test: I am CPU=%d\n", cpu);
		local_delay_ms(100);

		arm_hardware_set_handler(SGI13_IRQn, SecondCPUTaskSGI13, BOARD_SGI_PRIO, UINT32_C(1) << 1);
		arm_hardware_set_handler(SGI15_IRQn, SecondCPUTaskSGI15, BOARD_SGI_PRIO, UINT32_C(1) << 1);

		for (;;)
		{
			// 0: to cpu1 or CPU0 (в зависимости от указанной маски в GIC_SendSGI)
			// 1: to cpu1
			// 2: to cpu0
			//PRINTF("fltr = %d\n", i);
			GIC_SendSGI(SGI15_IRQn, TGCPUMASK1, 0x00);	// CPU1, filer=0
			GIC_SendSGI(SGI13_IRQn, TGCPUMASK1, 0x00);	// CPU1, filer=0
			local_delay_ms(300);
		}
//
//		PRINTF("Main thread test: I am CPU=%d. halt\n", cpu);
//		for (;;)
//			;

	}
#endif
#if 0 && CPUSTYLE_XC7Z
	{
		PRINTF("XDCFG->MCTRL.PS_VERSION=%02lX\n", (XDCFG->MCTRL >> 28) & 0x0F);
	}
#endif
#if 0
	{
		PRINTF("CPU speed changing test:\n");
		stm32mp1_pll1_slow(1);
		TP();
		local_delay_ms(500);
		stm32mp1_pll1_slow(0);
		TP();
		local_delay_ms(500);
		stm32mp1_pll1_slow(1);
		TP();
		local_delay_ms(500);
		stm32mp1_pll1_slow(0);
		TP();
		local_delay_ms(500);
		stm32mp1_pll1_slow(1);
		TP();
		local_delay_ms(500);

		for (;0;)
		{
			/* Обеспечение работы USER MODE DPC */
			uint_fast16_t kbch;
			uint_fast8_t kbready;
			processmessages(& kbch, & kbready);
			char c;
			if (dbg_getchar(& c))
			{
				switch (c)
				{
				case '1':
					stm32mp1_pll1_slow(1);
					break;
				case '0':
					stm32mp1_pll1_slow(0);
					break;
				}
				dbg_putchar(c);
				if (c == 0x1b)
					break;
			}
		}
		PRINTF("CPU speed changing test done.\n");
	}
#endif
#if 0
	{
		// gnu11 tests
		enum e1 { WORKMASK1 = -1 << 7 };
		uint32_t v1 = WORKMASK1;
		enum e2 { WORKMASK2 = -1 << 15 };
		uint32_t v2 = WORKMASK2;
		PRINTF("v1=%08lX, v2=%08lX, %u, %u\n", v1, v2, sizeof (enum e1), sizeof (enum e2));
		for (;;)
			;

	}
#endif
#if 0 && WITHDEBUG
	{
		const time_t t = time(NULL);

		PRINTF("sizeof (time_t) == %u, t = %lu\n", sizeof (time_t), (unsigned long) t);
	}
#endif
#if 1 && defined (__GNUC__)
	{

		PRINTF(PSTR("__GNUC__=%d, __GNUC_MINOR__=%d\n"), (int) __GNUC__, (int) __GNUC_MINOR__);
	}
#endif
#if 0 && (__CORTEX_A != 0)
	{

		PRINTF(PSTR("FPEXC=%08lX\n"), (unsigned long) __get_FPEXC());
		__set_FPEXC(__get_FPEXC() | 0x80000000uL);
		PRINTF(PSTR("FPEXC=%08lX\n"), (unsigned long) __get_FPEXC());
	}
#endif
#if 0 && (__L2C_PRESENT == 1)
	{
		// Renesas: PL310 as a secondary cache. The IP version is r3p2.
		// ZYNQ: RTL release R3p2
		// RTL release 0x8 denotes r3p2 code of the cache controller
		// RTL release 0x9 denotes r3p3 code of the cache controller.
		PRINTF("L2C_310->CACHE_ID=%08lX\n", L2C_GetID());	// L2C_GetID()
		//PRINTF("L2C_310->CACHE_ID Implementer=%02lX\n", (L2C_GetID() >> 24) & 0xFF);
		//PRINTF("L2C_310->CACHE_ID CACHE ID=%02lX\n", (L2C_GetID() >> 10) & 0x3F);
		//PRINTF("L2C_310->CACHE_ID Part number=%02lX\n", (L2C_GetID() >> 6) & 0x0F);
		PRINTF("L2C_310->CACHE_ID RTL release=%02lX\n", (L2C_GetID() >> 0) & 0x3F);

		PRINTF("L2C Data RAM latencies: %08lX\n", * (volatile uint32_t *) ((uintptr_t) L2C_310 + 0x010C)); // reg1_data_ram_control
		PRINTF("L2C Tag RAM latencies: %08lX\n", * (volatile uint32_t *) ((uintptr_t) L2C_310 + 0x0108)); // reg1_tag_ram_control
	}
#endif
#if 0 && defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U)
	{

		#define ICPIDR0	(* (const volatile uint32_t *) (GIC_DISTRIBUTOR_BASE + 0xFE0))
		#define ICPIDR1	(* (const volatile uint32_t *) (GIC_DISTRIBUTOR_BASE + 0xFE4))
		#define ICPIDR2	(* (const volatile uint32_t *) (GIC_DISTRIBUTOR_BASE + 0xFE8))
		// GIC version diagnostics
		// Renesas: ARM GICv1
		//	GICInterface->IIDR=3901043B, GICDistributor->IIDR=0000043B
		// STM32MP1: ARM GICv2
		//	GICInterface->IIDR=0102143B, GICDistributor->IIDR=0100143B
		// ZINQ XC7Z010: ARM GICv1
		//	GICInterface->IIDR=3901243B, GICDistributor->IIDR=0102043B
		// Allwinner T507-H: ARM GICv2
		//	GICInterface->IIDR=0202143B, GICDistributor->IIDR=0200143B

		PRINTF("GICInterface->IIDR=%08X, GICDistributor->IIDR=%08X\n", (unsigned) GIC_GetInterfaceId(), (unsigned) GIC_DistributorImplementer());

		switch (ICPIDR1 & 0x0F)
		{
		case 0x03:	PRINTF("arm_gic_initialize: ARM GICv1\n"); break;
		case 0x04:	PRINTF("arm_gic_initialize: ARM GICv2\n"); break;
		default:	PRINTF("arm_gic_initialize: ARM GICv? (code=%08X @%p)\n", (unsigned) ICPIDR1, & ICPIDR1); break;
		}
	}
#endif /* defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U) */
#if 0 && (__CORTEX_A == 9U) && defined (SCU_CONTROL_BASE)
	{
		// SCU registers dump
		// ZYNQ7000:
		//	SCU Control Register=00000002
		//	SCU Configuration Register=00000501
		//	SCU CPU Power Status Register=03030000
		//	Filtering Start Address Register=00100000
		//	Filtering End Address Register=FFE00000
		//PRINTF("SCU_CONTROL_BASE=%08lX\n", SCU_CONTROL_BASE);
		PRINTF("SCU Control Register=%08lX\n", ((volatile uint32_t *) SCU_CONTROL_BASE) [0]);	// 0x00
		PRINTF("SCU Configuration Register=%08lX\n", ((volatile uint32_t *) SCU_CONTROL_BASE) [1]);	// 0x04
		PRINTF("SCU CPU Power Status Register=%08lX\n", ((volatile uint32_t *) SCU_CONTROL_BASE) [2]);	// 0x08
		PRINTF("Filtering Start Address Register=%08lX\n", ((volatile uint32_t *) SCU_CONTROL_BASE) [0x10]);	// 0x40
		PRINTF("Filtering End Address Register=%08lX\n", ((volatile uint32_t *) SCU_CONTROL_BASE) [0x11]);	// 0x44
	}
#endif
#if 0 && CPUSTYLE_STM32MP1 && WITHDEBUG
	{
		PRINTF("stm32mp1_get_mpuss_freq()=%lu (MPU)\n", stm32mp1_get_mpuss_freq());
		PRINTF("stm32mp1_get_per_freq()=%lu\n", stm32mp1_get_per_freq());
		PRINTF("stm32mp1_get_axiss_freq()=%lu\n", stm32mp1_get_axiss_freq());
		PRINTF("stm32mp1_get_pll2_r_freq()=%lu (DDR3)\n", stm32mp1_get_pll2_r_freq());
	}
#endif
#if 0 && defined (DDRPHYC) && WITHDEBUG && CPUSTYLE_STM32MP1
	{
		// Check DQS Gating System Latency (R0DGSL) and DQS Gating Phase Select (R0DGPS)
		PRINTF("stm32mp1_ddr_init results: DX0DQSTR=%08lX, DX1DQSTR=%08lX, DX2DQSTR=%08lX, DX3DQSTR=%08lX\n",
				DDRPHYC->DX0DQSTR, DDRPHYC->DX1DQSTR,
				DDRPHYC->DX2DQSTR, DDRPHYC->DX3DQSTR);

		// 16 bit single-chip DDR3:
		// PanGu board: stm32mp1_ddr_init results: DX0DQSTR=3DB02001, DX1DQSTR=3DB02001, DX2DQSTR=3DB02000, DX3DQSTR=3DB02000
		// board v2: 	stm32mp1_ddr_init results: DX0DQSTR=3DB03001, DX1DQSTR=3DB03001, DX2DQSTR=3DB02000, DX3DQSTR=3DB02000
		// voard v3: 	stm32mp1_ddr_init results: DX0DQSTR=3DB03001, DX1DQSTR=3DB03001, DX2DQSTR=3DB02000, DX3DQSTR=3DB02000

	}
#endif
#if 0
	#include "dsp3D.h"
	{


		static float32_t dsp3dModel [] = {
				12,3,
				//         VERTEXES
				//   coords    normals
				1,	1, 1, 	1,0,0, // 0
				1, 1,-1, 	0,-1,0, // 1
				1,-1, 1, 	-1,0,0, // 2
				1,-1,-1, 	0,0,-1, // 3
				3,	1, 1, 	1,0,0, // 0
				3, 1,-1, 	0,-1,0, // 1
				3,-1, 1, 	-1,0,0, // 2
				3,-1,-1, 	0,0,-1, // 3
				6,	1, 1, 	1,0,0, // 0
				6, 1,-1, 	0,-1,0, // 1
				6,-1, 1, 	-1,0,0, // 2
				6,-1,-1, 	0,0,-1, // 3
//				3,	1, 1, 	1,0,0, // 0
//				3, 1,-1, 	0,-1,0, // 1
//				3,-1, 1, 	-1,0,0, // 2
//				3,-1,-1, 	0,0,-1, // 3

				//         FACES
				//    Indexes     RGB
				0,1,2,   255,0,255,
				//1,2,3,   255,0,255,
				4,5,6,   0,0,255,
				//5,6,7,   0,0,255,
				8,9,10,   0,255,0,
				//9,10,11,   0,255,0,
		};

//		calcnormaltriangle(dsp3dModel);
//		TP();
//		for (;;)
//			;
		dsp3D_init();
		dsp3D_setCameraPosition(0,0,10);
		dsp3D_setLightPosition(0,0,10);
		dsp3D_setCameraTarget(0,0,0);

		int phase = 0;
		unsigned cnt = 0;
		PRINTF("3d: test started, CPU_FREQ=%lu kHz\n", (unsigned long) (CPU_FREQ / 1000));
		for (;;)
		{
			float * const buf = dsp3dModel + 5;
			float * const buf2 = dsp3dModel + 11;
			float * const buf3 = dsp3dModel + 17;
			float * const buf4 = dsp3dModel + 23;
//			setnormal(buf, phase % 6);
//			setnormal(buf2, (phase / 6) % 6);
//			setnormal(buf3, phase / 36);
			//PRINTF("normal : %d, %d, %d", (int) buf [0], (int) buf [1], (int) buf [2]);
			const time_t start = time(NULL);
			meshRotation[0] = 0;
			meshRotation[1] = 0;
			meshRotation[2] = 0;
			float a;
			for (a = 0; a < 0.1; a += 0.001f)
			{
				meshRotation[0]+=a;
				meshRotation[1]+=a;
				//meshRotation[2]+=a;

				//dsp3D_renderGouraud(dsp3dModel);
				dsp3D_renderFlat(dsp3dModel);
				//dsp3D_renderWireframe(dsp3dModel);
//				char buff [64];
//				snprintf(buff, 64, "normal : %d, %d, %d,", (int) buf [0], (int) buf [1], (int) buf [2]);
//				display_text(20, 10, buff, & dbstylev);
//				snprintf(buff, 64, "normal : %d, %d, %d,", (int) buf2 [0], (int) buf2 [1], (int) buf2 [2]);
//				display_text(20, 15, buff, & dbstylev);
//				snprintf(buff, 64, "normal : %d, %d, %d,", (int) buf3 [0], (int) buf3 [1], (int) buf3 [2]);
//				display_text(20, 20, buff, & dbstylev);
				dsp3D_present();
				local_delay_ms(25);
				char c;
				if (0 && dbg_getchar(& c))
				{
					switch (c)
					{
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
						setnormal(buf, c - '0');
					default:
					case ' ':
						PRINTF("normal : %d, %d, %d,\n", (int) buf [0], (int) buf [1], (int) buf [2]);
						break;
					}
				}

			}
			const time_t end = time(NULL);
			PRINTF("3d: cnt=%u, %d S\n", cnt, (int) (end - start));
			phase = phase + 1;
			if (phase >= (6 * 6 * 6))
				phase = 0;
		}
	}
#endif
#if 0
	{
		unsigned pin;
		PRINTF("zynq pin & bank calculations test.\n");
		for (pin = 0; pin < 118; ++ pin)
		{
			uint_fast8_t Bank;
			uint_fast8_t PinNumber;

			GPIO_BANK_DEFINE(pin, Bank, PinNumber);
			ASSERT(Bank == GPIO_PIN2BANK(pin));
			ASSERT(PinNumber == GPIO_PIN2BITPOS(pin));
		}
		PRINTF("zynq pin & bank calculations test passed.\n");
	}
#endif
#if 0 && CPUSTYLE_STM32MP1
	{
		//	This register is used by the MPU to check the reset source. This register is updated by the
		//	BOOTROM code, after a power-on reset (por_rst), a system reset (nreset), or an exit from
		//	Standby or CStandby.
		PRINTF(PSTR("MP_RSTSCLRR=%08lX\n"), (unsigned long) RCC->MP_RSTSCLRR);
		RCC->MP_RSTSCLRR = RCC->MP_RSTSCLRR;
		PRINTF(PSTR("MP_RSTSCLRR=%08lX\n"), (unsigned long) RCC->MP_RSTSCLRR);
		PRINTF(PSTR("ACTLR=%08lX\n"), (unsigned long) __get_ACTLR());
	}
#endif
#if 0 && defined (TZC) && WITHDEBUG
	{

        const uint_fast8_t lastregion = TZC->BUILD_CONFIG & 0x1f;
        uint_fast8_t i;
        PRINTF("TZC=%p\n", TZC);
        for (i = 0; i <= lastregion; ++ i)
        {
            volatile uint32_t * const REG_ATTRIBUTESx = & TZC->REG_ATTRIBUTESO + (i * 8);
            volatile uint32_t * const REG_ID_ACCESSx = & TZC->REG_ID_ACCESSO + (i * 8);
            volatile uint32_t * const REG_BASE_LOWx = & TZC->REG_BASE_LOWO + (i * 8);
            volatile uint32_t * const REG_BASE_HIGHx = & TZC->REG_BASE_HIGHO + (i * 8);
            volatile uint32_t * const REG_TOP_LOWx = & TZC->REG_TOP_LOWO + (i * 8);
            volatile uint32_t * const REG_TOP_HIGHx = & TZC->REG_TOP_HIGHO + (i * 8);

            PRINTF("TZC->REG_BASE_LOW%d=%08lX ", i, * REG_BASE_LOWx);
            PRINTF("REG_BASE_HIGH%d=%08lX ", i, * REG_BASE_HIGHx);
            PRINTF("REG_TOP_LOW%d=%08lX ", i, * REG_TOP_LOWx);
            PRINTF("REG_TOP_HIGH%d=%08lX ", i, * REG_TOP_HIGHx);
            PRINTF("REG_ATTRIBUTES%d=%08lX ", i, * REG_ATTRIBUTESx);
            PRINTF("REG_ID_ACCESS%d=%08lX\n", i, * REG_ID_ACCESSx);
        }
	}
#endif
#if 0 && WITHDEBUG
	{
		// FPU speed test
		uint_fast8_t state = 0;
#if defined (BOARD_BLINK_INITIALIZE)
		BOARD_BLINK_INITIALIZE();
#elif CPUSTYLE_R7S721
		const uint_fast32_t mask = (1uL << 10);	// P7_10: RXD0: RX DATA line
		arm_hardware_pio7_outputs(mask, mask);
#else /* CPUSTYLE_R7S721 */
		const uint_fast32_t mask = (1uL << 13);	// PA13
		arm_hardware_pioa_outputs(mask, 1 * mask);
#endif /* CPUSTYLE_R7S721 */
		PRINTF("cplxmla @%p, src @%p, dst @%p. refv @%p, CPU_FREQ=%lu MHz\n", cplxmla, src, dst, refv, CPU_FREQ / 1000000uL);
		global_disableIRQ();
		for (;;)
		{
			// stm32mp1 @800 MHz, 16 bit DDR3 @533 MHz
			//	__GNUC__=10, __GNUC_MINOR__=2
			//	cplxmla @C001D174, src @C0CD06C0, dst @C0CC06C0. refv @C0B01A00, CPU_FREQ=792 MHz
			// -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon
			// cplxmla & cplxmlasave: 2.43 kHz
			// -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4
			// cplxmla & cplxmlasave: 2.38 kHz
			// -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=vfpv4
			// cplxmla & cplxmlasave: 1.87 kHz

			// ZYNQ 7000 @666 MHz, 16 bit DDR3 @533 MHz
			//	__GNUC__=10, __GNUC_MINOR__=2
			//	cplxmla @00112798, src @0080DB40, dst @007FDB40. refv @00362E00, CPU_FREQ=666 MHz
			// -mcpu=cortex-a9  -mfloat-abi=hard  -mfpu=vfpv3
			// cplxmla & cplxmlasave: 1.53 kHz
			// -mcpu=cortex-a9  -mfloat-abi=hard  -mfpu=neon-vfpv3
			// cplxmla & cplxmlasave: 1.4 kHz

			// R7S721 @360 MHz
			//	__GNUC__=10, __GNUC_MINOR__=2
			// cplxmla @2001C184, src @2027D780, dst @2026D780. refv @20186B00, CPU_FREQ=360 MHz
			// -mcpu=cortex-a9  -mfloat-abi=hard  -mfpu=vfpv3
			// cplxmla & cplxmlasave: 0.71 kHz
			// -mcpu=cortex-a9  -mfloat-abi=hard  -mfpu=neon-vfpv3
			// cplxmla & cplxmlasave: 0.71 kHz
			// -mcpu=cortex-a9  -mfloat-abi=hard  -mfpu=neon
			// cplxmla & cplxmlasave: 0.71 kHz

			cplxmla(src, dst, refv,  FFTZS);
			cplxmlasave(dst, FFTZS);

			if (state)
			{
				state = 0;
	#if defined (BOARD_BLINK_SETSTATE)
			BOARD_BLINK_SETSTATE(0);
	#elif CPUSTYLE_R7S721
				R7S721_TARGET_PORT_S(7, mask);
	#else /* CPUSTYLE_R7S721 */
				(GPIOA)->BSRR = BSRR_S(mask);
	#endif /* CPUSTYLE_R7S721 */
			}
			else
			{
				state = 1;
		#if defined (BOARD_BLINK_SETSTATE)
				BOARD_BLINK_SETSTATE(1);
		#elif CPUSTYLE_R7S721
				R7S721_TARGET_PORT_C(7, mask);
		#else /* CPUSTYLE_R7S721 */
				(GPIOA)->BSRR = BSRR_C(mask);
		#endif /* CPUSTYLE_R7S721 */
			}
		}

	}
#endif
#if 0
	{
	#if 0
		unsigned k;
		TP();
		for (k = 0; k < 1024; ++ k)
		{
			memfill(k);
		}
		TP();
		for (k = 0; k < 1024; ++ k)
		{
			PRINTF("\r%4d ", k);
			if (!memprobe(k))
				break;
		}
		PRINTF("\n");
		PRINTF("MCU_AHB_SRAM size = %uK\n", k);
		printhex(MCU_AHB_SRAM, (const uint8_t *) MCU_AHB_SRAM, 256);	// /* Cortex-M4 memories */
	#endif
	#if WITHSDRAMHW
		printhex(DRAM_MEM_BASE, (const uint8_t *) DRAM_MEM_BASE, 256);	// DDR3
	#endif /* WITHSDRAMHW */
		//printhex(QSPI_MEM_BASE, (const uint8_t *) QSPI_MEM_BASE, 256);	// QSPI
		//arm_hardware_sdram_initialize();
	}
#endif
#if 0
	{
		PRINTF(PSTR("PLL_FREQ=%lu Hz (%lu MHz)\n"), (unsigned long) PLL_FREQ, (unsigned long) PLL_FREQ / 1000000);
	}
#endif
#if 0
	{
		#define WORKMASK (MFPGA | MSYS | MDSP | MRES)

		arm_hardware_piog_outputs(WORKMASK, 0);

		static RAMNOINIT_D1 FATFSALIGN_BEGIN float Etalon [2048] FATFSALIGN_END;
		static RAMNOINIT_D1 FATFSALIGN_BEGIN float TM [2048] FATFSALIGN_END;
		static RAMNOINIT_D1 FIL WPFile;			/* Описатель открытого файла - нельзя располагать в Cortex-M4 CCM */
		static const char fmname [] = "tstdata.dat";
		static RAMNOINIT_D1 FATFS wave_Fatfs;		/* File system object  - нельзя располагать в Cortex-M4 CCM */
	
		int CountZap=0;
		int LC=2048;
		for (CountZap = 0; CountZap < 5; ++ CountZap)
		{
			GPIOG->ODR ^= WORKMASK;	// Debug LEDs
			hardware_spi_io_delay();
			local_delay_ms(500);
		}
		arm_hardware_piog_outputs(WORKMASK, 0);

		f_mount(& wave_Fatfs, "", 0);
		for (CountZap = 0; CountZap < 1000; ++ CountZap, sdtick())
		{
			PRINTF(PSTR("CountZap %d\n"), CountZap);
			int i;
			FRESULT rc;
			UINT ByteWrite;
			for (i=0;i<LC;i++)
			{
				Etalon[i]=i;
				TM[i]=0;
			}
			rc=f_open(&WPFile, fmname, FA_CREATE_ALWAYS | FA_WRITE);
			if (rc!=FR_OK)
				sdfault();

			rc=f_write (&WPFile, Etalon,LC*4,&ByteWrite );

			if (rc!=FR_OK)
				sdfault();
			rc=f_close (&WPFile);
			if (rc!=FR_OK)
				sdfault();
			rc=f_open(&WPFile, fmname, FA_OPEN_EXISTING | FA_READ);
			if (rc!=FR_OK)
				sdfault();
			rc=f_read (&WPFile, TM,LC*4,&ByteWrite );
			f_sync (&WPFile);
			if (rc!=FR_OK)
				sdfault();
			rc=f_close (&WPFile);
			if (rc!=FR_OK)
				sdfault();
			for (i=0;i<LC;i++)
			{
				if (TM[i]!=Etalon[i])
					sdfault();

			}
		}
		f_mount(NULL, VOLPREFIX "", 0);

		for (;;)
		{
			GPIOG->ODR ^= WORKMASK;	// Debug LEDs
			hardware_spi_io_delay();
			local_delay_ms(500);
		}
	}
#endif
#if 0 && __MPU_PRESENT
	{
		// Cortex Memory Protection Unit (MPU)
		PRINTF(PSTR("MPU=%p\n"), MPU);
		PRINTF(PSTR("MPU->TYPE=%08lX, MPU->CTRL=%08lX\n"), MPU->TYPE, MPU->CTRL);
		const uint_fast8_t n = (MPU->TYPE & MPU_TYPE_DREGION_Msk) >>MPU_TYPE_DREGION_Pos;
		uint_fast8_t i;
		for (i = 0; i < n; ++ i)
		{
			MPU->RNR = i;
			PRINTF(PSTR("MPU->RNR=%08lX, MPU->RBAR=%08lX, MPU->RASR=%08lX "), MPU->RNR, MPU->RBAR, MPU->RASR);
			const uint_fast32_t rasr = MPU->RASR;
			PRINTF(PSTR("XN=%u,AP=%u,TEX=%u,S=%u,C=%u,B=%u,SRD=%u,SIZE=%u,ENABLE=%u\n"),
				((rasr & MPU_RASR_XN_Msk) >> MPU_RASR_XN_Pos),   	// DisableExec
				((rasr & MPU_RASR_AP_Msk) >> MPU_RASR_AP_Pos),   	// AccessPermission
				((rasr & MPU_RASR_TEX_Msk) >> MPU_RASR_TEX_Pos),  	// TypeExtField
				((rasr & MPU_RASR_S_Msk) >> MPU_RASR_S_Pos),    	// IsShareable
				((rasr & MPU_RASR_C_Msk) >> MPU_RASR_C_Pos),    	// IsCacheable
				((rasr & MPU_RASR_B_Msk) >> MPU_RASR_B_Pos),    	// IsBufferable
				((rasr & MPU_RASR_SRD_Msk) >> MPU_RASR_SRD_Pos),  	// SubRegionDisable
				((rasr & MPU_RASR_SIZE_Msk) >> MPU_RASR_SIZE_Pos), 	// Size 512 kB
				((rasr & MPU_RASR_ENABLE_Msk) >> MPU_RASR_ENABLE_Pos)// Enable
				);
		}
	}
#endif
#if 0
	{
		// Сигналы управления HD44780
		for (;;)
		{
			// Установить
			//LCD_STROBE_PORT_S(LCD_STROBE_BIT);
			LCD_RS_PORT_S(ADDRES_BIT);
			//LCD_WE_PORT_S(WRITEE_BIT);
			local_delay_ms(20);

			// Сбросить
			//LCD_STROBE_PORT_C(LCD_STROBE_BIT);
			LCD_RS_PORT_C(ADDRES_BIT);
			//LCD_WE_PORT_C(WRITEE_BIT);
			local_delay_ms(20);
		}
	}
#endif
#if 0
	{
		// вычисления с плавающей точкой
		//
		//				   1.4142135623730950488016887242096981L
		// #define M_SQRT2  1.41421356237309504880

		//original: sqrt(2)=1.41421356237309514547462
		//double:   sqrt(2)=1.41421356237309514547462
		//float:    sqrt(2)=1.41421353816986083984375
		char b [64];
		
		//snprintf(b, sizeof b / sizeof b [0], "%u\n", (unsigned) SCB_GetFPUType());
		//PRINTF(PSTR("SCB_GetFPUType: %s"), b);

		snprintf(b, sizeof b / sizeof b [0], "sqrt(2)=%1.23f\n", (double) 1.41421356237309504880);
		PRINTF(PSTR("original: %s"), b);

		volatile double d0 = 2;
		volatile double d = sqrt(d0);
		snprintf(b, sizeof b / sizeof b [0], "sqrt(2)=%1.23f\n", d);
		PRINTF(PSTR("double:   %s"), b);

		volatile float f0 = 2;
		volatile float f = sqrtf(f0);
		snprintf(b, sizeof b / sizeof b [0], "sqrt(2)=%1.23f\n", f);
		PRINTF(PSTR("float:    %s"), b);
	}
#endif
#if 0 && CTLSTYLE_V1V
	{
		// "прерыватель"

		#define RELAY_PORT PORTD	// выходы процессора - управление трактом ппередачи и манипуляцией
		#define RELAY_DDR DDRD		// переключение на вывод - управление трактом передачи и манипуляцией

		// Управление передатчиком - сигналы TXPATH_ENABLE (PA11) и TXPATH_ENABLE_CW (PA10) - активны при нуле на выходе.
		#define RELAY_BIT		(1uL << PD5)

		RELAY_DDR |= RELAY_BIT;
		unsigned offtime = 100;
		unsigned ontime = 100;
		showstate(offtime, ontime);
		unsigned offt = offtime;
		unsigned ont = ontime;
		unsigned offphase = 0;
		for (;;)
		{
			uint_fast8_t kbch, repeat;

			if ((repeat = kbd_scan(& kbch)) != 0)
			{
				switch (kbch)
				{
				case KBD_CODE_8:
					ontime = modif (ontime, 100, 1, 100);
					break;
				case KBD_CODE_9:
					ontime = modif (ontime, 100, 99, 100);
					break;

				case KBD_CODE_4:
					ontime = modif (ontime, 10, 1, 10);
					break;
				case KBD_CODE_5:
					ontime = modif (ontime, 10, 99, 10);
					break;

				case KBD_CODE_0:
					ontime = modif (ontime, 1, 1, 10);
					break;
				case KBD_CODE_1:
					ontime = modif (ontime, 1, 99, 10);
					break;
				//
				case KBD_CODE_10:
					offtime = modif (offtime, 100, 1, 100);
					break;
				case KBD_CODE_11:
					offtime = modif (offtime, 100, 99, 100);
					break;

				case KBD_CODE_6:
					offtime = modif (offtime, 10, 1, 10);
					break;
				case KBD_CODE_7:
					offtime = modif (offtime, 10, 99, 10);
					break;

				case KBD_CODE_2:
					offtime = modif (offtime, 1, 1, 10);
					break;
				case KBD_CODE_3:
					offtime = modif (offtime, 1, 99, 10);
					break;


				}
				// Update times with new parameters
				offt = offtime;
				ont = ontime;
				showstate(offtime, ontime);
			}
			{
				if (offphase)
				{
					RELAY_PORT &= ~ RELAY_BIT;
					local_delay_ms(10);
					if (-- offt == 0)
					{
						offphase = 0;
						ont = ontime;
						if (ont == 0)
						{
							offphase = 1;
							offt = offtime;
						}
					}
				}
				else
				{
					RELAY_PORT |= RELAY_BIT;
					local_delay_ms(10);
					if (-- ont == 0)
					{
						offphase = 1;
						offt = offtime;
						if (offt == 0)
						{
							offphase = 0;
							ont = ontime;
						}
					}
				}
			}
		}


	}
#endif /* 1 && CTLSTYLE_V1V */
#if 0
	{
		gxstyle_textcolor(& dbstylev, COLOR_GREEN, COLOR_BLACK);
		display_text(5, 0, PSTR("PT-Electronics 2015"), & dbstylev);

		gxstyle_textcolor(& dbstylev, COLOR_RED, COLOR_BLACK);
		display_text(7, 3, PSTR("RENESAS"), & dbstylev);

		gxstyle_textcolor(& dbstylev, COLOR_WHITEALL, COLOR_BLACK);
		display_text(9, 6, PSTR("2.7 inch TFT"), & dbstylev);
		
		for (;;)
			;
	}
#endif
#if 0 && WITHDEBUG
	{
		// тестирование приёма и передачи символов
		PRINTF(PSTR("Serial port ECHO test.\n"));
		for (;;)
		{
			char c;
			if (dbg_getchar(& c))
			{
				dbg_putchar(c);
				if (c == 0x1b)
					break;
			}
		}
}
#endif
#if 0
	// Тест для проверки корректности работы последовательного порта с прерываниями.
	// Должно работать просто "эхо" вводимых символов.
	{
		serial_irq_loopback_test();
	}
#endif
#if 0
	{
		PRINTF(PSTR("FPU tests start.\n"));
		local_delay_ms(300);
		//volatile int a = 10, b = 0;
		//volatile int c = a / b;
		unsigned long i;
		for (i = 0x8000000;; ++ i)
		{
			const double a = i ? i : 1;
			//const int ai = (int) (sin(a) * 1000);
			PRINTF(PSTR("Hello! %lu, sqrt(%lu)=%lu\n"), i, (unsigned) a, (unsigned)( sqrt(a)*10));
		}
	}
#endif
#if 0 && defined (RTC1_TYPE)
	{
		board_rtc_settime(10, 8, 0);
		board_rtc_setdate(2015, 9, 14);
	}
#endif
#if 0 && WITHDEBUG && WITHSDHCHW
	{
		// SD card control lines test
		HARDWARE_SDIOPOWER_INITIALIZE();
		HARDWARE_SDIOPOWER_SET(1);

		arm_hardware_piod_outputs((1uL << 2), 1 * (1uL << 2));	/* PD2 - SDIO_CMD	*/
		arm_hardware_pioc_outputs((1uL << 12), 1 * (1uL << 12));	/* PC12 - SDIO_CK	*/
		arm_hardware_pioc_outputs((1uL << 8), 1 * (1uL << 8));	/* PC8 - SDIO_D0	*/
		arm_hardware_pioc_outputs((1uL << 9), 1 * (1uL << 9));	/* PC9 - SDIO_D1	*/
		arm_hardware_pioc_outputs((1uL << 10), 1 * (1uL << 10));	/* PC10 - SDIO_D2	*/
		arm_hardware_pioc_outputs((1uL << 11), 1 * (1uL << 11));	/* PC11 - SDIO_D3	*/

		int i;
		for (i = 0;; ++ i)
		{
			int f0 = (i & (1 << 0)) != 0;
			int f1 = (i & (1 << 1)) != 0;
			int f2 = (i & (1 << 2)) != 0;
			int f3 = (i & (1 << 3)) != 0;
			int f4 = (i & (1 << 4)) != 0;
			int f5 = (i & (1 << 5)) != 0;

			arm_hardware_piod_outputs((1uL << 2), f0 * (1uL << 2));	/* PD2 - SDIO_CMD	*/
			arm_hardware_pioc_outputs((1uL << 12), f1 * (1uL << 12));	/* PC12 - SDIO_CK	*/
			arm_hardware_pioc_outputs((1uL << 8), f2 * (1uL << 8));	/* PC8 - SDIO_D0	*/
			arm_hardware_pioc_outputs((1uL << 9), f3 * (1uL << 9));	/* PC9 - SDIO_D1	*/
			arm_hardware_pioc_outputs((1uL << 10), f4 * (1uL << 10));	/* PC10 - SDIO_D2	*/
			arm_hardware_pioc_outputs((1uL << 11), f5 * (1uL << 11));	/* PC11 - SDIO_D3	*/

		}
	}
#endif
#if 0 && WITHDEBUG
	{
		PRINTF(PSTR("SD sensors test\n"));
		// SD card sensors test
		HARDWARE_SDIOSENSE_INITIALIZE();
		for (;;)
		{
			PRINTF(PSTR("SD sensors: CD=%d, WP=%d\n"), HARDWARE_SDIOSENSE_CD(), HARDWARE_SDIOSENSE_WP());
			local_delay_ms(50);
		}
	}
#endif
#if 0 && WITHDEBUG && ! WITHISBOOTLOADER
	#if ! (defined (WITHUSESDCARD) && defined (WITHUSEUSBFLASH))
		#error Wrong configuration
	#endif
	{
		// USB file -> eMMC write
		bootloaderFLASH("0:", eMMCtargetdrv);
		PRINTF("Done. Halted.\n");
		for (;;)
			;
	}
#endif
#if 0 && WITHDEBUG && WITHUSEFATFS
	// SD CARD low level functions test
	{
		diskio_test(eMMCtargetdrv);
	}
#endif
#if 0 && WITHDEBUG && WITHUSEFATFS
	// SD CARD FatFs functions test
	{
		fatfs_filesystest(0);
	}
#endif
#if 0 && WITHDEBUG && WITHUSEFATFS
	// SD CARD file system level functions speed test
	// no interactive
	{

		FRESULT rc;
//
//		static const MKFS_PARM defopt = { FM_ANY, 0, 0, 0, 0};	/* Default parameter */
//		defopt.fmt = FM_ANY;	/* Format option (FM_FAT, FM_FAT32, FM_EXFAT and FM_SFD) */
//		defopt.n_fat = 2;		/* Number of FATs */
//		defopt.align = 0;		/* Data area alignment (sector) */
//		defopt.n_root = 128;	/* Number of root directory entries */
//		defopt.au_size = 0;		/* Cluster size (byte) */

		PRINTF("Wait for storage device ready. Press space key\n");
		for (;;)
		{
			char c;
			if (dbg_getchar(& c))
			{
				if (c == 0x1B)
				{
					PRINTF("Skip storage device test\n");
					return;
				}
				if (c == ' ')
					break;
			}
			testsloopprocessing();		// обработка отложенного вызова user mode функций
			//local_delay_ms(5);
		}
		PRINTF("Storage device ready\n");
		unsigned t;
//		for (t = 0; t < 7000; t += 5)
//		{
//			testsloopprocessing();		// обработка отложенного вызова user mode функций
//		}
		static ticker_t test_recordticker;
		IRQL_t oldIrql;
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		ticker_initialize(& test_recordticker, 1, test_recodspool, NULL);	// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
		ticker_add(& test_recordticker);
		LowerIrql(oldIrql);
		{
 			f_mount(NULL, VOLPREFIX "", 0);		/* Unregister volume work area (never fails) */
			rc = f_mkfs("0:", NULL, rbwruff, sizeof (rbwruff));
			if (rc != FR_OK)
			{
				PRINTF(PSTR("sdcardformat: f_mkfs failure, rc=0x%02X\n"), (int) rc);
				return;
			}
			else
			{
				PRINTF(PSTR("sdcardformat: f_mkfs okay\n"));
			}

		}
		for (;;)
		{
			PRINTF(PSTR("Storage device test - %d bytes block.\n"), sizeof rbwruff);
			PRINTF("Storage device test\n");
			if (fatfs_filesyspeedstest())
				break;
			for (t = 0; t < 7000; t += 5)
			{
				testsloopprocessing();		// обработка отложенного вызова user mode функций
		#if WITHUSEAUDIOREC
				sdcardbgprocess();
		#endif /* WITHUSEAUDIOREC */
				//local_delay_ms(5);
			}
		}
		PRINTF("Storage device test done\n");

	}
#endif
#if 1 && (WITHNANDHW || WITHNANDSW)
	// NAND memory test
	// PrimeCell Static Memory Controller (PL353) ARM r2p1
	{
		nand_tests();
	}
#endif
#if 0 && WITHDEBUG && WITHUSEAUDIOREC
	// SD CARD file system level functions test
	{
		static ticker_t test_recordticker;
		IRQL_t oldIrql;
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		ticker_initialize(& test_recordticker, 1, test_recodspool, NULL);	// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
		ticker_add(& test_recordticker);
		LowerIrql(oldIrql);
		fatfs_filesystest(1);
	}
#endif
#if 0 && WITHDEBUG && WITHUSEFATFS
	// Автономный программатор SPI flash memory
	{
		//diskio_test();
		////mmcCardSize();
		////mmcCardSize();
		fatfs_progspi();
	}
#endif
#if 0
	{
		// VFP tests, double
		unsigned long i;
		for (i = 0x8000000;; ++ i)
		{
			const double a = i ? i : 1;
			//const int ai = (int) (sin(a) * 1000);

			PRINTF(PSTR("Hello! %lu, sqrt(%lu)=%lu\n"), i, (unsigned) a, (unsigned) sqrt(a));

		}
	}
#endif
#if 0 && CPUSTYLE_R7S721
	{
		// RZ board leds test
		//i2c_initialize();

		arm_hardware_pio7_outputs(LEDBIT, LEDBIT);	/* ---- P7_1 : LED0 direct connection to IP */
		arm_hardware_pio1_inputs(SW1BIT);	/*  */
		ledsinit();

		for (;;)
		{
			leds(0x01);
			local_delay_ms(100);
			leds(0x02);
			local_delay_ms(100);
			leds(0x04);
			local_delay_ms(100);
	#if 1
			if (GPIO.PPR1 & SW1BIT)
			{
				GPIO.P7 |= LEDBIT;
				leds(1);
			}
			else
			{
				GPIO.P7 &= ~ LEDBIT;
				leds(0);
			}
			continue;
	#endif
			//R_LED_On();
			//GPIO.PNOT7 = LEDBIT;
			LED_TARGET_PORT_S(LEDBIT);	// Led OFF
			__DSB();
			local_delay_ms(1000);
			//R_LED_Off();
			//GPIO.PNOT7 = LEDBIT;
			LED_TARGET_PORT_C(LEDBIT);	// Led ON
			__DSB();
			local_delay_ms(5000);
		}
	}
#endif
#if 0 && ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED
	// note: rdx0154 should be enabled (for I2C functions include).
	{
		test_cpu_pwron(1);
		eink_lcd_backlight(1);
		// M9546 (PCF8576C) support functions
		LCD1x9_Initialize();

		if (0)
		{
			// получение номеров сегментов LCD
			int segment = 0;
			LCD1x9_seg(segment, 1);
			for (;;)
			{

				uint_fast8_t kbch, repeat;

				if ((repeat = kbd_scan(& kbch)) != 0)
				{
					switch (kbch)
					{
					case KBD_CODE_0:
						LCD1x9_seg(segment, 0);
						if (++ segment >= MAXSEGMENT)
							segment = 0;
						LCD1x9_seg(segment, 1);
						PRINTF(PSTR("seg = %d\n"), segment);
						break;

					case KBD_CODE_1:
						LCD1x9_seg(segment, 0);
						if (segment == 0)
							segment = MAXSEGMENT - 1;
						else
							-- segment;
						LCD1x9_seg(segment, 1);
						PRINTF(PSTR("seg = %d\n"), segment);
						break;
					}
				}
			}
		}
		for (;;)
		{

			lcd_outarray(pe2014, sizeof pe2014 / sizeof pe2014 [0]);
			int t;
			for (t = 0; t < 100; ++ t)
			{
				local_delay_ms(50);
				check_poweroff();
			}
			LCD1x9_clear();
			{
				// Зажигаем все сегменты
				uint_fast8_t comIndex;
				for (comIndex = 0; comIndex < 4; ++ comIndex)
				{
					uint_fast8_t bitIndex;
					for (bitIndex = 0; bitIndex < 40; ++ bitIndex)
					{
						check_poweroff();
						LCD1x9_enableSegment(comIndex, bitIndex);
						LCD1x9_Update();
						local_delay_ms(50);
					}
				}
			}
			{
				// Гасим все сегменты
				uint_fast8_t comIndex;
				for (comIndex = 0; comIndex < 4; ++ comIndex)
				{
					uint_fast8_t bitIndex;
					for (bitIndex = 0; bitIndex < 40; ++ bitIndex)
					{
						check_poweroff();
						LCD1x9_disableSegment(comIndex, bitIndex);
						LCD1x9_Update();
						local_delay_ms(50);
					}
				}
			}
		}

		for (;;)
			;
	}
#endif /* ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED */
#if 0 && ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED
	{
		test_cpu_pwron(1);
		// EM027BS013 tests
		eink_initialize();
		for (;;)
			;
	}
#endif /* ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED */
#if 0 && ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED
	{
		// проверка кнопок включения-выключени я питания
		test_cpu_pwron(1);
		for (;;)
		{
			local_delay_ms_spool(1000);
		}
	}
#endif /* ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED */
#if 1 && ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED
	{
		test_cpu_pwron(1);
		//EPD_power_init(EPD_200);

		EPD_display_init();
		for(;;) {
			PRINTF(PSTR("eink: loop start\n"));
			/* User selects which EPD size to run demonstration by changing the
			 * USE_EPD_Type in image_data.h
			 * The Image data arrays for each EPD size are defined at image_data.c */
	#if (USE_EPD_Type==USE_EPD144)
			EPD_display_from_pointer(EPD_144,image_array_144_2,image_array_144_1);
	#elif (USE_EPD_Type==USE_EPD200)
			EPD_display_from_pointer(EPD_200,image_array_200_2,image_array_200_1);
	#elif (USE_EPD_Type==USE_EPD270)
			EPD_display_from_pointer(EPD_270,image_array_270_2,image_array_270_1);
	#endif

			/* The interval of two images alternatively change is 10 seconds */
			//local_delay_ms(10000);
			local_delay_ms_spool(1000);

	#if (USE_EPD_Type==USE_EPD144)
			EPD_display_from_pointer(EPD_144,image_array_144_1,image_array_144_2);
	#elif (USE_EPD_Type==USE_EPD200)
			EPD_display_from_pointer(EPD_200,image_array_200_1,image_array_200_2);
	#elif (USE_EPD_Type==USE_EPD270)
			EPD_display_from_pointer(EPD_270,image_array_270_1,image_array_270_2);
	#endif

			/* The interval of two images alternatively change is 10 seconds */
			//local_delay_ms(10000);
			local_delay_ms_spool(2000);
			PRINTF(PSTR("eink: loop end\n"));
		}
	}
#endif /* ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED */
#if 0
	{
		//hardware_tim21_initialize();
		for (;;)
		{
			PRINTF(PSTR("TIM21 CNT=%08lX\n"), TIM21->CNT);
		}
	}
#endif
#if 0
	{
		// test: initialize TIM2:TIM5
		// TIM5 включён на выход TIM2
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM5EN;   // подаем тактирование на TIM2 & TIM5
		__DSB();
		//TIM3->DIER = TIM_DIER_UIE;        	 // разрешить событие от таймера
		TIM5->PSC = 1;
		TIM2->PSC = 1;

		TIM5->CR1 = TIM_CR1_CEN; /* включить таймер */
		TIM2->CR1 = TIM_CR1_CEN; /* включить таймер */
		for (;;)
		{
			PRINTF(PSTR("TIM2:TIM5 CNT=%08lX:%08lX\n"), TIM2->CNT, TIM5->CNT);
		}
	}
#endif
#if 0 && LCDMODE_COLORED && ! DSTYLE_G_DUMMY
	{

		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
		TP();
		unsigned cnt;
		display2_fillbg(db);
		//disableAllIRQs();
		for (cnt = 0; ; ++ cnt)
		{
			const time_t tstart = time(NULL);
			//GridTest();
			BarTest();
			const time_t tend = time(NULL);
			PRINTF("BarTest: %u, %ds, pixelsize=%d @%u MHz\n", cnt, (int) (tend - tstart), LCDMODE_PIXELSIZE, (unsigned) (CPU_FREQ / 1000000));
		}
		// Divide result by 10
		// 800x480, Renesas RZ/A1L, @360 MHz, L8, software (w cache: 5.6s..5.7s)
		// 800x480, Renesas RZ/A1L, @360 MHz, L8, software (no cache: 0.2s)
		// 800x480, STM32MP157, @650 MHz, L8, software (w cache: 0.8s)
		// 800x480, STM32MP157, @650 MHz, L8, hardware MDMA: (no cache - 0.9s..1s)
		// 800x480, STM32MP157, @650 MHz, RGB565, hardware MDMA: (no cache - 1.4s)
		// 800x480, STM32MP157, @650 MHz, ARGB8888, hardware MDMA: (no cache - 2.5s)
		// 800x480, Allwinner t113-s3, @1200 MHz, RGB565, software 0.6s
		// 800x480, Allwinner F133-A, @1200 MHz, RGB565, hardware G2D 0.7s
		// 800x480, Allwinner F133-A, @1200 MHz, XRGB8888, hardware G2D 0.9s
		// 800x480, Allwinner t507, @1200 MHz, RGB565, software 0.4s
	}
#endif
#if 0 && WITHLTDCHW && LCDMODE_COLORED && ! DSTYLE_G_DUMMY
	{
		// test: вывод палитры на экран
		const unifont_t * const font = & unifont_small3;	// шрифт, используемый при отриосовке надписей на шкале
		display2_fillbg(db);
		PACKEDCOLORPIP_T * const fr = colmain_fb_draw();
		int sepx = 3, sepy = 3;
		int wx = DIM_X / 16;
		int wy = DIM_Y / 16;
		int x = 0, y = 0;

		for (int i = 0; i <= 255; i++)
		{

			colpip_fillrect(x, y, wx - sepx, wy - sepy, i << 4);

			if (wx > 24)
			{
				char buf [4];
				const size_t bifsz = local_snprintf_P(buf, sizeof buf / sizeof buf [0], PSTR("%d"), i);
				unifont_text(fr, DIM_X, DIM_Y, x, y, font, buf, bufsz, COLORPIP_WHITE);
			}

			x = x + wx;
			if ((i + 1) % 16 == 0)
			{
				x = 0;
				y = y + wy;
			}
		}

		colmain_nextfb();
		for (;;)
			;
	}
#endif
#if 0 && defined (TSC1_TYPE)
	/* Тест результата калибровки с рисованием точки касания */
	#include "touch\touch.h"
	{
		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();

		gxdrawb_t dbv;	// framebuffer для выдачи диагностических сообщений
		gxdrawb_initialize(& dbv, colmain_fb_draw(), DIM_X, DIM_Y);

		colpip_fillrect(& dbv, 0, 0, DIM_X, DIM_Y, COLOR_BLACK);
		colpip_string_small(& dbv, DIM_X / 3, DIM_Y / 2, "TEST TSC", COLOR_GREEN);
		colmain_nextfb();
		for (;;)
		{
			uint_fast16_t x, y;
			if (board_tsc_getxy(& x, & y))
			{
				enum { r0 = 15 };
				gxdrawb_t dbv;	// framebuffer для выдачи диагностических сообщений
				char msg [64];

				PRINTF("board_tsc_getxy: x=%-5u, y=%-5u\n", x, y);

				gxdrawb_initialize(& dbv, colmain_fb_draw(), DIM_X, DIM_Y);
				// стереть фон
				colpip_fillrect(& dbv, 0, 0, DIM_X, DIM_Y, COLOR_BLACK);
				colpip_string_small(& dbv, DIM_X / 3, DIM_Y / 2, "TEST TSC", COLOR_GREEN);
				local_snprintf_P(msg, ARRAY_SIZE(msg), PSTR("X=%5d, Y=%5d"), (int) x, (int) y);
				colpip_string_small(& dbv, 0, 0, msg, COLOR_GREEN);
				enum { RSZ = 5 };	// размер метки касания
				if (x < DIM_X - RSZ && y < DIM_Y - RSZ)
					colpip_fillrect(& dbv, x, y, RSZ, RSZ, COLOR_WHITEALL);

				colmain_nextfb();
			}
		}
	}
#endif
#if 0 && WITHDEBUG && defined (TSC1_TYPE)
	/* Тест - печать ненормализованных значений координат */
	{
		for (;;)
		{
			uint_fast16_t x, y, z;
			if (board_tsc_getraw(& x, & y, & z))
			{
				uint_fast16_t xc = board_tsc_normalize_x(x, y, NULL);
				uint_fast16_t yc = board_tsc_normalize_y(x, y, NULL);
				PRINTF("board_tsc_getraw: x=%-5u, y=%-5u , z=%-5u -> xc=%-5u, yc=%-5u\n", x, y, z, xc, yc);
			}
		}
	}
#endif
#if 0 && (CTLSTYLE_V1E || CTLSTYLE_V1F)
	{
		//int n = TIM6_DAC_IRQn;
		unsigned long i = 0;
		gxstyle_textcolor(& dbstylev, COLOR_WHITEALL, COLOR_BLACK);
		for (;;)
		{
			++ i;
			uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
			do
			{
				char buff [22];

				// IF gain
				local_snprintf_P(buff, sizeof buff / sizeof buff [0], 
					PSTR("CNT=%08lX"), i
					 );

				display_gotoxy(0, 0 + lowhalf);
				gxstyle_textcolor(& dbstylev, COLOR_WHITEALL, COLOR_BLACK);
				display_text(buff, lowhalf, & dbstylev);
			} while (lowhalf --);
			PRINTF(PSTR("CNT=%08lX\n"), i);
		}
	}
	{
		// PB8 signal
		enum { WORKMASK = 1ul << 0 };
		arm_hardware_pioa_outputs(WORKMASK, WORKMASK);

		for (;;)
		{
			GPIOA->ODR ^= WORKMASK;	// Debug LEDs
			//hardware_spi_io_delay();
		}
	}
#endif
#if 0 && WITHNMEA
	// NMEA test
	{
		for (;;)
		{
			char buff [32];
			local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("%02d:%02d:%02d,ky=%d"), 
				th.hours, th.minutes, th.seconds, 
				HARDWARE_NMEA_GET_KEYDOWN());

			uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
			do
			{
				display_gotoxy(0, 0 + lowhalf);

				display_text(buff, lowhalf, & dbstylev);
			} while (lowhalf --);
		}
	}
#endif
#if 0 && WITHDEBUG
	// NVRAM test
	{
		PRINTF(PSTR("NVRAM test started...\n"));
		nvram_initialize();
		unsigned char i = 0;
		for (;; ++ i)
		{
			save_i8(10, 0x55);
			save_i8(20, 0xaa);
			save_i8(30, i);
			const uint_fast8_t v1 = restore_i8(10);
			const uint_fast8_t v2 = restore_i8(20);
			const uint_fast8_t v3 = restore_i8(30);
			PRINTF(PSTR("v1=%02x, v2=%02x, v3=%02x (expected=%02x)\n"), v1, v2, v3, i);
			local_delay_ms(50);
		}
		for (;;)
			;
	}
#endif

#if 0 && defined (RTC1_TYPE) && WITHDEBUG

	/* RTC test */
	{
		////board_rtc_initialize(); // already done
		//board_rtc_settime(22, 48, 30);
		//board_rtc_setdate(2015, 5, 3);
		
		for (;;)
		{
			uint_fast16_t year;
			uint_fast8_t month, day;
			uint_fast8_t hour, minute, seconds;

			//board_rtc_getdate(& year, & month, & day);
			//board_rtc_gettime(& hour, & minute, & seconds);

			//PRINTF(PSTR("%04d-%02d-%02d "), year, month, day);
			//PRINTF(PSTR("%02d:%02d:%02d "), hour, minute, seconds);

			board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & seconds);

			PRINTF(PSTR("%04d-%02d-%02d "), year, month, day);
			PRINTF(PSTR("%02d:%02d:%02d\n"), hour, minute, seconds);

			local_delay_ms(1250);
			
		}
	}
#endif
#if 0 && WITHDEBUG
	/* ADC test */
	{

		for (;;)
		{
			uint_fast8_t i;
			for (i = 0; i < 8; ++ i)
			{
				//const uint_fast8_t i = AVOXIX; //KI2;
				PRINTF(PSTR("ADC%d=%3d "), i, board_getadc_unfiltered_u8(i, 0, 255));
				//PRINTF(PSTR("ADC%d=%3d "), i, board_getadc_unfiltered_1 /* true */value(i));
			}
			PRINTF(PSTR("\n"));
		}
	}
#endif
#if 0 && WITHDEBUG
	/* ADC test on screen*/
	{
		// see HARDWARE_ADCINPUTS 
		enum { NADC = 8, COLWIDTH = 8 };
		unsigned long cnt = 0;
		for (;; ++ cnt)
		{
			uint_fast8_t i;
			for (i = 0; i < NADC; ++ i)
			{
				char buff [32];

				local_snprintf_P(buff, sizeof buff / sizeof buff [0],
					PSTR("V%d=%4d"), i, board_getadc_unfiltered_1 /* true */value(i));
				display_text(COLWIDTH * (i % 2), i / 2, buff, & dbstylev);
			}
			if (0)
			{
				char buff [32];
				local_snprintf_P(buff, sizeof buff / sizeof buff [0],
					PSTR("CNT=%08lX"), cnt); 
				display_text(8 * (i % 2), i / 2, buff, & dbstylev);
			}
		}
	}
#endif
#if 0 && WITHDEBUG
	// вечный цикл
	for (;;)
	{
		i2c_start(0xaa);// Si570: адрес 0x55, запись = 0
		i2c_write(135);
		i2c_write(0x80);	// RST_REG = 1 - выключает генерацию на выходе Si570
		i2c_waitsend();
		i2c_stop();
			
		_delay_ms(20);
	}
#endif
#if 0 && WITHTX && WITHVOX && WITHDEBUG
	// Отображение значений с выхода DSP модуля - уровень VOX
	{
		updateboard();	/* полная перенастройка (как после смены режима) - режим приема */
		updateboard2();			/* настройки валкодера и цветовой схемы дисплея. */
		for (;;)
		{
			//unsigned dsp_getmikev(void);
			//unsigned vox1 = dsp_getmikev();
			unsigned vox2 = board_getvox();
			unsigned avox = board_getavox();
			uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
			do
			{
				char buff [22];

				//////////////////////////
				// VOX level
				local_snprintf_P(buff, sizeof buff / sizeof buff [0], 
					PSTR("vox2=%3d"), vox2
					 );
				display_gotoxy(0, 0 + lowhalf);
				display_text(buff, lowhalf, & dbstylev);

				//////////////////////////
				// VOX2 level
				/*
				local_snprintf_P(buff, sizeof buff / sizeof buff [0], 
					PSTR("vox1=%5d"), vox1
					 );
				display_gotoxy(0, 1 + lowhalf);
				display_text(buff, lowhalf, & dbstylev);
				*/
				//////////////////////////
				// A-VOX level
				local_snprintf_P(buff, sizeof buff / sizeof buff [0], 
					PSTR("avox=%3d"), avox
					 );
				display_gotoxy(11, 0 + lowhalf);
				display_text(buff, lowhalf, & dbstylev);

			} while (lowhalf --);
		}
	}
#endif
#if 0 && WITHDEBUG
	TP();
	// Трансивер с DSPIF4 "Вороненок-DSP"
	// Отображение значений с дополнительных входов АЦП
	for (;;)
	{
//		if (! display_refreshenabled_wpm())
//			continue;
		// подтверждаем, что обновление выполнено
//		display_refreshperformed_wpm();

		//const unsigned potrf = board_getadc_filtered_u8(POTIFGAIN, 0, UINT8_MAX);
		const unsigned potrft = board_getadc_unfiltered_1 /* true */value(POTIFGAIN);
		const unsigned potrf = board_getadc_unfiltered_1 /* true */value(POTIFGAIN);

		//const unsigned potaf = board_getadc_smoothed_u8(POTAFGAIN, BOARD_AFGAIN_MIN, BOARD_AFGAIN_MAX);
		const unsigned potaft = board_getadc_unfiltered_1 /* true */value(POTAFGAIN);
		const unsigned potaf = board_getadc_unfiltered_1 /* true */value(POTAFGAIN);

		//const unsigned aux1 = board_getadc_filtered_u8(POTAUX1, 0, UINT8_MAX);
		//const unsigned aux2 = board_getadc_filtered_u8(POTAUX2, 0, UINT8_MAX);
		//const unsigned aux3 = board_getadc_filtered_u8(POTAUX3, 0, UINT8_MAX);
#if WITHPOTWPM
		const unsigned wpm = board_getpot_filtered_u8(POTWPM, 0, UINT8_MAX);
#endif /* WITHPOTWPM */

		PRINTF("potrft=%u potaft=%u\n", potrf, potaft);
		continue;
		char buff [22];

#if 1
		// сокращённый вариант отображения
		// AF gain
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("af= %4d"), potaf
			 );
		display_text(db, 0, 0 * HALFCOUNT_SMALL, buff, & dbstylev);
		// AF gain raw
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("aft=%4d"), potaft
			 );
		display_text(db, 0, 1 * HALFCOUNT_SMALL, buff, & dbstylev);
		continue;
#else
		// IF gain
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("rf= %4d"), potrf
			 );
		display_text(db, 0, 0 * HALFCOUNT_SMALL, buff, & dbstylev);
		// AF gain
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("af= %4d"), potaf
			 );
		display_text(db, 0, 1 * HALFCOUNT_SMALL, buff, & dbstylev);

		// AUX1
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("A1= %4d"), aux1
			 );
		display_gotoxy(14, 0 + lowhalf);
		display_text(buff, lowhalf, & dbstylev);

		// AUX2
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("A2= %4d"), aux2
			 );
		display_gotoxy(0, 1 + lowhalf);
		display_text(buff, lowhalf, & dbstylev);

		// AUX3
		/*
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("A3=%3d"), aux3
			 );
		display_gotoxy(7, 1 + lowhalf);
		display_text(buff, lowhalf, & dbstylev);
		*/
#if WITHPOTWPM
		// WPM
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("cw=%3d"), wpm
			 );
		display_gotoxy(14, 1 + lowhalf);
		display_text(buff, lowhalf, & dbstylev);
#endif /* WITHPOTWPM */

		// IF gain raw
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("rft=%4d"), potrft
			 );
		display_gotoxy(0, 2 + lowhalf);
		display_text(buff, lowhalf, & dbstylev);
		// AF gain raw
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("aft=%4d"), potaft
			 );
		display_gotoxy(10, 2 + lowhalf);
		display_text(buff, lowhalf, & dbstylev);
#endif


	}
#endif
#if 0 && WITHDEBUG
	// тест дисплея - вывод меняющихся цифр
	{
		unsigned long i = 0;
		gxstyle_textcolor(& dbstylev, COLOR_WHITEALL, COLOR_BLACK);
		for (;;)
		{
			++ i;
			uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
			do
			{
				char buff [22];

				// IF gain
				local_snprintf_P(buff, sizeof buff / sizeof buff [0], 
					PSTR("CNT=%08lX"), i
					 );

				display_gotoxy(0, 0 + lowhalf);
				gxstyle_textcolor(& dbstylev, COLOR_WHITEALL, COLOR_BLACK);
				display_text(buff, lowhalf, & dbstylev);
			} while (lowhalf --);
		}
	}
#endif
#if 0 && LCDMODE_COLORED
	board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
	board_update();
	gxstyle_t dbstylev;
	gxstyle_initialize(& dbstylev);
	// тест интерфейса дисплея - цветов RGB565
	for (;;)
	{
		// Palette parameters
		//enum { rSkip = 3, gSkip = 2, bSkip = 3 };
		enum { rSkip = 0, gSkip = 0, bSkip = 0 };
		char b [32];
		int c;
		if (0)
		{
			for (;;)
			{
				{
					// Solid BLACK
					c = UINT8_C(0);
					display2_setbgcolor(TFTRGB(c, c, c));
					display2_fillbg(db);
					colmain_nextfb();
					local_delay_ms(1000);
				}
				{
					// Solid WHITE
					c = UINT8_C(0xFF);
					display2_setbgcolor(TFTRGB(c, c, c));
					display2_fillbg(db);
					colmain_nextfb();
					local_delay_ms(1000);
				}

			}
		}
		if (0)
		{
			for (c = 0; c < 256; ++ c)
			{
				display2_setbgcolor(TFTRGB(c, c, c));
				display2_fillbg(db);
				local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("WHITE %-3d"), c);
				gxstyle_textcolor(& dbstylev, COLOR_WHITEALL, COLOR_BLACK);
				display_text(db, 0, 0, b, & dbstylev);
				colmain_nextfb();
				local_delay_ms(50);
			}
			for (; -- c > 0; )
			{
				display2_setbgcolor(TFTRGB(c, c, c));
				display2_fillbg(db);
				local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("WHITE %-3d"), c);
				gxstyle_textcolor(& dbstylev, COLOR_WHITEALL, COLOR_BLACK);
				display_text(db, 0, 0, b, & dbstylev);
				colmain_nextfb();
				local_delay_ms(50);
			}
			continue;
		}
		if (0)
		{
			for (c = 0; c < 8; ++ c)
			{
				display2_setbgcolor(TFTRGB(UINT8_C(1) << c, UINT8_C(1) << c, UINT8_C(1) << c));
				display2_fillbg(db);
				local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("X%d"), c);
				gxstyle_textcolor(& dbstylev, COLOR_WHITEALL, COLOR_BLACK);
				display_text(db, 0, 0, b, & dbstylev);
				colmain_nextfb();
				local_delay_ms(2000);
			}
			continue;
		}
		for (c = 0; c < (8 - rSkip); ++ c)
		{
			display2_setbgcolor(TFTRGB(UINT8_C(1) << (c + rSkip), 0, 0));
			display2_fillbg(db);
			local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("R%d"), c + rSkip);
			gxstyle_textcolor(& dbstylev, COLOR_WHITEALL, COLOR_BLACK);
			display_text(db, 0, 0, b, & dbstylev);
			colmain_nextfb();
			local_delay_ms(2000);
		}
		for (c = 0; c < (8 - gSkip); ++ c)
		{
			display2_setbgcolor(TFTRGB(0, UINT8_C(1) << (c + gSkip), 0));
			display2_fillbg(db);
			local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("G%d"), c + gSkip);
			gxstyle_textcolor(& dbstylev, COLOR_WHITEALL, COLOR_BLACK);
			display_text(db, 0, 0, b, & dbstylev);
			colmain_nextfb();
			local_delay_ms(2000);
		}
		for (c = 0; c < (8 - bSkip); ++ c)
		{
			display2_setbgcolor(TFTRGB(0, 0, UINT8_C(1) << (c + bSkip)));
			display2_fillbg(db);
			local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("B%d"), c + bSkip);
			gxstyle_textcolor(& dbstylev, COLOR_WHITEALL, COLOR_BLACK);
			display_text(db, 0, 0, b, & dbstylev);
			colmain_nextfb();
			local_delay_ms(2000);
		}
	}
#endif
#if 0
	board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
	board_update();
	// тест дисплея - проход по всем возможным уровням основных цветов
	for (;;)
	{
		char b [32];
		int c;
		// COLORPIP_T bg
		for (c = 0; c < 256; ++ c)
		{
			display2_setbgcolor(TFTRGB(c, c, c));
			display2_fillbg(db);
			local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("WHITE %-3d"), c);
			gxstyle_textcolor(& dbstylev, COLOR_WHITEALL, COLOR_BLACK);
			display_text(db, 0, 0, b, & dbstylev);
			colmain_nextfb();
			local_delay_ms(50);
		}
		for (c = 0; c < 256; ++ c)
		{
			display2_setbgcolor(TFTRGB(c, 0, 0));
			display2_fillbg(db);
			local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("RED %-3d"), c);
			gxstyle_textcolor(& dbstylev, COLOR_WHITEALL, COLOR_BLACK);
			display_text(db, 0, 0, b, & dbstylev);
			colmain_nextfb();
			local_delay_ms(50);
		}
		for (c = 0; c < 256; ++ c)
		{
			display2_setbgcolor(TFTRGB(0, c, 0));
			display2_fillbg(db);
			local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("GREEN %-3d"), c);
			gxstyle_textcolor(& dbstylev, COLOR_WHITEALL, COLOR_BLACK);
			display_text(db, 0, 0, b, & dbstylev);
			colmain_nextfb();
			local_delay_ms(50);
		}
		for (c = 0; c < 256; ++ c)
		{
			display2_setbgcolor(TFTRGB(0, 0, c));
			display2_fillbg(db);
			local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("BLUE %-3d"), c);
			gxstyle_textcolor(& dbstylev, COLOR_WHITEALL, COLOR_BLACK);
			display_text(db, 0, 0, b, & dbstylev);
			colmain_nextfb();
			local_delay_ms(50);
		}
	}
#endif
#if 0 && WITHDEBUG
	{
		//synth_lo1_setfreq(99000000, getlo1div(tx));
		//for (;;)
		//{
		//}


		unsigned long if1 = 73050000ul;
		unsigned long freq;
		for (freq = 0; freq < (56000000 + 50000); freq += 1)
		//for (freq = 28000000; freq < (29700000); freq += 1000)
		{
			synth_lo1_setfreq(freq + if1, getlo1div(tx));
			///*
			if ((freq % 1000) == 0)
			{
				static int cd;
				char buff [22];
				//unsigned char v1, v2;
				display_gotoxy(0, 4);
				local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("%10lu"), freq );
				display_text(buff, 0, & dbstylev);
				ITM_SendChar('a' + (cd ++) % 16);
				//SWO_PrintChar('a' + (cd ++) % 16);

			}
			//*/


		}
		for (;;)
		{
		}

	}

#endif
#if 0
	{
		static char buff [22];
		unsigned char v1, v2;
		//v1 = si570_get_status();
		display_gotoxy(0, 6);
		local_snprintf_P(buff, 22, PSTR("he %08lX:%08lX"), 0x1726354aul, -7ul);
		display_text(buff, 0, & dbstylev);

		display_gotoxy(0, 7);
		//unsigned long l1 = (unsigned long) (rftw >> 32);
		//unsigned long l2 = (unsigned long) rftw;
		local_snprintf_P(buff, 22, PSTR("he %08lx"), -4000000l);
		display_text(buff, 0, & dbstylev);
		for (;;)
			;
	}
#endif
#if 0
	{
		// Проверка, откуда идут помехи - от I2C или от SPI.
		for (;;)
		{

			uint_fast8_t kbch, repeat;

			if ((repeat = kbd_scan(& kbch)) != 0)
			{
				int i;
				switch (kbch)
				{
				case KBD_CODE_BAND_UP:
					// проверка индикатора
					for (i = 0; i < 20000; ++ i)
					{
						char s [21];
						local_snprintf_P(s, sizeof s / sizeof s [0], PSTR("%08lx    "), (unsigned long) i);
						display_menu_label(s);
					}
					break;
				case KBD_CODE_BAND_DOWN:
					// проверка SPI
					for (i = 0; i < 10000; ++ i)
					{
						synth_lo1_setfreq(i, getlo1div(tx));
					}
					break;
				}
			}
		}
	}
#endif
#if 0
	// Тестирование скорости передачи по SPI. На SCK должна быть частота SPISPEED
	for (;;)
	{
		const spitarget_t cs = SPI_CSEL4;
		spi_select(cs, SPIC_MODE3);
		spi_progval8_p1(cs, 0xff);
		//for (;;)
			spi_progval8_p2(cs, 0x55);
			spi_progval8_p2(cs, 0x55);
			spi_progval8_p2(cs, 0x55);
			spi_progval8_p2(cs, 0x55);
			spi_progval8_p2(cs, 0x55);
			spi_progval8_p2(cs, 0x55);
			spi_progval8_p2(cs, 0x55);
			spi_progval8_p2(cs, 0x55);
			spi_progval8_p2(cs, 0x55);
		spi_complete(cs);
		spi_unselect(cs);
	}
#endif
#if 0
	{
		unsigned phase = 0;
		// тестирование входов манипуляции, ptt, ключа и CAT
		PRINTF("ptt, elkey, cat dtr/rts testing.\n");
		for (;;)
		{
			uint_fast8_t kbch, repeat;

			if ((repeat = kbd_scan(& kbch)) != 0)
			{
				//display_dispfreq(kbch * 1000UL + v);
				//v = (v + 10) % 1000;
			}
			unsigned tune1 = hardware_get_tune();
			unsigned ptt1 = hardware_get_ptt();
			unsigned ptt2 = HARDWARE_CAT_GET_RTS();
			unsigned elkey = hardware_elkey_getpaddle(0);
			unsigned ckey = HARDWARE_CAT_GET_DTR();


			PRINTF("tune=%u, ptt=%u, ptt2=%u, elkey=%u\n", tune1, ptt1, ptt2, elkey);
			continue;

			display_text(db, 0, 0, ptt1 != 0 ? "ptt " : "    ", & dbstylev);
			display_text(db, 0, 2, ptt2 != 0 ? "cptt " : "     ", & dbstylev);
			display_text(db, 0, 4, ckey != 0 ? "ckey " : "     ", & dbstylev);

			display_text(db, 0, 6, (elkey & ELKEY_PADDLE_DIT) != 0 ? "dit " : "     ", & dbstylev);
			display_text(db, 0, 8, (elkey & ELKEY_PADDLE_DASH) != 0 ? "dash" : "      ", & dbstylev);
			display_text(db, 0, 10, (phase = ! phase) ? " test1" : " test2", & dbstylev);

		}
	}
#endif
#if 0
	{
		// тестирование валкодера в режиме "интеллектуального ускорения"
		// На индикаторе отображается скорость в оборотах в секунду
		for (;;)
		{
			unsigned speed;
			int nrotate = encoderA_get_snapshot(& speed, 1);
			uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
			do
			{
				display_gotoxy(0, 0 + lowhalf);
				display2_menu_value(speed * 100UL / ENCODER_NORMALIZED_RESOLUTION, 7, 2, 0, lowhalf);
				display_gotoxy(0, HALFCOUNT_SMALL + lowhalf);
				display2_menu_value(nrotate, 5 | WSIGNFLAG, 255, 0, lowhalf);
				local_delay_ms(100);
			} while (lowhalf --);
			//(void) nrotate;
			//display_debug_digit(speed * 100UL / ENCODER_NORMALIZED_RESOLUTION, 7, 2, 0);
			//enum { lowhalf = 0 };
			//display_gotoxy(0, 1 + lowhalf);		// курсор в начало второй строки
		}
	}
#endif
#if 0
	{
		// тестирование валкодера в режиме "интеллектуального ускорения"
		// На индикаторе отображается скорость в оборотах в секунду
		for (;;)
		{
			uint_fast8_t jumpsize;
			int_least16_t nrotate = encoder_getrotatehires(& encoder1, & jumpsize);
			(void) nrotate;
			//display_gotoxy(0, 1);		// курсор в начало второй строки
			display_debug_digit(jumpsize, 7, 0, 0);
		}
	}
#endif
#if 0
	// Показ в одной строке трех значений с АЦП клавиатуры
	for (;;)
	{
		uint_fast8_t row;
		uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;

		do
		{
			display_gotoxy(0, 0 + lowhalf);
			display2_menu_value(board_getadc_unfiltered_u8(KI0, 0, UINT8_MAX), 5, 255, 0, lowhalf);
			display_gotoxy(5, 0 + lowhalf);
			display2_menu_value(board_getadc_unfiltered_u8(KI1, 0, UINT8_MAX), 5, 255, 0, lowhalf);
			display_gotoxy(10, 0 + lowhalf);
			display2_menu_value(board_getadc_unfiltered_u8(KI2, 0, UINT8_MAX), 5, 255, 0, lowhalf);
		} while (lowhalf --);
		//
		local_delay_ms(20);
	}
#endif

#if 0
	for (;;)
	{
		uint_fast8_t row;
		for (row = 0; row < 8; ++ row)
		{
			//
			const uint_fast8_t v0 = board_getadc_unfiltered_u8(row, 0, UINT8_MAX); 
			uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;

			do
			{
				display_gotoxy(0, row * HALFCOUNT_SMALL + lowhalf);		// курсор в начало второй строки
				display_text("ADCx=", lowhalf, & dbstylev);
				display_gotoxy(5, row * HALFCOUNT_SMALL + lowhalf);		// курсор в начало второй строки
				display2_menu_value(v0, 5, 255, 0, lowhalf);
			} while (lowhalf --);
		}
		//
		local_delay_ms(20);
	}
#endif

#if 0
	{
		PRINTF(PSTR("kbd_test2:\n"));
		// kbd_test2
		// Показ условных номеров клавиш для создания новых матриц перекодировки
		// и тестирования работоспособности клавиатуры.
		enum { menuset = 0 };
		int v = 0;
		for (;;)
		{
			//PRINTF(PSTR("keyport = %02x\n"), (unsigned) KBD_TARGET_PIN);
			//continue;
			
			uint_fast16_t scancode;
			IRQL_t oldIrql;
			RiseIrql(IRQL_SYSTEM, & oldIrql);
			scancode = board_get_pressed_key();
			LowerIrql(oldIrql);

			if (scancode != KEYBOARD_NOKEY)
			{
				PRINTF(PSTR("keycode = 0x%02X (%u), %d\n"), (unsigned) scancode, (unsigned) scancode, v);
				v = (v + 1) % 1000;
			}
		}
	}
#endif

#if 0
	{
		// kbd_test1
		// показ кодов клавиш
		enum { menuset = 0 };
		int v = 0;
		PRINTF(PSTR("kbd_test1:\n"));
		for (;;)
		{
			uint_fast8_t kbch, repeat;

			if ((repeat = kbd_scan(& kbch)) != 0)
			{
				PRINTF(PSTR("kbch = %02x\n"), (unsigned) kbch);
				continue;
				display2_dispfreq_a2(kbch * 1000UL + v, 255, 0, menuset);
				v = (v + 10) % 1000;
			}
		}
	}
#endif

#if 0
	// test SPI lines (ATT functions)
	{
		int i = 0;
		for (;;)
		{
			board_set_att(i);
			board_update();		/* вывести забуферированные изменения в регистры */
			i = (i + 1) % ATTMODE_COUNT;
			local_delay_ms(500);
		}
	}
#endif

#if 0
	// test SPI lines (band selection hardware)
	{
		int i = 0;
		for (;;)
		{
			board_set_bandf(i);
			board_update();		/* вывести забуферированные изменения в регистры */
			i = (i + 1) % 16;
			local_delay_ms(500);
		}
	}
#endif

#if 0
	// test relays (preapm)
	{
		for (;;)
		{
			int pre;
			for (pre = 0; pre < PAMPMODE_COUNT; ++ pre)
			{
				board_set_tx(0);	
				board_set_bandf(1);	
				board_set_preamp(pre);
				board_update();		/* вывести забуферированные изменения в регистры */
				local_delay_ms(500);
			}
		}
	}
#endif

#if 0
	// test relays (attenuators, preapm, antenna switch)
	{
		for (;;)
		{
			int att;
			for (att = 0; att < ATTMODE_COUNT; ++ att)
			{
				int pre;
				for (pre = 0; pre < PAMPMODE_COUNT; ++ pre)
				{
					int ant;
					for (ant = 0; ant < ANTMODE_COUNT; ++ ant)
					{
						int bandf;
						for (bandf = 0; bandf < 8; ++ bandf)
						{
							board_set_att(att);
							board_set_antenna(ant);
							board_set_bandf(bandf);	
							board_set_bandf2(bandf);
							board_set_bandf3(bandf);
							board_set_preamp(pre);
							board_update();		/* вывести забуферированные изменения в регистры */
							local_delay_ms(100);
						}
					}
				}
			}
		}
	}
#endif

#if 0
	// тестирование сигналов выбора VFO
	{
		for (;;)
		{
			uint_fast8_t i;
			for (i = 0; i < HYBRID_NVFOS; ++ i)
			{
				board_set_lo1vco(i);
				board_update();		/* вывести забуферированные изменения в регистры */
				local_delay_ms(500);
			}
		}
	}
#endif

#if 0
	enum { ADDR = 0x88 };
	// Test I/O exoanders at STM3210C-EVAL
	// Chip name: STMPE811
	// chip addresses: 0x82 & 0x88
	i2c_start(ADDR | 0x00);
	i2c_write(0x04);	// register to write: 0x17 - GPIO select gpio mode bits
	i2c_write(0x0b);	// value to write: 0x17 - GPIO I/O
	i2c_waitsend();
	i2c_stop();

	i2c_start(ADDR | 0x00);
	i2c_write(0x17);	// register to write: 0x17 - GPIO select gpio mode bits
	i2c_write(0xff);	// value to write: 0x17 - GPIO I/O
	i2c_waitsend();
	i2c_stop();

	#if 0
		// чтение всех регшистров
		for (;;)
		{
			int i;
			for (i = 0; i < 0x63; ++ i)
			{
				uint_fast8_t v1 = 0xaa;

				i2c_start(ADDR | 0x00);
				i2c_write_withrestart(i);	// register to read: 0x12 - GPIO monitor pin state register
				i2c_start(ADDR | 0x01);
				i2c_read(& v1, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */
				
				static const char fmt_1 [] = "%02X";
				char buff [17];
				local_snprintf_P(buff, 17, fmt_1, v1);

				display_gotoxy((i % 8) * 3, (i / 8) * 2);
				display_text(buff, 0, & dbstylev);
			}
		}
	#endif

	for (;;)
	{

		enum { ADDR = 0x88 };
		uint_fast8_t v1 = 0xaa;

		i2c_start(ADDR | 0x00);
		i2c_write_withrestart(0x12);	// register to read: 0x12 - GPIO monitor pin state register
		i2c_start(ADDR | 0x01);
		i2c_read(& v1, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */


		static const char fmt_1 [] = "v=%02X";
		char buff [17];
		local_snprintf_P(buff, 17, fmt_1, v1);
		display_gotoxy(0, 0);
		display_text(buff, 0, & dbstylev);
		local_delay_ms(100);
	}

#endif

#if 0
	// PCF8535 (tic154) address (r/w): 
	//	0x79/0x78, 0x7b/0x7a, 0x7d/0x7c, 0x7f/0x7e
	// PCF8574 (bus expander) address (r/w): 
	//	0x41/0x40, 0x43/0x42, 0x45/0x45, 0x47/0x46, 
	//	0x49/0x48, 0x4b/0x4a, 0x4d/0x4c, 0x4f/0x4e, 
	//
	// Test I2C interface
	i2c_start(0x4e);
	i2c_write(0xff);	// все биты на ввод
	i2c_waitsend();
	i2c_stop();

	for (;;)
	{
		i2c_start(0x4f);
		unsigned char v = i2c_readAck();
		//unsigned char v = i2c_readNak();
		//i2c_waitsend();
		i2c_stop();
		static const char fmt_1 [] = "v=%02X";
		char buff [17];
		local_snprintf_P(buff, 17, fmt_1, v);
		display_gotoxy(0, 0);
		display_text(buff, & dbstylev);
		local_delay_ms(100);
	}
#endif

#if 0
	// test SPI lines (output attenuator bits)
	display_dispfreq(v * 1000UL);
	for (;;)
	{
		int v = 0;
		uint_fast8_t kbch, repeat;

		if ((repeat = kbd_scan(& kbch)) != 0)
		{
			v = (v + 1) % 4;
			display_dispfreq(v * 1000UL);
			board_set_att(v);
			board_update();		/* вывести забуферированные изменения в регистры */
#if 1
			double x = v;
			double v = sin(x);	// проверка работы библиотечных функций с плавающей точкой
			uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;

			do
			{
				display_gotoxy(0, 1 + lowhalf);		// курсор в начало первой строки
				display2_menu_value(v * 1000ul, 7, 2, 1, lowhalf);
			} while (lowhalf --);
#endif
		}
	}
#endif

#if 0
	{
		unsigned long v = 0;
		//i2c_initialize();
		//display_initialize();
		for (;;)
		{	
			static const char fmt_1 [] = "%08lX";
			char buff [17];

			local_snprintf_P(buff, 17, fmt_1, v ++);
			display_gotoxy(0, 0);
			display_text(buff, 0, & dbstylev);
			
		}
	}
#endif

#if 0
	{
		uint_fast8_t pos = 0;
		board_cat_set_speed(9600);
		while (dbg_putchar(pos | 0xf0) == 0)
			;
		showpos(pos);
		for (;;)
		{
			uint_fast8_t kbch, repeat;

			local_delay_ms(500);
			pos = calc_next(pos, 0, 15);
			while (dbg_putchar(pos | 0xf0) == 0)
				continue;			
			;
			showpos(pos);
			continue;

			if ((repeat = kbd_scan(& kbch)) != 0)
			{
				switch (kbch)
				{
				case KBD_CODE_BAND_UP:
					pos = calc_next(pos, 0, 15);
					while (dbg_putchar(pos | 0xf0) == 0)
						;
					break;

				case KBD_CODE_BAND_DOWN:
					pos = calc_prev(pos, 0, 15);
					while (dbg_putchar(pos | 0xf0) == 0)
						;
					break;

				default:
					continue;
				}
				showpos(pos);
			}
			else
			{
				//while (dbg_putchar(pos | 0xf0) == 0)
				//	;
			}

		}
	}
#endif
#if 0 && (WITHDSPEXTTXFIR || WITHDSPEXTRXFIR)
	{
		// Тестирование сигналов управления загрузкой параметров фильтров FPGA

		unsigned seq;

		TARGET_FPGA_FIR_INITIALIZE();

		for (seq = 0;; ++ seq)
		{
			if (seq & 0x01)
				TARGET_FPGA_FIR_CS_PORT_S(TARGET_FPGA_FIR_CS_BIT);
			else
				TARGET_FPGA_FIR_CS_PORT_C(TARGET_FPGA_FIR_CS_BIT);

			if (seq & 0x02)
				TARGET_FPGA_FIR1_WE_PORT_S(TARGET_FPGA_FIR1_WE_BIT);
			else
				TARGET_FPGA_FIR1_WE_PORT_C(TARGET_FPGA_FIR1_WE_BIT);

			if (seq & 0x04)
				TARGET_FPGA_FIR2_WE_PORT_S(TARGET_FPGA_FIR2_WE_BIT);
			else
				TARGET_FPGA_FIR2_WE_PORT_C(TARGET_FPGA_FIR2_WE_BIT);

			local_delay_ms(250);
		}
	}
#endif
#if 0
	TP();
	for (;;)
	{
		SPI_CS_ASSERT(targetnvram);
		local_delay_ms(700);
		SPI_CS_DEASSERT(targetnvram);
		local_delay_ms(700);
		TP();
	}
#endif

#if WITHLTDCHW && LCDMODE_LTDC
	colmain_nextfb();	// Скрыть результаты теста, разнести рисуемый и ообрадаемый буферы
#endif /* WITHLTDCHW && LCDMODE_LTDC */
}

#if WITHRTOS

#include "FreeRTOS.h"
#include "task.h"

#define STACK_SIZE 8129
static TaskHandle_t task_blinky_handle;
static TaskHandle_t task_blinky_handle2;
static TaskHandle_t task_blinky_handle3;
static TaskHandle_t task_gr_handle;

//static QueueHandle_t kbd_queue;

static void task_blinky(void *arg)
{
	(void)arg;
	uint32_t state = 1;

	PRINTF("blinky1\n");

	while (1)
	{
		PRINTF("!");
		vTaskDelay(NTICKS(750));
	}
}

static void task_blinky2(void *arg)
{
	int state = 0;
	(void)arg;

#ifdef BOARD_BLINK_INITIALIZE
	BOARD_BLINK_INITIALIZE();
#endif /* BOARD_BLINK_INITIALIZE */
	while (1)
	{
		//PRINTF("@");
#ifdef BOARD_BLINK_INITIALIZE
		BOARD_BLINK_SETSTATE(state);
#endif /* BOARD_BLINK_INITIALIZE */
		state = ! state;
		vTaskDelay(NTICKS(500));
	}
}

static void task_blinky3(void *arg)
{
	int state = 0;
	(void)arg;

	while (1)
	{
		PRINTF("@");
		vTaskDelay(NTICKS(1500));
	}
}

//void vApplicationFPUSafeIRQHandler( uint32_t ulICCIAR )
void vApplicationIRQHandler( uint32_t ulICCIAR )
{
	const IRQn_ID_t int_id = ulICCIAR & 0x3FFUL;

	// IHI0048B_b_gic_architecture_specification.pdf
	// See ARM IHI 0048B.b 3.4.2 Special interrupt numbers when a GIC supports interrupt grouping

	if (int_id == 1022)
	{
	}

	if (int_id >= 1020)
	{
		//dbg_putchar('2');
		//LCLSPIN_LOCK(& giclock);
		//GIC_SetPriority(0, GIC_GetPriority(0));	// GICD_IPRIORITYRn(0) = GICD_IPRIORITYRn(0);
		//GICDistributor->IPRIORITYR [0] = GICDistributor->IPRIORITYR [0];
		GIC_SetPriority(0, GIC_GetPriority(0));
		//LCLSPIN_UNLOCK(& giclock);

	}
	else if (int_id != 0 /* || GIC_GetIRQStatus(0) != 0 */)
	{
		const IRQHandler_t f = IRQ_GetHandler(int_id);


		if (f != (IRQHandler_t) 0)
		{
			(* f)();	    /* Call interrupt handler */
		}


		//dbg_putchar('5');
	}
	else
	{
		//dbg_putchar('3');
		//LCLSPIN_LOCK(& giclock);
		//GIC_SetPriority(0, GIC_GetPriority(0));	// GICD_IPRIORITYRn(0) = GICD_IPRIORITYRn(0);
		//GICDistributor->IPRIORITYR [0] = GICDistributor->IPRIORITYR [0];
		GIC_SetPriority(0, GIC_GetPriority(0));
		//LCLSPIN_UNLOCK(& giclock);
	}
}


void vApplicationMallocFailedHook(void)
{
	PRINTF("malloc failed\n");

	for (;;)
		;
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
	(void)xTask;

	PRINTF("task stack overflow %s\n", pcTaskName);

	for (;;)
		;
}

void vApplicationIdleHook( void )
{
//	uart_printf("k\n");
}

#if configSUPPORT_STATIC_ALLOCATION

static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
//uart_printf("idle get task memory %x %x %x\n", *ppxIdleTaskTCBBuffer, *ppxIdleTaskStackBuffer, *pulIdleTaskStackSize);
}

static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
//	uart_printf("timer get task memory %x %x %x\n", *ppxTimerTaskTCBBuffer, *ppxTimerTaskStackBuffer, *pulTimerTaskStackSize);
}

#endif /* configSUPPORT_STATIC_ALLOCATION */

void task_init(void *arg)
{
	(void)arg;

	//syscall init
	//syscalls_init();

	//kbd_queue = xQueueCreate(10, sizeof(kbd_event_t));

	BaseType_t ret = xTaskCreate(task_blinky, "led1", STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &task_blinky_handle);
	if (ret != pdTRUE){
		PRINTF("1 not created\n");
		for (;;)
			;
	}

	BaseType_t ret2 = xTaskCreate(task_blinky2, "led2", STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &task_blinky_handle2);
	if (ret2 != pdTRUE){
		PRINTF("2 not created\n");
		for (;;)
			;
	}

	BaseType_t ret3 = xTaskCreate(task_blinky3, "led3", STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &task_blinky_handle3);
	if (ret3 != pdTRUE){
		PRINTF("3 not created\n");
		for (;;)
			;
	}
//
//	ret = xTaskCreate(task_gr, "gr", 1000, NULL, tskIDLE_PRIORITY+2, &task_gr_handle);
//	if (ret != pdTRUE){
//		PRINTF("not created\n");
//		while(1);
//	}
//
//	usb_task_init();

	vTaskDelete(NULL);
}

/* PL1 Physical Timer */
#if (__CORTEX_A == 7U) && (__TIM_PRESENT == 1U)

static volatile uint_fast32_t gtimloadvalue;

void vConfigureTickInterrupt(void)
{
	const uint_fast32_t f = CPU_PL1_FREQ;
	const IRQn_ID_t irqn = SecurePhysicalTimer_IRQn;
	/* Stop Timer */
	PL1_SetControl(0x0);
	PL1_SetCounterFrequency(f);
	gtimloadvalue = f / configTICK_RATE_HZ;
	/* Initialize Counter */
	PL1_SetLoadValue(gtimloadvalue);

	/* Disable corresponding IRQ */
	IRQ_Disable(irqn);
	IRQ_ClearPending(irqn);
	IRQ_SetHandler(irqn, FreeRTOS_Tick_Handler);

	/* Set timer priority to lowest (Only bit 7:3 are implemented in MP1 CA7 GIC) */
	/* TickPriority is based on 16 level priority (from MCUs) so set it in 7:4 and leave bit 3=0 */
//	if (TickPriority < (1UL << 4)) {
//		IRQ_SetPriority(irqn, TickPriority << 4);
//		uwTickPrio = TickPriority;
//	} else {
//		return HAL_ERROR;
//	}
	IRQ_SetPriority(irqn, portLOWEST_USABLE_INTERRUPT_PRIORITY << portPRIORITY_SHIFT);
	/* todo: Use other SMP-related flags */
	/* Set edge-triggered IRQ */
	IRQ_SetMode(irqn, IRQ_MODE_TRIG_EDGE_RISING);

	/* Enable corresponding interrupt */
	IRQ_Enable(irqn);

	/* Kick start Timer */
	PL1_SetControl(0x1);
}

void vClearTickInterrupt(void)
{
	PL1_SetLoadValue(gtimloadvalue);
}

#elif (__CORTEX_A == 5U) || (__CORTEX_A == 9U)
// Private timer use
// Disable Private Timer and set load value
void vConfigureTickInterrupt(void)
{
	PTIM_SetControl(0);
	PTIM_SetCurrentValue(0);
	PTIM_SetLoadValue(calcdivround2(CPU_FREQ, ticksfreq * 2));	// Private Timer runs with the system frequency / 2
	// Set bits: IRQ enable and Auto reload
	PTIM_SetControl(0x06U);

	arm_hardware_set_handler(PrivTimer_IRQn, PTIM_Handler, portLOWEST_USABLE_INTERRUPT_PRIORITY << portPRIORITY_SHIFT, UINT32_C(1) << 0);

	// Start the Private Timer
	PTIM_SetControl(PTIM_GetControl() | 0x01);
}

void vClearTickInterrupt(void)
{
	PTIM_ClearEventFlag();
}

#endif

#elif ! LINUX_SUBSYSTEM //#else /* WITHRTOS */

void __WEAK FreeRTOS_SWI_Handler(void)
{
	ASSERT(0);
}

void __WEAK FreeRTOS_IRQ_Handler(void)
{
	ASSERT(0);
}

#endif /* WITHRTOS */

// Вызывается перед инициализацией NVRAM, но после инициализации SPI
void midtests(void)
{
#if 0
	{
		PRINTF("SPI speed test.\n");
		uint8_t buff [64];
		memset(buff, 0xFF, sizeof buff);
		//__disable_irq();
		for (;;)
		{
			//prog_spi_io(targetnvram, SPIC_SPEEDUFAST, SPIC_MODE3, buff, sizeof buff, NULL, 0, NULL, 0); // 24 M
			//prog_spi_io(targetnvram, SPIC_SPEEDFAST, SPIC_MODE3, buff, sizeof buff, NULL, 0, NULL, 0); // 12 M
			prog_spi_io(targetnvram, SPIC_SPEED400k, SPIC_MODE3, buff, sizeof buff, NULL, 0, NULL, 0); // 400 kHz
			//prog_spi_io(targetnvram, SPIC_SPEED1M, SPIC_MODE3, buff, sizeof buff, NULL, 0, NULL, 0); // 1 M
			//prog_spi_io(targetnvram, SPIC_SPEED4M, SPIC_MODE3, buff, sizeof buff, NULL, 0, NULL, 0); // 4 M
			//prog_spi_io(targetnvram, SPIC_SPEED10M, SPIC_MODE3, buff, sizeof buff, NULL, 0, NULL, 0); // 10 M
		}
	}
#endif
#if WITHRTOS
	{
		InitializeIrql(0xffU);
		  __enable_irq();
		// now we switch to freertos
		BaseType_t ret = xTaskCreate(task_init, "init", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL);
		if (ret != pdTRUE){
			PRINTF("not created\n");
			while(1);
		}

		PRINTF("starting scheduler\n");
		vTaskStartScheduler();

		for (;;)
			;

	}
#endif /* WITHRTOS */
#if 0 && CPUSTYLE_R7S721
	// тестирование скорости после инициализации MMU
	{
		const uint32_t mask = (1uL << 13);	// P6_13
		// R7S721 pins
		arm_hardware_pio6_outputs(mask, mask);
		for (;;)
		{
			R7S721_TARGET_PORT_S(6, mask);	/* P6_13=1 */
			local_delay_ms(10);
			R7S721_TARGET_PORT_C(6, mask);	/* P6_13=0 */
			local_delay_ms(10);
		}
	}
#endif
	//hardware_cw_diagnostics(0, 0, 1);	// 'U'
#if 0
	// тестирование управляемого по SPI регистра
	{
		for (;;)
		{
			const spitarget_t target = targetctl1;
			//
			spi_select(target, SPIC_MODE3);
			spi_progval8_p1(target, 0xff);
			spi_progval8_p2(target, 0xff);
			spi_progval8_p2(target, 0xff);
			spi_progval8_p2(target, 0xff);
			spi_complete(target);
			spi_unselect(target);
			local_delay_ms(500);
			//
			spi_select(target, SPIC_MODE3);
			spi_progval8_p1(target, 0x00);
			spi_progval8_p2(target, 0x00);
			spi_progval8_p2(target, 0x00);
			spi_progval8_p2(target, 0x00);
			spi_complete(target);
			spi_unselect(target);
			local_delay_ms(500);
		}

	}
#endif
}

#if STM32F0XX_MD
void hardware_f051_dac_initialize(void)		/* инициализация DAC на STM32F4xx */
{
	RCC->APB1ENR |= RCC_APB1ENR_DACEN; //подать тактирование
	__DSB();

	DAC1->CR = DAC_CR_EN1;
}
// вывод 12-битного значения на ЦАП - канал 1
void hardware_f051_dac_ch1_setvalue(uint_fast16_t v)
{
	DAC1->DHR12R1 = v;
}
#endif /* STM32F0XX_MD */


// Сразу после начала main

#if 0 && WITHLVGL && LINUX_SUBSYSTEM

#include "linux/linux_subsystem.h"
#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lvgl/demos/lv_demos.h"
#include "lvgl/examples/lv_examples.h"
#include <linux/kd.h>


#define DISP_BUF_SIZE	(128 * DIM_X)

pthread_t lv_tick_inc_t;

void thread_lv_tick_inc(void)
{
	while(1) {
		lv_tick_inc(1);
		usleep(1000);
	}
}

void thread_spool_encinterrupt(void)
{
	while(1)
	{
		indev_enc2_spool();
		spool_encinterrupt2();
		usleep(1000);
	}
}

#endif

#if 0 && LINUX_SUBSYSTEM
void signal_handler(int n, siginfo_t *info, void *unused)
{
	PRINTF("received value %d\n", info->si_int);
}
#endif

//#include "buffers.h"

void lowtests(void)
{
#if 0
	// cached memory tests
	{
		PRINTF("Cached memory test - dcache row size test (should see partially modified buffer):\n");
		static ALIGNX_BEGIN uint8_t buff [128];

		global_disableIRQ();

		memset(buff, 0xE5, sizeof buff);
		dcache_clean((uintptr_t) buff, sizeof buff);

		memset(buff, 0x00, sizeof buff);
		dcache_clean((uintptr_t) buff, 1);	// one byte - force one cache row clear
		dcache_invalidate((uintptr_t) buff, sizeof buff);

		printhex((uintptr_t) buff, buff, sizeof buff);
//		for (;;)
//			;
	}
	// cached memory tests
	{
		static const char msg [] = "May not see this text";
		PRINTF("Cached memory test (may not see text):\n");
		static ALIGNX_BEGIN uint8_t buff [128];

		global_disableIRQ();

		memset(buff, 0xE5, sizeof buff);
		dcache_clean((uintptr_t) buff, sizeof buff);

		memcpy(buff + DCACHEROWSIZE, msg, sizeof msg);
		dcache_invalidate((uintptr_t) buff, sizeof buff);

		printhex((uintptr_t) buff, buff, sizeof buff);
//		for (;;)
//			;
	}
	{
		static const char msg [] = "Should see this text";
		PRINTF("Non-cached memory test (should see text):\n");
		static RAMNC ALIGNX_BEGIN uint8_t buff [128];

		global_disableIRQ();

		memset(buff, 0xE5, sizeof buff);
		dcache_clean((uintptr_t) buff, sizeof buff);

		memcpy(buff + DCACHEROWSIZE, msg, sizeof msg);
		dcache_invalidate((uintptr_t) buff, sizeof buff);

		printhex((uintptr_t) buff, buff, sizeof buff);
		for (;;)
			;
	}
#endif
#if 0 && LINUX_SUBSYSTEM
	struct sigaction sig;
	sig.sa_sigaction = signal_handler;
	sig.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &sig, NULL);

	const char * argv [3] = { "/sbin/modprobe", "inttest", NULL, };
	linux_run_shell_cmd(argv);
	sleep(1);

	int fs = open("/dev/inttest", O_RDONLY);
	ASSERT(fs > 0);
	int pid = getpid();
	ioctl(fs, 0, pid);
	close(fs);

	for(;;) {}
#endif

#if 0 && WITHLVGL && LINUX_SUBSYSTEM

	int ttyd = open(LINUX_TTY_FILE, O_RDWR);
	if (ttyd)
		ioctl(ttyd, KDSETMODE, KD_GRAPHICS);

	close(ttyd);

	/*LVGL init*/
	lv_init();

	/*Linux frame buffer device init*/
	fbdev_init();

	/*A small buffer for LittlevGL to draw the screen's content*/
	static lv_color_t buf1[DISP_BUF_SIZE];
	static lv_color_t buf2[DISP_BUF_SIZE];

	/*Initialize a descriptor for the buffer*/
	static lv_disp_draw_buf_t disp_buf;
	lv_disp_draw_buf_init(&disp_buf, buf1, buf2, DISP_BUF_SIZE);

	/*Initialize and register a display driver*/
	static lv_disp_drv_t disp_drv;
	lv_disp_drv_init(& disp_drv);
	disp_drv.draw_buf   = & disp_buf;
	disp_drv.flush_cb   = fbdev_flush;
	disp_drv.hor_res    = DIM_X;
	disp_drv.ver_res    = DIM_Y;
	lv_disp_drv_register(& disp_drv);

	i2c_initialize();
	board_tsc_initialize();

	/* Set up touchpad input device interface */
	lv_indev_drv_t touch_drv;
	lv_indev_drv_init(& touch_drv);
	touch_drv.type = LV_INDEV_TYPE_POINTER;
	touch_drv.read_cb = board_tsc_indev_read;
	lv_indev_drv_register(& touch_drv);

	/* Encoder register */
	lv_indev_drv_t enc_drv;
	lv_indev_drv_init(& enc_drv);
	enc_drv.type = LV_INDEV_TYPE_ENCODER;
	enc_drv.read_cb = encoder_indev_read;
	lv_indev_drv_register(& enc_drv);

	linux_create_thread(& lv_tick_inc_t, thread_spool_encinterrupt, 50, 1);
	linux_create_thread(& lv_tick_inc_t, thread_lv_tick_inc, 50, 1);

//	lv_demo_benchmark_run_scene(26);
//	lv_demo_widgets();
	lv_demo_keypad_encoder();

	while(1) {
		lv_timer_handler();
		usleep(10000);
	}

#endif
#if 0 && __riscv && defined(__riscv_zicsr)
	{
		unsigned vm = (csr_read_mstatus() >> 24) & 0x1F;
		unsigned sxl = (csr_read_mstatus() >> 34) & 0x03;
		unsigned uxl = (csr_read_mstatus() >> 32) & 0x03;
		unsigned xs = (csr_read_mstatus() >> 15) & 0x03;
		PRINTF("1 mstatus=%08lX, vm=%u, sxl=%u, uxl=%u, xs=%u\n", csr_read_mstatus(), vm, sxl, uxl, xs);
	}
#endif
#if 0 && __riscv && defined(__riscv_zicsr)
	{
		PRINTF("misa=0x%016lX\n", (unsigned long) csr_read_misa());
		PRINTF("mvendorid=0x%08X\n", csr_read_mvendorid());
		PRINTF("marchid=0x%08X\n", csr_read_marchid());
		PRINTF("mimpid=0x%08X\n", csr_read_mimpid());
		PRINTF("mhartid=0x%08X\n", csr_read_mhartid());
	}
#endif
#if 0 && __riscv && defined(__riscv_zicsr)
	{
		// see https://github.com/five-embeddev/riscv-csr-access/blob/master/include/riscv-csr.h

		//	0 A Atomic extension
		//	1 B Reserved
		//	2 C Compressed extension
		//	3 D Double-precision floating-point extension
		//	4 E RV32E base ISA
		//	5 F Single-precision floating-point extension
		//	6 G Reserved
		//	7 H Hypervisor extension
		//	8 I RV32I/64I/128I base ISA
		//	9 J Reserved
		//	10 K Reserved
		//	11 L Reserved
		//	12 M Integer Multiply/Divide extension
		//	13 N Tentatively reserved for User-Level Interrupts extension
		//	14 O Reserved
		//	15 P Tentatively reserved for Packed-SIMD extension
		//	16 Q Quad-precision floating-point extension
		//	17 R Reserved
		//	18 S Supervisor mode implemented
		//	19 T Reserved
		//	20 U User mode implemented
		//	21 V “V” Vector extension implemented
		//	22 W Reserved
		//	23 X Non-standard extensions present
		//	24 Y Reserved
		//	25 Z Reserved

		// Allwinner F133-A
		//	READ_CSR(misa)=00B4112D: --X-VU-S-----M---I--F-DC-A
		const unsigned misa_val = csr_read_misa();
		unsigned i;
		PRINTF("misa=%08X: ", misa_val);
		for (i = 0; i < 26; ++ i)
		{
			const int pos = 25 - i;
			const unsigned mask = UINT32_C(1) << pos;
			PRINTF("%c", (misa_val & mask) ? 'A' + pos : '-');
		}
		PRINTF("\n");

	}
#endif
#if 0 && WITHDEBUG && __riscv
	{
		PRINTF("sqrtf=%d\n", (int) (sqrtf(2) * 10000));
		PRINTF("sqrt=%d\n", (int) (sqrt(2) * 10000));
		PRINTF("sqrtf=%g\n", sqrtf(2));
		PRINTF("sqrt=%g\n", sqrt(2));

	}
#endif
#if 0 && defined (BOARD_BLINK_INITIALIZE)
	{
		// LED blink test
		uint_fast8_t state = 0;
		BOARD_BLINK_INITIALIZE();
		for (;;)
		{
			if (state)
			{
				state = 0;
				BOARD_BLINK_SETSTATE(0);
			}
			else
			{
				state = 1;
				BOARD_BLINK_SETSTATE(1);
			}
			local_delay_ms(250);
		}
	}
#endif
#if 0 && (CPUSTYLE_T113 || CPUSTYLE_F133)
	{
		PRINTF("SYS_CFG->SYS_LDO_CTRL_REG=0x%08X (expected arm: 0x0000190E, risc-v: 0x00002F0F)\n", (unsigned) SYS_CFG->SYS_LDO_CTRL_REG);
	}
#endif
#if 0 && (CPUSTYLE_T113)
	{
		PRINTF("C0_CPUX_CFG->C0_CTRL_REG0=0x%08X (expected 0x80000000)\n", (unsigned) C0_CPUX_CFG->C0_CTRL_REG0);
	}
#endif
#if 0
	{
		// CMSIS RTOS2 test
		blinky_main();
	}
#endif
//	PRINTF("TARGET_UART1_TX_MIO test\n");
//	for (;;)
//	{
//		const portholder_t pinmode = MIO_PIN_VALUE(1, 0, GPIO_IOTYPE_LVCMOS33, 1, 0, 0, 0, 0, 0);
//		gpio_output2(TARGET_UART1_TX_MIO, 1, pinmode);
//		local_delay_ms(200);
//		gpio_output2(TARGET_UART1_TX_MIO, 0, pinmode);
//		local_delay_ms(200);
//	}
#if 0 && CPUSTYLE_XC7Z && defined (ZYNQBOARD_LED_RED)
	{
		// калибровка программной задержки
		for (;;)
		{
			const portholder_t pinmode = MIO_PIN_VALUE(1, 0, GPIO_IOTYPE_LVCMOS33, 1, 0, 0, 0, 0, 0);
			gpio_output2(ZYNQBOARD_LED_RED, 0, pinmode);		// LED_R
			//gpio_output2(ZYNQBOARD_LED_GREEN, 1, pinmode);		// LED_G
			local_delay_ms(50);

			gpio_output2(ZYNQBOARD_LED_RED, 1, pinmode);		// LED_R
			//gpio_output2(ZYNQBOARD_LED_GREEN, 0, pinmode);		// LED_G
			local_delay_ms(50);

		}
	}
#endif
#if 0 && CPUSTYLE_STM32MP1
	{
		// калибровка программной задержки

		//RCC->MP_APB5ENSETR = RCC_MP_APB5ENSETR_TZPCEN;
		//PRINTF("Hello. STM32MP157\n");
		//arm_hardware_pioa_altfn20(1uL << 13, 0);	// DBGTRO
		// LED blinking test
		//const uint_fast32_t mask = (1uL << 14);	// PA14 - GREEN LED LD5 on DK1/DK2 MB1272.pdf
		//const uint_fast32_t maskd = (1uL << 14);	// PD14 - LED on small board
		const uint_fast32_t maska = (1uL << 13);	// PA13 - bootloader status LED
		//arm_hardware_piod_outputs(maskd, 1 * maskd);
		arm_hardware_pioa_outputs(maska, 1 * maska);
		for (;;)
		{
			//dbg_putchar('5');
			//(GPIOD)->BSRR = BSRR_S(maskd);
			(GPIOA)->BSRR = BSRR_S(maska);
			__DSB();
			local_delay_ms(50);
			//dbg_putchar('#');
			//(GPIOD)->BSRR = BSRR_C(maskd);
			(GPIOA)->BSRR = BSRR_C(maska);
			__DSB();
			local_delay_ms(50);

		}
	}
#endif /* CPUSTYLE_STM32MP1 */
#if 0 && WITHDEBUG
	{
		// c++ execution test
		cpptest();
	}
#endif /* WITHDEBUG */
}
