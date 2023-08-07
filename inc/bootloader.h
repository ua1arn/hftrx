/*
 * bootloader.h
 *
 *  Created on: 14 апр. 2023 г.
 *      Author: User
 */

#ifndef INC_BOOTLOADER_H_
#define INC_BOOTLOADER_H_

#include "hardware.h"

void bootloader_fatfs_mainloop(void);
void bootloader_mainloop(void);


#if CPUSTYLE_STM32MP1
	// Bootloader parameters
	#if WITHSDRAMHW
		#define BOOTLOADER_RAMAREA DRAM_MEM_BASE	/* адрес ОЗУ, куда перемещать application */
		#define BOOTLOADER_RAMSIZE (256 * 1024uL * 1024uL)	// 256M
		#define BOOTLOADER_RAMPAGESIZE	(1024uL * 1024)	// при загрузке на исполнение используется размер страницы в 1 мегабайт
		#define USBD_DFU_RAM_XFER_SIZE 4096
		#define USBD_DFU_RAM_LOADER BOOTLOADER_RAMAREA//(BOOTLOADER_RAMAREA + 0x4000uL)	/* адрес ОЗУ, куда DFU загрузчиком помещаем первую страницу образа */
	#endif /* WITHSDRAMHW */

	#define BOOTLOADER_FLASHSIZE (1024uL * 1024uL * 16)	// 16M FLASH CHIP
	#define BOOTLOADER_SELFBASE QSPI_MEM_BASE	/* адрес где лежит во FLASH образ application */
	#define BOOTLOADER_SELFSIZE (1024uL * 512)	// 512k

	#define BOOTLOADER_APPBASE (BOOTLOADER_SELFBASE + BOOTLOADER_SELFSIZE)	/* Воображаемый адрес, адрес где лежит во FLASH образ application */
	#define BOOTLOADER_APPSIZE (chipsizeDATAFLASH() - BOOTLOADER_SELFSIZE)	// 2048 - 128
	#define USBD_DFU_FLASH_XFER_SIZE 256	// match to (Q)SPI FLASH MEMORY page size
	#define USBD_DFU_FLASHNAME "W25Q128JV"

	#define APPFIRSTOFFSET	(4400uL + BOOTLOADER_SELFSIZE)		// начальное смещение расположения образа applicaton
	#define USERFIRSTOFFSET	(APPFIRSTOFFSET + BOOTLOADER_FLASHSIZE)	// начальное смещение области для создания хранилища данных
	//#define APPFIRSTSECTOR (APPFIRSTOFFSET / 512)

	#define APPFIRSTSECTOR (0x84400  / 512)
	#define FSBL1FIRSTSECTOR (0x04400 / 512)
	#define FSBL2FIRSTSECTOR (0x44400  / 512)

#endif /* CPUSTYLE_STM32MP1*/

#if CPUSTYLE_R7S721
	// Bootloader parameters
	#define BOOTLOADER_RAMAREA Renesas_RZ_A1_ONCHIP_SRAM_BASE	/* адрес ОЗУ, куда перемещать application */
	#define BOOTLOADER_RAMSIZE (1024uL * 1024uL * 2)	// 2M
	#define BOOTLOADER_RAMPAGESIZE	(1024uL * 1024)	// при загрузке на исполнение используется размер страницы в 1 мегабайт
	#define USBD_DFU_RAM_XFER_SIZE 4096
	#define USBD_DFU_RAM_LOADER BOOTLOADER_RAMAREA//(BOOTLOADER_RAMAREA + 0x4000uL)	/* адрес ОЗУ, куда DFU загрузчиком помещаем первую страницу образа */

	#define BOOTLOADER_FLASHSIZE (1024uL * 1024uL * 2)	// 2M FLASH CHIP
	#define BOOTLOADER_SELFBASE Renesas_RZ_A1_SPI_IO0	/* Воображаемый адрес, адрес где лежит во FLASH образ application */
	#define BOOTLOADER_SELFSIZE (1024uL * 128)	// 128k

	#define BOOTLOADER_APPBASE (BOOTLOADER_SELFBASE + BOOTLOADER_SELFSIZE)	/* адрес где лежит во FLASH образ application */
	#define BOOTLOADER_APPSIZE (chipsizeDATAFLASH() - BOOTLOADER_SELFSIZE)	// 2048 - 128

	//#define BOOTLOADER_PAGESIZE (1024uL * 64)	// M25Px with 64 KB pages
	#define USBD_DFU_FLASH_XFER_SIZE 256	// match to (Q)SPI FLASH MEMORY page size
	#define USBD_DFU_FLASHNAME "M25P16"

