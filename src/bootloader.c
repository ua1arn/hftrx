/*
 * bootloader.c
 *
 *  Created on: 14 апр. 2023 г.
 *      Author: User
 */


#include "hardware.h"
#include "bootloader.h"
#include "board.h"
#include "keyboard.h"
#include "encoder.h"
//#include "display2.h"

#include "spi.h"	// bootloader_readimage definition
#include "formats.h"

#if WITHUSEFATFS
	#include "fatfs/ff.h"
#endif /* WITHUSEFATFS */
#if WITHUSEFATFS
	#include "sdcard.h"
#endif /* WITHUSEFATFS */

#include <string.h>
#include <ctype.h>
#include <math.h>


#if WITHISBOOTLOADER

int bootloader_withusb(void)
{
	static int st;
	static int en;
	if (st == 0)
	{
		st = 1;
#if defined BOARD_IS_USERBOOT
		en = BOARD_IS_USERBOOT();
#else
		en = 0;
#endif
	}
	return en;
}

struct stm32_header {
	uint32_t magic_number;
	uint8_t image_signature[64];
	uint32_t image_checksum;
	uint8_t  header_version[4];
	uint32_t image_length;
	uint32_t image_entry_point;
	uint32_t reserved1;
	uint32_t load_address;
	uint32_t reserved2;
	uint32_t version_number;
	uint32_t option_flags;
	uint32_t ecdsa_algorithm;
	uint8_t ecdsa_public_key[64];
	uint8_t padding[83];
	uint8_t binary_type;
} ATTRPACKED;

#define HEADER_MAGIC	0x324d5453  //	__be32_to_cpu(0x53544D32)

static uint_fast8_t bootloader_get_start(
		uintptr_t apparea,	/* целевой адрес для загрузки образа - здесь лежит заголовок файла */
		uintptr_t * ip)
{
	volatile struct stm32_header * const hdr = (volatile struct stm32_header *) apparea;
	uint_fast32_t checksum = hdr->image_checksum;
	uint_fast32_t length = hdr->image_length;
	const uint8_t * p = (const uint8_t *) (uintptr_t) hdr->load_address;
	if (hdr->magic_number != HEADER_MAGIC)
		return 1;
	* ip = hdr->image_entry_point;
	while (length --)
		checksum -= * p ++;
	return checksum != 0;	// возврат 0 если контрольная сумма совпала
}

static uint_fast8_t bootloader_get_start_no_cks(
		uintptr_t apparea,	/* целевой адрес для загрузки образа - здесь лежит заголовок файла */
		uintptr_t * ip, unsigned * lp)
{
	volatile struct stm32_header * const hdr = (volatile struct stm32_header *) apparea;
	uint_fast32_t checksum = hdr->image_checksum;
	uint_fast32_t length = hdr->image_length;
	const uint8_t * p = (const uint8_t *) (uintptr_t) hdr->load_address;
	if (hdr->magic_number != HEADER_MAGIC)
		return 1;
	* lp = length;
	return 0;
}

static uint_fast8_t bootloader_copyapp(
		uint_fast32_t appoffset,	/* смещение заголовка приожения в накопителе */
		uintptr_t * ip
		)
{
	enum { HEADERSIZE = 256 };
	static uint8_t tmpbuff [HEADERSIZE];
	volatile struct stm32_header * const hdr = (volatile struct stm32_header *) tmpbuff;

	bootloader_readimage(appoffset, tmpbuff, HEADERSIZE);
	//printhex(appoffset, tmpbuff, HEADERSIZE);
	if (hdr->magic_number != HEADER_MAGIC)
	{
		printhex(appoffset, tmpbuff, HEADERSIZE);
		return 1;
	}
	PRINTF("bootloader_copyapp: ip=%08X (addr=%08X, len=%08X)\n", (unsigned) * ip, (unsigned) hdr->load_address, (unsigned) hdr->image_length);
	bootloader_readimage(appoffset + HEADERSIZE, (void *) (uintptr_t) hdr->load_address, hdr->image_length);
	PRINTF("bootloader_copyapp done.\n");
	if (bootloader_get_start((uintptr_t) hdr, ip))	// verify
	{
		printhex((uintptr_t) hdr->load_address, (void *) (uintptr_t) hdr->load_address, 512);
		return 1;
	}
	return 0;
}

// Сюда попадаем из USB DFU клвсса при приходе команды
// DFU_Detach после USBD_Stop
static void
bootloader_launch_app(uintptr_t ip)
{
	dcache_clean_all();
	global_disableIRQ();

	/* вынесение в отдельную функцию приводит к незхапуску application */
	{

#if (__L2C_PRESENT == 1)
	L2C_Disable();
#endif
#if (__CORTEX_A != 0)
	L1C_DisableCaches();
	L1C_DisableBTAC();
#endif


#if CPUSTYLE_F133
	/* disable memory cache */
//	csr_read_mhint=0x4000
//	csr_read_mxstatus=0xc0408000
//	csr_read_mhcr=0x109
//	csr_read_mcor=0x3
	csr_write_mhint(0x4000);
	csr_write_mxstatus(0xc0408000);

	csr_write_mhcr(0x109);
	csr_write_mcor(0x3);

#endif /* CPUSTYLE_F133 */


#if (__GIC_PRESENT == 1)
	// keep enabled foe CPU1 start
	//GIC_DisableInterface();
	//GIC_DisableDistributor();

	// Disable all IRQs
	{
		// Get ITLinesNumber
		const unsigned n = ((GIC_DistributorInfo() & 0x1f) + 1) * 32;
		unsigned i;
		// 32 - skip SGI handlers (keep enabled for CPU1 start).
		for (i = 32; i < n; ++ i)
			IRQ_Disable(i);
	}
#endif

#if CPUSTYLE_F133
	/* disable interrupts*/
	csr_clr_bits_mie(MIE_MEI_BIT_MASK);	// MEI
	csr_clr_bits_mstatus(MSTATUS_MIE_BIT_MASK); // Disable interrupts routing

#endif /* CPUSTYLE_F133 */


#if (__CORTEX_A != 0) && CPUSTYLE_ARM && (! defined(__aarch64__))

	MMU_Disable();
	MMU_InvalidateTLB();
	__ISB();
	__DSB();
#endif

#if CPUSTYLE_F133
	/* disable MMU */

#endif /* CPUSTYLE_F133 */

	}
	dbg_flush();	// дождаться, пока будут переданы все символы, ы том числе и из FIFO


	(* (void (*)(void)) ip)();


	for (;;)
		;
}

/* Вызов заказан вызывется из обработчика USB прерываний EP0 */
void bootloader_deffereddetach(void * arg)
{
#if defined (USBD_DFU_RAM_LOADER)
	uintptr_t ip;
	if (bootloader_get_start(USBD_DFU_RAM_LOADER, & ip) == 0)
	{
		PRINTF("bootloader_deffereddetach: ip=%08lX\n", (unsigned long) ip);
		/* Perform an Attach-Detach operation on USB bus */
#if WITHUSBHW
		if (bootloader_withusb())
			board_usb_deactivate();
		if (bootloader_withusb())
			board_usb_deinitialize();
#endif /* WITHUSBHW */
		bootloader_launch_app(ip);
	}
	else
	{
		PRINTF("bootloader_deffereddetach: Header is not loaded to %08lX.\n", (unsigned long) USBD_DFU_RAM_LOADER);
	}
#endif /* defined (USBD_DFU_RAM_LOADER) */
}

#if WITHISBOOTLOADERFATFS