#endif /* CPUSTYLE_R7S721 */

#if CPUSTYLE_XC7Z
	// Bootloader parameters

	#if WITHSDRAMHW
		#define BOOTLOADER_RAMAREA SDRAM_BASE	/* адрес ОЗУ, куда перемещать application */
		#define BOOTLOADER_RAMSIZE SDRAM_APERTURE_SIZE	// 255M
		#define BOOTLOADER_RAMPAGESIZE	(16 * 1024uL * 1024)	// при загрузке на исполнение используется размер страницы в 1 мегабайт
		#define USBD_DFU_RAM_XFER_SIZE 4096
		#define USBD_DFU_RAM_LOADER BOOTLOADER_RAMAREA//(BOOTLOADER_RAMAREA + 0x4000uL)	/* адрес ОЗУ, куда DFU загрузчиком помещаем первую страницу образа */
	#endif /* WITHSDRAMHW */

	#define BOOTLOADER_FLASHSIZE (16 * 1024uL * 1024uL)	// 16M FLASH CHIP
	#define BOOTLOADER_SELFBASE QSPI_LINEAR_BASE	/* Воображаемый адрес, адрес где лежит во FLASH образ application */
	#define BOOTLOADER_SELFSIZE (1024uL * 512)	// 512k

	#define BOOTLOADER_APPBASE (BOOTLOADER_SELFBASE + BOOTLOADER_SELFSIZE)	/* Воображаемый адрес, адрес где лежит во FLASH образ application */
	#define BOOTLOADER_APPSIZE (chipsizeDATAFLASH() - BOOTLOADER_SELFSIZE)	// 2048 - 128

	//#define BOOTLOADER_PAGESIZE (1024uL * 64)	// W25Q32FV with 64 KB pages

	#define USBD_DFU_FLASH_XFER_SIZE 256	// match to (Q)SPI FLASH MEMORY page size
	#define USBD_DFU_FLASHNAME "W25Q128JV"

#endif /* CPUSTYLE_XC7Z */

#if CPUSTYLE_ALLWINNER
	// Bootloader parameters
	#if WITHSDRAMHW
		#define BOOTLOADER_RAMAREA DRAM_SPACE_BASE	/* адрес ОЗУ, куда перемещать application */
		#define BOOTLOADER_RAMSIZE (1024uL * 1024uL * 128)	// 256M
		#define BOOTLOADER_RAMPAGESIZE	(1024uL * 1024)	// при загрузке на исполнение используется размер страницы в 1 мегабайт
		#define USBD_DFU_RAM_XFER_SIZE 4096
		#define USBD_DFU_RAM_LOADER BOOTLOADER_RAMAREA//(BOOTLOADER_RAMAREA + 0x4000uL)	/* адрес ОЗУ, куда DFU загрузчиком помещаем первую страницу образа */
	#endif /* WITHSDRAMHW */

	/* DFU device разделяет по приходящему адресу куда писать */
	#define BOOTLOADER_FLASHSIZE (1024uL * 1024uL * 16)	// 16M FLASH CHIP
	#define BOOTLOADER_SELFBASE 0x30000000uL	/* Воображаемый адрес, где лежит во FLASH образ application */
	#define BOOTLOADER_SELFSIZE (1024uL * 256)	// 256k

	#define BOOTLOADER_APPBASE (BOOTLOADER_SELFBASE + BOOTLOADER_SELFSIZE)	/* Воображаемый адрес, адрес где лежит во FLASH образ application */
	#define BOOTLOADER_APPSIZE (chipsizeDATAFLASH() - BOOTLOADER_SELFSIZE)	// 2048 - 128

	//#define BOOTLOADER_PAGESIZE (1024uL * 64)	// W25Q32FV with 64 KB pages

	#define USBD_DFU_FLASH_XFER_SIZE 256	// match to (Q)SPI FLASH MEMORY page size
	#define USBD_DFU_FLASHNAME "W25Q128JV"

#endif /* CPUSTYLE_ALLWINNER */

#endif /* INC_BOOTLOADER_H_ */