void bootloader_fatfs_mainloop(void)
{
	static const char IMAGENAME [] = WITHISBOOTLOADERIMAGE;
	static FATFSALIGN_BEGIN BYTE header [sizeof (struct stm32_header)] FATFSALIGN_END;
	static RAMNOINIT_D1 FATFS Fatfs;		/* File system object  - нельзя располагать в Cortex-M4 CCM */
	static RAMNOINIT_D1 FIL Fil;			/* Описатель открытого файла - нельзя располагать в Cortex-M4 CCM */
	FRESULT rc;
	UINT br = 0;		//  количество считанных байтов
	struct stm32_header * const hdr = (struct stm32_header *) & header;

	board_set_bglight(1, WITHLCDBACKLIGHTMIN);	// выключить подсветку
	board_update();
	PRINTF("bootloader_fatfs_mainloop start: '%s'\n", IMAGENAME);

	static BYTE targetdrv = 0;
	DSTATUS st = disk_initialize (targetdrv);				/* Physical drive nmuber (0..) */
	if (st != RES_OK)
	{
		PRINTF("disk_initialize code=%02X\n", st);
		PRINTF(" STA_NOINIT = %d\n", STA_NOINIT);
		PRINTF(" STA_NODISK = %d\n", STA_NODISK);
		PRINTF(" STA_PROTECT = %d\n", STA_PROTECT);
		for (;;)
			;
	}
	f_mount(& Fatfs, "", 0);		/* Register volume work area (never fails) */
	// чтение файла
	rc = f_open(& Fil, IMAGENAME, FA_READ);
	if (rc != FR_OK)
	{
		PRINTF("Can not open file '%s'\n", IMAGENAME);
		PRINTF("Failed with rc=%u.\n", rc);
		for (;;)
			;
	}
	rc = f_read(& Fil, header, sizeof header, & br);	/* Read a chunk of file */
	if (rc != FR_OK || br != sizeof (header))
	{
		PRINTF("Can not read header of file '%s'\n", IMAGENAME);
		PRINTF("Failed with rc=%u.\n", rc);
		for (;;)
			;
	}

	uint_fast32_t length = hdr->image_length;
	const uint8_t * p = (const uint8_t *) hdr->load_address;
	if (hdr->magic_number != HEADER_MAGIC)
	{
		PRINTF("Wrong header of file '%s'\n", IMAGENAME);
		for (;;)
			;
	}
	rc = f_read(& Fil, (BYTE *) hdr->load_address, hdr->image_length, & br);	/* Read a chunk of file */
	if (rc != FR_OK || br != hdr->image_length)
	{
		PRINTF("Can not read body of file '%s', rc=%d, hdr->image_length=%08lX, br=%08lX\n", IMAGENAME, (int) rc, (unsigned long) hdr->image_length, (unsigned long) br);
		PRINTF("Failed with rc=%u.\n", rc);
		for (;;)
			;
	}
	uint_fast32_t checksum = hdr->image_checksum;
	while (length --)
		checksum -= * p ++;
	if (checksum != 0)
	{
		PRINTF("Wrong body checksum of file '%s'\n", IMAGENAME);
		for (;;)
			;
	}
	rc = f_close(& Fil);
	if (rc != FR_OK)
	{
		PRINTF("Can not close file '%s'\n", IMAGENAME);
		PRINTF("Failed with rc=%u.\n", rc);
		for (;;)
			;
	}

#if BOOTLOADER_RAMSIZE
	uintptr_t ip;
	if (bootloader_get_start((uintptr_t) header, & ip) != 0)	/* проверка сигнатуры и получение стартового адреса */
	{
		PRINTF("bootloader_fatfs_mainloop start: can not load '%s'\n", IMAGENAME);
		for (;;)
			;
	}
#else
	ASSERT(0);
	for (;;)
		;
#endif /* BOOTLOADER_RAMSIZE */
#if WITHUSBHW
	if (bootloader_withusb())
		board_usb_deactivate();
	if (bootloader_withusb())
		board_usb_deinitialize();
#endif /* WITHUSBHW */
#if BOOTLOADER_RAMSIZE
	PRINTF("bootloader_fatfs_mainloop start: run '%s' at %p\n", IMAGENAME, (void *) ip);
#if WITHDEBUG
	local_delay_ms(100);
#endif /* WITHDEBUG */
	bootloader_launch_app(ip);
#endif /* BOOTLOADER_RAMSIZE */
}

#else /* WITHISBOOTLOADERFATFS */

// Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана
//void display2_keyboard_screen0(
//	uint_fast8_t x,
//	uint_fast8_t y,
//	dctx_t * pctx
//	)
//{
//
//}

#if WITHISBOOTLOADER0

static const uint8_t boot2image [] =
{
	#include BOARG_BOOTLOADER_IMAGE
};

void bootloader0_mainloop(void)
{
	const uintptr_t target = 0x20240100;	// The on-chip large-capacity RAM Page 4 base + 256K
	PRINTF("bootloader0_mainloop [%p]: target=%p\n", bootloader0_mainloop, (void *) target);

	memcpy((void *) target, boot2image, sizeof boot2image);

#if WITHUSBHW
	#error No WITHUSBHW supported
#endif /* WITHUSBHW */
	bootloader_launch_app(target);
	for (;;)
		;
}

#endif

void bootloader_mainloop(void)
{
	PRINTF("bootloader_mainloop: wait user/USB loop [%p]\n", bootloader_mainloop);
	PRINTF("bootloader_mainloop: user user/USB loop, CPU_FREQ=%u MHz\n", (unsigned) (CPU_FREQ / 1000 / 1000));

#if WITHLCDBACKLIGHT
	board_set_bglight(1, WITHLCDBACKLIGHTMIN);	// выключить подсветку
	board_update();
#endif /* WITHLCDBACKLIGHT */

#if BOOTLOADER_RAMSIZE && defined (BOARD_IS_USERBOOT) && ! WITHISBOOTLOADERRAWDISK

	if (BOARD_IS_USERBOOT() == 0)
	{
		PRINTF("bootloader_mainloop: user boot button released\n");
		/* Нет запроса на вход в режим загрузчика - грузим с QSPI FLASH */
		do
		{
			uintptr_t ip = 0xDEADBEEF;
			if (bootloader_copyapp(BOOTLOADER_SELFSIZE, & ip) != 0)	/* копирование исполняемого образа (если есть) в требуемое место */
			{
				PRINTF("bootloader_mainloop: No application image at offset 0x%08X\n", (unsigned) BOOTLOADER_SELFSIZE);
				break;
			}
#if WITHUSBHW
			if (bootloader_withusb())
				board_usb_deactivate();
			if (bootloader_withusb())
				board_usb_deinitialize();
#endif /* WITHUSBHW */
			PRINTF("bootloader_mainloop: ip=%08X\n", (unsigned) ip);
			bootloader_launch_app(ip);

		} while (0);
	}
	else
	{
		PRINTF("bootloader_mainloop: user boot button holded\n");
	}
#endif /* BOOTLOADER_RAMSIZE && defined (BOARD_IS_USERBOOT) */

#if WITHISBOOTLOADERRAWDISK
	// чтение application с предопределённого смещения на накопителе
	// WITHISBOOTLOADERRAWDISK_DEV
	PRINTF("bootloader_mainloop: boot from offset 0x%08X at disk %d\n", (unsigned) BOOTLOADER_SELFSIZE, (int) WITHISBOOTLOADERRAWDISK_DEV);

	do
	{
		uintptr_t drambase = DRAM_SPACE_BASE;
		BYTE targetDEV = WITHISBOOTLOADERRAWDISK_DEV;
		DRESULT dc;
		UINT br = 0;		//  количество считанных байтов
		uintptr_t ip;
		unsigned length = 0;
		struct stm32_header * const hdr = (struct stm32_header *) drambase;

		if (disk_initialize(targetDEV) != 0)
		{
			PRINTF("No targed device\n");
			break;
		}
		dc = disk_read(targetDEV,(void *) drambase, BOOTLOADER_SELFSIZE / 512, 1);
		if (dc != 0)
		{
			PRINTF("bootloade header read error\n");
			break;
		}
		if (bootloader_get_start_no_cks(drambase, & ip, & length) != 0)
		{
			PRINTF("1 bootloader: header is not loaded to %08lX.\n", (unsigned long) drambase);
			printhex(drambase, (void *) drambase, 512);
			break;
		}
		else
		{
			PRINTF("app read: length = %u (%u)\n", length + 256, (length + 256 + 511) / 512 * 512);
		}
		dc = disk_read(targetDEV,(void *) drambase, BOOTLOADER_SELFSIZE / 512, (length + 256 + 511) / 512);
		if (dc != 0)
		{
			PRINTF("app read error\n");
			break;
		}
		if (bootloader_get_start(drambase, & ip) == 0)
		{
			PRINTF("bootloader: go to ip=%08lX\n", (unsigned long) ip);
			/* Perform an Attach-Detach operation on USB bus */
			bootloader_launch_app(ip);
		}
		else
		{
			PRINTF("2 bootloader: app is not loaded to %08lX.\n", (unsigned long) drambase);
			printhex(drambase, (void *) drambase, 512);
		}
	} while (0);

#endif /* WITHISBOOTLOADERRAWDISK */

	/* Обеспечение работы USB DFU */
	for (;;)
	{
		uint_fast8_t kbch, kbready;
		processmessages(& kbch, & kbready, 0, NULL);

		if (kbready)
			PRINTF("bkbch=%02x\n", kbch);

		{
			/* здесь можно добавить обработку каких-либо команд с debug порта */
			char c;
			if (dbg_getchar(& c))
			{
				switch (c)
				{
				case 0x00:
					break;
				default:
					PRINTF("bkey=%02X\n", (unsigned char) c);
					break;

				}
			}
		}
	}
}
#endif /* WITHISBOOTLOADERFATFS */

#else


int bootloader_withusb(void)
{
#if WITHUSBHW
	return 1;
#else
	return 0;
#endif
}

#endif /* WITHISBOOTLOADER */
