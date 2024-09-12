/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	/* sprintf() replacement */
#include "gpio.h"
#include <ctype.h>
#include <string.h>

#if WITHSDHCHW && ! CPUSTYLE_ALLWINNER



enum
{

	EV_SD_ERROR = 1,
	EV_SD_READY = 2,
	EV_SD_DATA = 4

};

enum { WAIT_ANY = 0 };
typedef uint_fast8_t events_t;

static int WaitEvents(events_t e, int type);

#endif /* WITHSDHCHW && ! CPUSTYLE_ALLWINNER */

#if WITHUSESDCARD && ! CPUSTYLE_ALLWINNER

#include "board.h"
#include "sdcard.h"
#include "spi.h"
#include "fatfs/ff.h"	
#include "fatfs/diskio.h"		/* FatFs lower layer API */

enum { XC7Z_SDRDWRDMA = 1, XC7Z_AUTO_CMD12 = 1 };


/* Card Status Register*/

#define SD_STAT_OUT_OF_RANGE       0x80000000  /*The command's argument was out of the allowed range for this card.*/
#define SD_STAT_ADDRESS_ERROR      0x40000000  /*A misaligned address which did not match the block length was used in the command. */
#define SD_STAT_BLOCK_LEN_ERROR    0x20000000  /*The transferred block length is not allowed for this card, or the number
                                         of transferred bytes does not match the block length. */
#define SD_STAT_ERASE_SEQ_ERROR    0x10000000  /*An error in the sequence of erase commands occurred. */
#define SD_STAT_ERASE_PARAM        0x08000000  /*An invalid selection of write-blocks for erase occurred. */
#define SD_STAT_WP_VIOLATION       0x04000000  /*Set when the host attempts to write to a protected block or to the
                                         temporary or permanent write protected card. */
#define SD_STAT_CARD_IS_LOCKED     0x02000000  /*When set, signals that the card is locked by the host */
#define SD_STAT_LOCK_UNLOCK_FAILED 0x01000000  /*Set when a sequence or password error has been detected in
                                         lock/unlock card command. */
#define SD_STAT_COM_CRC_ERROR      0x00800000  /*The CRC check of the previous command failed.*/
#define SD_STAT_ILLEGAL_COMMAND    0x00400000  /*Command not legal for the card state. */
#define SD_STAT_CARD_ECC_FAILED    0x00200000  /*Card internal ECC was applied but failed to correct the data.*/
#define SD_STAT_CC_ERROR           0x00100000  /*Internal card controller error.*/
#define SD_STAT_ERROR              0x00080000  /*A general or an unknown error occurred during the operation.*/
#define SD_STAT_CSD_OVERWRITE      0x00010000  /*Can be either one of the following errors:
                                          - The read only section of the CSD does not match the card content.
                                          - An attempt to reverse the copy (set as original) or permanent WP
                                            (unprotected) bits was made. */
#define SD_STAT_WP_ERASE_SKIP      0x00008000  /*Set when only partial address space was erased due to existing
                                         write protected blocks or the temporary or permanent write
                                         protected card was erased. */
#define SD_STAT_CARD_ECC_DISABLED  0x00004000  /*The command has been executed without using the internal ECC.*/
#define SD_STAT_ERASE_RESET        0x00002000  /*An erase sequence was cleared before executing because an out of
                                         erase sequence command was received. */
#define SD_STAT_CURRENT_STATE      0x00001E00  /*The state of the card when receiving the command. If the command
                                         execution causes a state change, it will be visible to the host in the
                                         response to the next command. The four bits are interpreted as a
                                         binary coded number between 0 and 15.*/
#define SD_STAT_CURRENT_STATE_IDLE  0x00000000  
#define SD_STAT_CURRENT_STATE_READY 0x00000200
#define SD_STAT_CURRENT_STATE_IDENT 0x00000400
#define SD_STAT_CURRENT_STATE_STDBY 0x00000600
#define SD_STAT_CURRENT_STATE_TRAN  0x00000800
#define SD_STAT_CURRENT_STATE_DATA  0x00000A00
#define SD_STAT_CURRENT_STATE_RECV  0x00000C00
#define SD_STAT_CURRENT_STATE_PROG  0x00000E00
#define SD_STAT_CURRENT_STATE_DIS   0x00001000

#define SD_STAT_READY_FOR_DATA      0x00000100 /*Corresponds to buffer empty signaling on the bus. */
#define SD_STAT_APP_CMD             0x00000020 /*The card will expect ACMD, or an indication that the command has
                                         been interpreted as ACMD. */
#define SD_STAT_AKE_SEQ_ERROR       0x0000008  /*Error in the sequence of the authentication process*/


// this variable will be used to track the current block length
// this allows the block length to be set only when needed
// unsigned long _BlockLength = 0;

// error/success codes
#define MMC_SUCCESS           0x00
#define MMC_BLOCK_SET_ERROR   0x01
#define MMC_RESPONSE_ERROR    0x02
#define MMC_DATA_TOKEN_ERROR  0x03
#define MMC_INIT_ERROR        0x04
#define MMC_CRC_ERROR         0x10
#define MMC_WRITE_ERROR       0x11
#define MMC_OTHER_ERROR       0x12
#define MMC_TIMEOUT_ERROR     0xFF



// Функция получения значения бита по номеру из массива
static unsigned long array_get_bit(const uint8_t * data, unsigned total, unsigned leftbit)
{
	const unsigned bitfromstart = (total - 1) - leftbit;
	const unsigned byteindex = bitfromstart / 8;
	const unsigned bitindex = 7 - bitfromstart % 8;
	return (data [byteindex] >> bitindex) & 0x01;

}

// Функция получения значения битового поля из массива
static unsigned array_get_bits(const uint8_t * data, unsigned total, unsigned leftbit, unsigned width)
{
	unsigned v = 0;
	while (width --)
		v = v * 2 + array_get_bit(data, total, leftbit --);
	return v;
}

static void sd_power_enable(
	uint_fast8_t enable	// флаг управления включением питания SD карты
	)
{
	board_set_sdcardpoweron(enable);
	board_update();
}

static void sd_power_cycle(void)
{
	HARDWARE_SDIO_HANGOFF();
	sd_power_enable(0);	// off
	local_delay_ms(200);
	sd_power_enable(1);	// and ON
	local_delay_ms(50);
	HARDWARE_SDIO_INITIALIZE();
}

void sdcardhw_initialize(void)
{
	//PRINTF(PSTR("sdcardhw_initialize\n"));
	HARDWARE_SDIOSENSE_INITIALIZE();
	HARDWARE_SDIOPOWER_INITIALIZE();
	//sd_power_cycle();
	
	//PRINTF(PSTR("sdcardhw_initialize: done.\n"));
	return;
}

static uint_fast64_t sdhost_sdcard_parse_CSD(const uint8_t * bv)
{
	uint_fast64_t MMC_CardSize = 0;
	uint_fast32_t mmc_C_SIZE = 0;
	// Обработка информациолнного блока (Code length is 128 bits = 16 bytes)
	//
	const uint_fast8_t csdv = array_get_bits(bv, 128, 127, 2);	//  [127:126] - csd structure version
	PRINTF(PSTR("CSD version=0x%02x\n"), csdv);
	PRINTF(PSTR("CSD TRAN_SPEED=%d (0x%02x)\n"), array_get_bits(bv, 128, 103, 8), array_get_bits(bv, 128, 103, 8));	// [103:96]

	switch (csdv)
	{	
	case 1:
		// CSD version 2.0
		// Расчёт объёма карты памяти по структуре версии 2
		mmc_C_SIZE = array_get_bits(bv, 128, 69, 22);	// [69:48] This parameter is used to calculate the user data area capacity in the SD memory card (not include the protected area).

		//PRINTF(PSTR("mmc_C_SIZE = %lu\n"), (unsigned long) mmc_C_SIZE);
		MMC_CardSize = (uint_fast64_t) (mmc_C_SIZE + 1) * 512 * 1024;
		break;

	case 0:
		// CSD version 1.0
		{
			uint_fast64_t im;      // mask
			unsigned short j;      // index
			// CSD version 1.0
			unsigned char mmc_READ_BL_LEN = 0,  // Read block length
				mmc_C_SIZE_MULT = 0;

			mmc_READ_BL_LEN = array_get_bits(bv, 128, 83, 4); // [83:80] bv [5] & 0x0F;		// lower 4 bits of CCC and
			mmc_C_SIZE = array_get_bits(bv, 128, 73, 12);	// [73:62]
			mmc_C_SIZE_MULT = array_get_bits(bv, 128, 49, 3);	// [49:47]
			// bits 73:62  C_Size (12 bit for v1)
			MMC_CardSize = (mmc_C_SIZE + 1);

			// power function with base 2 is better with a loop
			// i = (pow(2,mmc_C_SIZE_MULT+2)+0.5);
			for (im = 2, j = mmc_C_SIZE_MULT + 2; j > 1; j --)
				im <<= 1;

			MMC_CardSize *= im;

			// power function with base 2 is better with a loop
			//i = (pow(2,mmc_READ_BL_LEN)+0.5);
			for (im = 2, j = mmc_READ_BL_LEN; j > 1; j --)
				im <<= 1;

			MMC_CardSize *= im;
		}
		break;
	}
	return MMC_CardSize;
}

#if WITHSDHCHW	/* Hardware SD HOST CONTROLLER */

//void hardware_sdhost_setspeed(unsigned long ticksfreq);


/** 
 * @brief SDIO Commands  Index
 */
#define SD_CMD_GO_IDLE_STATE                       ((uint8_t)0)
#define SD_CMD_SEND_OP_COND                        ((uint8_t)1)
#define SD_CMD_ALL_SEND_CID                        ((uint8_t)2)
#define SD_CMD_SET_REL_ADDR                        ((uint8_t)3) /*!< SDIO_SEND_REL_ADDR for SD Card */
#define SD_CMD_SET_DSR                             ((uint8_t)4)
//#define SD_CMD_SDIO_SEN_OP_COND                    ((uint8_t)5)
#define SD_CMD_HS_SWITCH                           ((uint8_t)6)
#define SD_CMD_SEL_DESEL_CARD                      ((uint8_t)7)
//#define SD_CMD_HS_SEND_EXT_CSD                     ((uint8_t)8)
#define SD_CMD_SEND_CSD                            ((uint8_t)9)
#define SD_CMD_SEND_CID                            ((uint8_t)10)
#define SD_CMD_READ_DAT_UNTIL_STOP                 ((uint8_t)11) /*!< SD Card doesn't support it */
#define SD_CMD_STOP_TRANSMISSION                   ((uint8_t)12)
#define SD_CMD_SEND_STATUS                         ((uint8_t)13)
#define SD_CMD_HS_BUSTEST_READ                     ((uint8_t)14)
#define SD_CMD_GO_INACTIVE_STATE                   ((uint8_t)15)
#define SD_CMD_SET_BLOCKLEN                        ((uint8_t)16)
#define SD_CMD_READ_SINGLE_BLOCK                   ((uint8_t)17)
#define SD_CMD_READ_MULT_BLOCK                     ((uint8_t)18)
#define SD_CMD_HS_BUSTEST_WRITE                    ((uint8_t)19)
#define SD_CMD_WRITE_DAT_UNTIL_STOP                ((uint8_t)20) /*!< SD Card doesn't support it */
#define SD_CMD_SET_BLOCK_COUNT                     ((uint8_t)23) /*!< SD Card doesn't support it */
#define SD_CMD_WRITE_SINGLE_BLOCK                  ((uint8_t)24)
#define SD_CMD_WRITE_MULT_BLOCK                    ((uint8_t)25)
#define SD_CMD_PROG_CID                            ((uint8_t)26) /*!< reserved for manufacturers */
#define SD_CMD_PROG_CSD                            ((uint8_t)27)
#define SD_CMD_SET_WRITE_PROT                      ((uint8_t)28)
#define SD_CMD_CLR_WRITE_PROT                      ((uint8_t)29)
#define SD_CMD_SEND_WRITE_PROT                     ((uint8_t)30)
#define SD_CMD_SD_ERASE_GRP_START                  ((uint8_t)32) /*!< To set the address of the first write
                                                                  block to be erased. (For SD card only) */
#define SD_CMD_SD_ERASE_GRP_END                    ((uint8_t)33) /*!< To set the address of the last write block of the
                                                                  continuous range to be erased. (For SD card only) */
#define SD_CMD_ERASE_GRP_START                     ((uint8_t)35) /*!< To set the address of the first write block to be erased.
                                                                  (For MMC card only spec 3.31) */

#define SD_CMD_ERASE_GRP_END                       ((uint8_t)36) /*!< To set the address of the last write block of the
                                                                  continuous range to be erased. (For MMC card only spec 3.31) */

#define SD_CMD_ERASE                               ((uint8_t)38)
#define SD_CMD_FAST_IO                             ((uint8_t)39) /*!< SD Card doesn't support it */
#define SD_CMD_GO_IRQ_STATE                        ((uint8_t)40) /*!< SD Card doesn't support it */
#define SD_CMD_LOCK_UNLOCK                         ((uint8_t)42)
#define SD_CMD_APP_CMD                             ((uint8_t)55)
#define SD_CMD_GEN_CMD                             ((uint8_t)56)
#define SD_CMD_NO_CMD                              ((uint8_t)64)

/** 
 * @brief Following commands are SD Card Specific commands.
 *        SDIO_APP_CMD should be sent before sending these commands.
 */
#define SD_CMD_APP_SD_SET_BUSWIDTH                 ((uint8_t)6)  /*!< For SD Card only */
#define SD_CMD_SD_APP_STATUS                       ((uint8_t)13) /*!< For SD Card only */
#define SD_CMD_SD_APP_SEND_NUM_WRITE_BLOCKS        ((uint8_t)22) /*!< For SD Card only */
#define SD_CMD_SD_APP_OP_COND                      ((uint8_t)41) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CLR_CARD_DETECT          ((uint8_t)42) /*!< For SD Card only */
#define SD_CMD_SD_APP_SEND_SCR                     ((uint8_t)51) /*!< For SD Card only */
#define SD_CMD_SDIO_RW_DIRECT                      ((uint8_t)52) /*!< For SD I/O Card only */
#define SD_CMD_SDIO_RW_EXTENDED                    ((uint8_t)53) /*!< For SD I/O Card only */

#define SD_CMD_SD_APP_SET_NWB_PREERASED        ((uint8_t)23) /*!< For SD Card only */

/** 
 * @brief Following commands are SD Card Specific security commands.
 *        SDIO_APP_CMD should be sent before sending these commands.
 */
#define SD_CMD_SD_APP_GET_MKB                      ((uint8_t)43) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_MID                      ((uint8_t)44) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CER_RN1                  ((uint8_t)45) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_CER_RN2                  ((uint8_t)46) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CER_RES2                 ((uint8_t)47) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_CER_RES1                 ((uint8_t)48) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_READ_MULTIPLE_BLOCK   ((uint8_t)18) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MULTIPLE_BLOCK  ((uint8_t)25) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_ERASE                 ((uint8_t)38) /*!< For SD Card only */
#define SD_CMD_SD_APP_CHANGE_SECURE_AREA           ((uint8_t)49) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MKB             ((uint8_t)48) /*!< For SD Card only */
/** 
 * @brief  Following commands are SD Card Specific commands.
 *         SDIO_APP_CMD should be sent before sending these commands.
 */
#define SDIO_SEND_IF_COND               ((uint32_t)0x00000008)

#define SD_VOLTAGE_WINDOW_SD            ((uint32_t)0x80100000)
#define SD_HIGH_CAPACITY                ((uint32_t)0x40000000)
#define SD_STD_CAPACITY                 ((uint32_t)0x00000000)
#define SD_CHECK_PATTERN                ((uint32_t)0x000001AA)

/** 
 * @brief Supported SD Memory Cards
 */
#define SDIO_STD_CAPACITY_SD_CARD_V1_1             ((uint32_t)0x00000000)
#define SDIO_STD_CAPACITY_SD_CARD_V2_0             ((uint32_t)0x00000001)
#define SDIO_HIGH_CAPACITY_SD_CARD                 ((uint32_t)0x00000002)
#define SDIO_MULTIMEDIA_CARD                       ((uint32_t)0x00000003)
#define SDIO_SECURE_DIGITAL_IO_CARD                ((uint32_t)0x00000004)
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD            ((uint32_t)0x00000005)
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD          ((uint32_t)0x00000006)
#define SDIO_HIGH_CAPACITY_MMC_CARD                ((uint32_t)0x00000007)

static uint_fast32_t sdhost_sdcard_RCA; /* = 0 */
static uint8_t sdhost_sdcard_CID [16];
static uint8_t sdhost_sdcard_CSD [16];

static uint_fast32_t sdhost_CardType = SDIO_STD_CAPACITY_SD_CARD_V1_1;
static uint32_t sdhost_SDType = SD_STD_CAPACITY;

static uint_fast8_t sdhost_use_cmd23;	// set block count
static uint_fast8_t sdhost_use_cmd20;

/* Automatic CMD12 issue */
static uint_fast8_t sdhost_hardware_cmd12(void)
{
#if CPUSTYLE_R7S721
	return 1;

#elif CPUSTYLE_STM32MP1
	return 0;

#elif CPUSTYLE_STM32F
	return 0;

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU
	return XC7Z_AUTO_CMD12;

#else
	#error Undefined CPUSTYLE_XXX
	return 0;

#endif
}

#if ! WITHSDHCHW
// SPI SD CARD (MMC SD)


#define RESPLIMIT 64000U
#define RESPLIMITREAD 640000L

/* Assert the CS signal, active low (CS=0) */
static void SDCARD_CS_LOW(void)
{
	spi_select2(targetsdcard, SPIC_MODE3, SPIC_SPEEDSDCARD);	// MODE0/MODE3 - смотри комментарий выше
}

/* De-assert the CS signal (CS=1) */
static void SDCARD_CS_HIGH(void)
{
	spi_unselect(targetsdcard);
}

// установить скорость SPI для указанного канала контроллера
void hardware_sdhost_setspeed(unsigned long ticksfreq)
{
	hardware_spi_master_setfreq(SPIC_SPEEDSDCARD, ticksfreq);
}

void hardware_sdhost_setbuswidth(uint_fast8_t use4bit)
{
}

/* вспомогательная подпрограмма расчёта CRC для одного бита.
   CRC хранится в старших 7 битах.
*/
static  uint_fast8_t crc7b1(uint_fast8_t crc, uint_fast8_t v1)
{
	if ((v1 != 0) != ((crc & 0x80) != 0))
	{
		crc <<= 1;
		crc ^= (0x09 << 1);
	}
	else
	{
		crc <<= 1;
	}

	return crc & 0xFF;
}

/* вспомогательная подпрограмма расчёта CRC для одного байта */
static uint_fast8_t crc7b8(uint_fast8_t crc, uint_fast8_t v8)
{
	crc = crc7b1(crc, v8 & 0x80);
	crc = crc7b1(crc, v8 & 0x40);
	crc = crc7b1(crc, v8 & 0x20);
	crc = crc7b1(crc, v8 & 0x10);
	crc = crc7b1(crc, v8 & 0x08);
	crc = crc7b1(crc, v8 & 0x04);
	crc = crc7b1(crc, v8 & 0x02);
	crc = crc7b1(crc, v8 & 0x01);

	return crc;
}

#elif CPUSTYLE_R7S721

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32MP1


/** 
 * @brief  SDIO Static flags, TimeOut, FIFO Address
 */
#define SDIO_STATIC_FLAGS               ((uint32_t)0x000005FF)
#define SDIO_CMD0TIMEOUT                ((uint32_t)0x00010000)

// SDIO data transfer complete
//	SDIO DMA2	Stream6	Channel 4	
void DMA2_Stream6_IRQHandler(void)
{
	PRINTF(PSTR("DMA2_Stream6_IRQHandler() trapped.\n"));
	for (;;)
		;
}

void SDIO_IRQHandler(void)
{
	PRINTF(PSTR("SDIO_IRQHandler trapped\n"));
	for (;;)
		;
}

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

void SDIO0_IRQHandler(void)
{
	PRINTF(PSTR("SDIO0_IRQHandler trapped\n"));
	for (;;)
		;
}

static int zynq_wait_for_transfer(uint_fast8_t txmode)
{
	for (;;)
	{
		const uint_fast32_t status = SD0->INT_STATUS;	// bits are Readable, write a one to clear
		const uint_fast32_t psts = SD0->PRESENT_STATE;
		const uint_fast32_t hgapctl = SD0->HOST_CTRL_BLOCK_GAP_CTRL;
		//PRINTF("zynq_wait_for_transfer: status=%08X, psts=%08X, hgapctl=%08X\n", status, psts, hgapctl);
		if ((status & (1u << 7)) != 0) // Card_Removal
		{
			SD0->INT_STATUS = (1u << 7); // Card_Removal
			hardware_sdhost_detect((SD0->PRESENT_STATE >> 16) & 0x01);	// Card_Inserted
			return 1;
		}
		if ((status & (1u << 6)) != 0) // Card_Insertion
		{
			SD0->INT_STATUS = (1u << 6); // Card_Insertion
			hardware_sdhost_detect((SD0->PRESENT_STATE >> 16) & 0x01);	// Card_Inserted
			return 1;
		}
		if ((status & (1u << 15)) != 0)
		{
			SD0->INT_STATUS = (1u << 15); // Error_Interrupt
			return 1;
		}
//		if ((status & (1u << 16)) != 0)
//		{
//			SD0->INT_STATUS = (1u << 16); // Command_Timeout_Error
//			return 1;
//		}
		if ((status & (1u << 3)) != 0)
		{
			SD0->INT_STATUS = (1u << 3); // DMA_Interrupt
			SD0->SYS_DMA_ADDR = SD0->SYS_DMA_ADDR;
			//return 0;
		}
		if ((status & (1u << 1)) != 0)
		{
			SD0->INT_STATUS = (1u << 1); // Transfer_Complete
			return 0;
		}
	}
	return 1;

	// DMA_sdio_waitdone
	for (;;)
	{
		const uint_fast32_t status = SD0->INT_STATUS;	// bits are Readable, write a one to clear
		const uint_fast32_t psts = SD0->PRESENT_STATE;
		const uint_fast32_t hgapctl = SD0->HOST_CTRL_BLOCK_GAP_CTRL;
		//PRINTF("zynq_wait_for_transfer: status=%08X, psts=%08X, hgapctl=%08X\n", status, psts, hgapctl);
		if ((status & (1u << 7)) != 0) // Card_Removal
		{
			SD0->INT_STATUS = (1u << 7); // Card_Removal
			hardware_sdhost_detect((SD0->PRESENT_STATE >> 16) & 0x01);	// Card_Inserted
			return 1;
		}
		if ((status & (1u << 6)) != 0) // Card_Insertion
		{
			SD0->INT_STATUS = (1u << 6); // Card_Insertion
			hardware_sdhost_detect((SD0->PRESENT_STATE >> 16) & 0x01);	// Card_Inserted
			return 1;
		}
		if ((status & (1u << 15)) != 0)
		{
			SD0->INT_STATUS = (1u << 15); // Error_Interrupt
			return 1;
		}
//		if ((status & (1u << 16)) != 0)
//		{
//			SD0->INT_STATUS = (1u << 16); // Command_Timeout_Error
//			return 1;
//		}
		if (txmode)
		{
			if ((status & (1u << 3)) != 0)
			{
				SD0->INT_STATUS = (1u << 3); // DMA_Interrupt
				SD0->SYS_DMA_ADDR = SD0->SYS_DMA_ADDR;
				//return 0;
				if ((status & (1u << 1)) != 0)
				{
					SD0->INT_STATUS = (1u << 1); // Transfer_Complete
					return 0;
				}
			}
			if ((SD0->HOST_CTRL_BLOCK_GAP_CTRL & (1u << 16)) != 0)	// Stop_At_Block_Gap_Request
			{
				SD0->HOST_CTRL_BLOCK_GAP_CTRL &= ~ (1u << 16);	// Stop_At_Block_Gap_Request
				SD0->HOST_CTRL_BLOCK_GAP_CTRL |= (1u << 17);	// Continue_Request

			}
		}
		else
		{
			if ((status & (1u << 3)) != 0)
			{
				SD0->INT_STATUS = (1u << 3); // DMA_Interrupt
				SD0->SYS_DMA_ADDR = SD0->SYS_DMA_ADDR;
				//return 0;
			}
		}
		if ((status & (1u << 1)) != 0)
		{
			SD0->INT_STATUS = (1u << 1); // Transfer_Complete
			return 0;
		}
	}
	return 1;
}

static int zynq_wait_for_command(void)
{
	for (;;)
	{
		const uint_fast32_t status = SD0->INT_STATUS;	// bits are Readable, write a one to clear
		const uint_fast32_t psts = SD0->PRESENT_STATE;
		const uint_fast32_t hgapctl = SD0->HOST_CTRL_BLOCK_GAP_CTRL;
		//PRINTF("zynq_wait_for_command: status=%08X, psts=%08X, hgapctl=%08X\n", status, psts, hgapctl);
		if ((status & (1u << 7)) != 0) // Card_Removal
		{
			SD0->INT_STATUS = (1u << 7); // Card_Removal
			hardware_sdhost_detect((SD0->PRESENT_STATE >> 16) & 0x01);	// Card_Inserted
			return 1;
		}
		if ((status & (1u << 6)) != 0) // Card_Insertion
		{
			SD0->INT_STATUS = (1u << 6); // Card_Insertion
			hardware_sdhost_detect((SD0->PRESENT_STATE >> 16) & 0x01);	// Card_Inserted
			return 1;
		}
//		if ((psts & (3uL << 0)) != 0)	// Command_Inhibit_DAT | Command_Inhibit_CMD
//			continue;
		//PRINTF("sdhost_get_none_resp: SD0->INT_STATUS=%08X\n", SD0->INT_STATUS);
		if ((status & (1u << 15)) != 0)
		{
			SD0->INT_STATUS = (1u << 15); // Error_Interrupt
			return 1;
		}
		if ((status & (1u << 16)) != 0)
		{
			SD0->INT_STATUS = (1u << 16); // Command_Timeout_Error
			return 1;
		}
//		if ((status & (1u << 1)) != 0)
//		{
//			SD0->INT_STATUS = (1u << 1); // Transfer_Complete
//			return 0;
//		}
		if ((status & (1u << 0)) != 0)
		{
			SD0->INT_STATUS = (1u << 0); // Command_Complete
			return 0;
		}
	}
	return 1;
}


#else
	#error Wrong CPUSTYLE_xxx
#endif

#define DMA_SxCR_PL_VALUE 2uL		// STM32xxx DMA Priority level - High


/* DMA для обмена с SD CARD  - установка параметров следующего обмена */
//	RX	SAI2_B	DMA2	Stream6	Channel 4	
static void DMA_SDIO_setparams(
	uintptr_t addr,
	uint_fast32_t length0,
	uint_fast32_t count,
	uint_fast8_t direction	// 0: Peripheral-to-memory, 1: Memory-to-peripherial
	)
{
	//PRINTF(PSTR("DMA_SDIO_setparams: addr=%p\n"), addr);
#if ! WITHSDHCHW
// SPI SD CARD (MMC SD)

#elif CPUSTYLE_R7S721

	// DMA по передаче SDHI0

	enum { id = 14 };	// 14: DMAC14

	DMAC14.CHCTRL_n = 1 * (1U << DMAC14_CHCTRL_n_SWRST_SHIFT);		// SWRST
	//DMAC14.CHCTRL_n = 1 * (1U << DMAC14_CHCTRL_n_CLRINTMSK_SHIFT);	// CLRINTMSK
	DMAC14.CHCTRL_n = 1 * (1U << DMAC14_CHCTRL_n_SETINTMSK_SHIFT);	// SETINTMSK - запрещаем прерывание от контроллера DMA

	if (direction != 0)
	{
		// Write to SD CARD
		/* SDHI_0 transmission */
		// DMAC14
		DMAC14.N0SA_n = addr;	// Source address
		DMAC14.N0DA_n = (uint32_t) & SDHI0.SD_BUF0;	// Fixed destination address
		DMAC14.N0TB_n = length0 * count;	// размер в байтах

		// Values from Table 9.4 On-Chip Peripheral Module Requests
		// SDHI0 (transmit data empty)
		const uint_fast8_t mid = 0x30;	
		const uint_fast8_t rid = 0x01;		
		const uint_fast8_t tm = 0;		
		const uint_fast8_t am = 0x02;		
		const uint_fast8_t lvl = 1;		
		const uint_fast8_t reqd = 1;	

		DMAC14.CHCFG_n =
			0 * (1U << 31) |	// DMS	0: Register mode
			0 * (1U << 30) |	// REN	0: Does not continue DMA transfers.
			0 * (1U << 29) |	// RSW	0: Does not invert RSEL automatically after a DMA transaction
			0 * (1U << 28) |	// RSEL	0: Executes the Next0 Register Set
			0 * (1U << 27) |	// SBE	0: Stops the DMA transfer without sweeping the buffer (initial value).
			0 * (1U << 24) |	// DEM	0: Does not mask the DMA transfer end interrupt - прерывания каждый раз после TC
			tm * (1U << 22) |	// TM	0: Single transfer mode - берётся из Table 9.4
			1 * (1U << 21) |	// DAD	1: Fixed destination address
			0 * (1U << 20) |	// SAD	0: Increment source address
			2 * (1U << 16) |	// DDS	2: 32 bits (Destination Data Size)
			2 * (1U << 12) |	// SDS	2: 32 bits (Source Data Size)
			am * (1U << 8) |	// AM	1: ACK mode: Level mode (active until the transfer request from an on-chip peripheral module
			lvl * (1U << 6) |	// LVL	1: Detects based on the level.
			1 * (1U << 5) |		// HIEN	1: When LVL = 1: Detects a request when the signal is at the High level.
			reqd * (1U << 3) |	// REQD		Request Direction
			(id & 0x07) * (1U << 0) |		// SEL	0: CH0/CH8
			0;

		enum { dmarsshift = (id & 0x01) * 16 };
		DMAC1415.DMARS = (DMAC1415.DMARS & ~ (0x1FFul << dmarsshift)) |
			mid * (1U << (2 + dmarsshift)) |		// MID
			rid * (1U << (0 + dmarsshift)) |		// RID
			0;
	}
	else
	{
		// Read from SD CARD
		/* SDHI_0 reception */
		// DMAC14
		DMAC14.N0SA_n = (uint32_t) & SDHI0.SD_BUF0;		// Fixed source addess
		DMAC14.N0DA_n = addr;	// destination address
		DMAC14.N0TB_n = length0 * count;	// размер в байтах

		// Values from Table 9.4 On-Chip Peripheral Module Requests
		// SDHI0 (transmit data empty)
		const uint_fast8_t mid = 0x30;	
		const uint_fast8_t rid = 0x02;		
		const uint_fast8_t tm = 0;		
		const uint_fast8_t am = 0x02;		
		const uint_fast8_t lvl = 1;		
		const uint_fast8_t reqd = 0;	

		DMAC14.CHCFG_n =
			0 * (1U << 31) |	// DMS	0: Register mode
			0 * (1U << 30) |	// REN	0: Does not continue DMA transfers.
			0 * (1U << 29) |	// RSW	0: Does not invert RSEL automatically after a DMA transaction.
			0 * (1U << 28) |	// RSEL	0: Executes the Next0 Register Set
			0 * (1U << 27) |	// SBE	0: Stops the DMA transfer without sweeping the buffer (initial value).
			0 * (1U << 24) |	// DEM	0: Does not mask the DMA transfer end interrupt - прерывания каждый раз после TC
			tm * (1U << 22) |	// TM	0: Single transfer mode - берётся из Table 9.4
			0 * (1U << 21) |	// DAD	0: Increment destination address
			1 * (1U << 20) |	// SAD	1: Fixed source address
			2 * (1U << 16) |	// DDS	2: 32 bits (Destination Data Size)
			2 * (1U << 12) |	// SDS	2: 32 bits (Source Data Size)
			am * (1U << 8) |	// AM	1: ACK mode: Level mode (active until the transfer request from an on-chip peripheral module
			lvl * (1U << 6) |	// LVL	1: Detects based on the level.
			1 * (1U << 5) |		// HIEN	1: When LVL = 1: Detects a request when the signal is at the High level.
			reqd * (1U << 3) |	// REQD		Request Direction
			(id & 0x07) * (1U << 0) |		// SEL	0: CH0/CH8
			0;

		enum { dmarsshift = (id & 0x01) * 16 };
		DMAC1415.DMARS = (DMAC1415.DMARS & ~ (0x1FFul << dmarsshift)) |
			mid * (1U << (2 + dmarsshift)) |		// MID
			rid * (1U << (0 + dmarsshift)) |		// RID
			0;
	}


	// Установка режима для всех каналов (8..15)
    DMAC815.DCTRL_0_7 = (DMAC815.DCTRL_0_7 & ~ (/*(1U << 1) | */(1U << 0))) |
		//1 * (1U << 1) |		// LVINT	1: Level output
		1 * (1U << 0) |		// PR		1: Round robin mode
		0;

	DMAC14.CHCTRL_n = 1 * (1U << 0);		// SETEN

	if (count > 1)
	{
		while ((SDHI0.SD_INFO2 & (1u << 13)) == 0)	// SCLKDIVEN
			;
		SDHI0.SD_SIZE = length0;

		while ((SDHI0.SD_INFO2 & (1u << 13)) == 0)	// SCLKDIVEN
			;
		SDHI0.SD_STOP = 1 * (1U << 8);	// SEC 1: Enables SD_SECCNT

		while ((SDHI0.SD_INFO2 & (1u << 13)) == 0)	// SCLKDIVEN
			;
		SDHI0.SD_SECCNT = count;
	}
	else
	{
		while ((SDHI0.SD_INFO2 & (1u << 13)) == 0)	// SCLKDIVEN
			;
		SDHI0.SD_SIZE = length0;

		while ((SDHI0.SD_INFO2 & (1u << 13)) == 0)	// SCLKDIVEN
			;
		SDHI0.SD_STOP = 0 * (1U << 8);	// SEC 0: Disables SD_SECCNT
	}

	SDHI0.CC_EXT_MODE |= (1u << 1);	// DMASDRW

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	// в процессоре для обмена с SDIO используется выделенный блок DMA

	SDMMC1->IDMACTRL |= SDMMC_IDMA_IDMAEN;
	SDMMC1->IDMABASE0 = addr;
	//SDMMC1->IDMABSIZE = (SDMMC1->IDMABSIZE & ~ (SDMMC_IDMABSIZE_IDMABNDT)) |
	//	(((length0 * count / 32) << SDMMC_IDMABSIZE_IDMABNDT_Pos) & SDMMC_IDMABSIZE_IDMABNDT_Msk) |
	//0;
	//PRINTF(PSTR("SDMMC1->IDMABASE0=%08lx\n"), SDMMC1->IDMABASE0);
	ASSERT((SDMMC1->IDMACTRL & SDMMC_IDMA_IDMAEN_Msk) != 0);
	ASSERT(SDMMC1->IDMABASE0 == addr);


#elif CPUSTYLE_STM32F7XX || CPUSTYLE_STM32F4XX

	/* DMA2	Stream6	Channel 4 */ 
	const uint_fast8_t ch = 4;

	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	__DSB();

#if CPUSTYLE_STM32F7XX
	DMA2_Stream6->PAR = (uint32_t) & SDMMC1->FIFO;
#else /* CPUSTYLE_STM32F7XX */
	DMA2_Stream6->PAR = (uint32_t) & SDIO->FIFO;
#endif /* CPUSTYLE_STM32F7XX */

	DMA2_Stream6->M0AR = addr;
	DMA2_Stream6->NDTR = (DMA2_Stream6->NDTR & ~ DMA_SxNDT) |
		(1 * DMA_SxNDT_0);		// Особый случай - регистр игнорируется. В примерах от ST пишется 1

	if (direction == 0)
	{
		// Read from SD CARD
		//DMA2_Stream6->NDTR = (DMA2_Stream6->NDTR & ~ DMA_SxNDT) |
		//	(length0 * count / 4 * DMA_SxNDT_0);		// Особый случай - регистр игнорируется. В примерах от ST пишется 1

		DMA2_Stream6->FCR =
			1 * DMA_SxFCR_DMDIS |	// use FIFO mode
			3 * DMA_SxFCR_FTH_0 |	// 11: full FIFO
			0;
		(void) DMA2_Stream6->FCR;

		DMA2_Stream6->CR =
			0 * DMA_SxCR_DIR_0 |	// 00: Peripheral-to-memory
			ch * DMA_SxCR_CHSEL_0 | // канал
			1 * DMA_SxCR_PFCTRL |	// 1: The peripheral is the flow controller (required for SDIO/SDMMC connected DMA)
			1 * DMA_SxCR_MBURST_0 |	// 01: INCR4 (incremental burst of 4 beats)
			1 * DMA_SxCR_PBURST_0 |	// 01: INCR4 (incremental burst of 4 beats)
			//0 * DMA_SxCR_DIR_0 |	// 00: Peripheral-to-memory
			1 * DMA_SxCR_MINC |		//инкремент памяти
			2 * DMA_SxCR_MSIZE_0 | //длина в памяти - 32 bit
			2 * DMA_SxCR_PSIZE_0 | //длина в DR - 32 bit
			//1 * DMA_SxCR_CIRC | //циклический режим не требуется при DBM
			(DMA_SxCR_PL_VALUE << DMA_SxCR_PL_Pos) |		// Priority level - High
			0 * DMA_SxCR_CT |	// M0AR selected
			//0 * DMA_SxCR_DBM |	 // double buffer mode seelcted
			0;
	}
	else
	{
		// Write to SD CARD

		//DMA2_Stream6->NDTR = (DMA2_Stream6->NDTR & ~ DMA_SxNDT) |
		//	(length0 * count * DMA_SxNDT_0);		// Особый случай - регистр игнорируется. В примерах от ST пишется 1

		DMA2_Stream6->FCR =
			1 * DMA_SxFCR_DMDIS |	// use FIFO mode
			1 * DMA_SxFCR_FTH_0 |	// 01: 1/2 full FIFO (changed from read !)
			0;
		(void) DMA2_Stream6->FCR;

		DMA2_Stream6->CR =
			1 * DMA_SxCR_DIR_0 |	// 01: Memory-to-peripherial
			ch * DMA_SxCR_CHSEL_0 | // канал
			1 * DMA_SxCR_PFCTRL |	// 1: The peripheral is the flow controller (required for SDIO/SDMMC connected DMA)
			0 * DMA_SxCR_MBURST_0 |	// 00: single transfer (changed from read !)
			1 * DMA_SxCR_PBURST_0 |	// 01: INCR4 (incremental burst of 4 beats)
			//0 * DMA_SxCR_DIR_0 |	// 00: Peripheral-to-memory
			1 * DMA_SxCR_MINC |		//инкремент памяти
			0 * DMA_SxCR_MSIZE_0 | //длина в памяти - 8 bit (changed from read !) In direct mode, MSIZE is forced by hardware to the same value as PSIZE as soon as EN = 1.
			2 * DMA_SxCR_PSIZE_0 | //длина в DR - 32 bit
			//1 * DMA_SxCR_CIRC | //циклический режим не требуется при DBM
			(DMA_SxCR_PL_VALUE << DMA_SxCR_PL_Pos) |		// Priority level - High
			0 * DMA_SxCR_CT |	// M0AR selected
			//0 * DMA_SxCR_DBM |	 // double buffer mode seelcted
			0;
	}

	DMA2->HIFCR = (DMA_HIFCR_CTCIF6 /*| DMA_HIFCR_CTEIF6 */);	// Clear TC interrupt flag соответствующий stream
	//DMA2_Stream6->CR |= DMA_SxCR_TCIE;	// Разрешаем прерывания от DMA
	
	DMA2_Stream6->CR |= DMA_SxCR_EN;


#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU
	/*
	 * This register specifies the block size for block
	 * data transfers for CMD17, CMD18, CMD24, CMD25, and CMD53.
	 */
	SD0->SYS_DMA_ADDR = addr;
	SD0->Block_Size_Block_Count =
			(((unsigned long) length0) << 0) |	// Transfer_Block_Size
			(((unsigned long) count) << 16) | // Blocks_Count_for_Current_Transfer
			(0x07uL << 12) |	// Host_SDMA_Buffer_Size  - 111b - 512KB(Detects A18 Carry out)
			0;

#else
	#error Wrong CPUSTYLE_xxx
#endif
}

static uint_fast8_t DMA_sdio_waitdone(uint_fast8_t txmode)
{
#if ! WITHSDHCHW
// SPI SD CARD (MMC SD)
	return 0;

#elif CPUSTYLE_R7S721

	while ((DMAC14.CHSTAT_n & (1U << 5)) == 0)	// END
		;

	while ((SDHI0.SD_INFO2 & (1u << 13)) == 0)	// SCLKDIVEN
		;
	SDHI0.CC_EXT_MODE &= ~ (1u << 1);	// DMASDRW
	//__DMB();
	return 0;

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
	// в процессоре для обмена с SDIO используется выделенный блок DMA
	// check result
	if ((SDMMC1->STA & (SDMMC_STA_DATAEND | SDMMC_STA_DCRCFAIL | SDMMC_STA_DTIMEOUT)) != SDMMC_STA_DATAEND)
	{
		//__DMB();
		return 1;
	}
	return 0;

#elif CPUSTYLE_STM32F7XX || CPUSTYLE_STM32F4XX

	while ((DMA2->HISR & DMA_HISR_TCIF6) == 0)
	{
		//PRINTF(PSTR("DMA_sdio_waitdone: NDTR=%lu\n"), DMA2_Stream6->NDTRDMA2_Stream6->NDTR);
	}

	if ((DMA2->HISR & DMA_HISR_TCIF6) != 0)
		DMA2->HIFCR = DMA_HIFCR_CTCIF6;		// сбросил флаг - DMA готово начинать с начала
	else
	{
		PRINTF(PSTR("DMA_sdio_waitdone: force stop, NDTR=%lu\n"), DMA2_Stream6->NDTR);
		// DMA ещё может передавать
		// Надо привести в начальное состояние
		DMA2_Stream6->CR &= ~ DMA_SxCR_EN;
		while ((DMA2_Stream6->CR &  DMA_SxCR_EN) != 0)
			;
		//DMA2_Stream6->CR |= DMA_SxCR_EN;	// перезапуск DMA
	}
	//__DMB();
	return 0;

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU
	// DMA_sdio_waitdone
	return zynq_wait_for_transfer(txmode);

#else
	#error Wrong CPUSTYLE_xxx
#endif
}

// Обмен с SD CARD закончен аварийно. Остановить DMA
static void DMA_sdio_cancel(void)
{
#if ! WITHSDHCHW
// SPI SD CARD (MMC SD)

#elif CPUSTYLE_R7S721

	while ((SDHI0.SD_INFO2 & (1u << 13)) == 0)	// SCLKDIVEN
		;
	SDHI0.CC_EXT_MODE &= ~ (1u << 1);	// DMASDRW

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
	// в процессоре для обмена с SDIO используется выделенный блок DMA
	//SDMMC1->CMD = SDMMC_CMD_CMDSTOP;

	SDMMC1->DCTRL |= SDMMC_DCTRL_FIFORST_Msk;
	SDMMC1->IDMACTRL &= ~ SDMMC_IDMA_IDMAEN;

	while ((SDMMC1->DCTRL & SDMMC_DCTRL_FIFORST_Msk) != 0)
		;

#elif CPUSTYLE_STM32F7XX || CPUSTYLE_STM32F4XX

	{
		//PRINTF(PSTR("DMA_sdio_cancel: force stop, NDTR=%lu\n"), DMA2_Stream6->NDTR);
		// DMA ещё может передавать
		// Надо привести в начальное состояние
		DMA2_Stream6->CR &= ~ DMA_SxCR_EN;
		while ((DMA2_Stream6->CR &  DMA_SxCR_EN) != 0)
			;
		//DMA2_Stream6->CR |= DMA_SxCR_EN;	// перезапуск DMA
	}

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

#else
	#error Wrong CPUSTYLE_xxx
#endif
}


#if CPUSTYLE_R7S721

#elif CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

#elif CPUSTYLE_STM32F4XX

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

#else
	#error Wrong CPUSTYLE_xxx
#endif


// Ожидание окончания обмена data path state machine
// Возврат не-0 в слдучае ошибки
static uint_fast8_t sdhost_dpsm_wait(uintptr_t addr, uint_fast8_t txmode, uint_fast32_t len)
{
#if ! WITHSDHCHW
// SPI SD CARD (MMC SD)
	return 0;

#elif CPUSTYLE_R7S721

	return 0;

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	return WaitEvents(EV_SD_DATA, WAIT_ANY) == EV_SD_ERROR;

#elif CPUSTYLE_STM32F7XX

	unsigned w;
	w = 0;
	const uint_fast32_t errmask = (SDMMC_STA_DCRCFAIL | SDMMC_STA_RXOVERR | SDMMC_STA_TXUNDERR | SDMMC_STA_DTIMEOUT);
	for (;; ++ w)
	{
		const uint_fast32_t sta = SDMMC1->STA;
		if ((sta & errmask) != 0)
			break;
		if ((sta & SDMMC_STA_DBCKEND) != 0)
			return 0;
	}
	PRINTF(PSTR("sdhost_dpsm_wait error, STA=%08X, w=%u\n"), SDMMC1->STA, w);
	//SDMMC1->ICR = SDMMC1->STA & errmask;
	return 1;

#elif CPUSTYLE_STM32F4XX

	for (;;)
	{
		const uint_fast32_t sta = SDIO->STA;
		if ((sta & (SDIO_STA_DCRCFAIL | SDIO_STA_RXOVERR | SDIO_STA_TXUNDERR | SDIO_STA_DTIMEOUT /*| SDIO_STA_STBITERR */)) != 0)
			break;
		if ((sta & SDIO_STA_DBCKEND) != 0)
			return 0;
	}
	PRINTF(PSTR("sdhost_dpsm_wait error, STA=%08X\n"), SDIO->STA);
	return 1;

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	if (! XC7Z_SDRDWRDMA)
	{
		uint_fast32_t len4 = len / 4;
		ASSERT((len % 4) == 0);

		if (txmode)
		{
			// Fill FIFO
			const uint8_t * p = (const uint8_t *) addr;

			for (; len4 --; p += 4)
			{
				while ((SD0->PRESENT_STATE & (1u << 10)) == 0)	// 10 - Buffer_Write_Enable
					;
				const uint_fast32_t v =
					((uint_fast32_t) p [0] << 0) |
					((uint_fast32_t) p [1] << 8) |
					((uint_fast32_t) p [2] << 16) |
					((uint_fast32_t) p [3] << 24) |
					0;
				SD0->BUFFER_DATA_PORT = v;
			}
		}
		else
		{
			// Read FIFO
			uint8_t * p = (uint8_t *) addr;

			for (; len4 --; p += 4)
			{
				while ((SD0->PRESENT_STATE & (1u << 11)) == 0)	// 11 - Buffer_Read_Enable
					;
				const uint_fast32_t v = SD0->BUFFER_DATA_PORT;
				p [0] = v >> 0;
				p [1] = v >> 8;
				p [2] = v >> 16;
				p [3] = v >> 24;
			}
		}
	}
	return 0;

#else
	#error Wrong CPUSTYLE_xxx
	return 1;
#endif
}


static void sdhost_dpsm_wait_fifo_empty(void)
{
#if ! WITHSDHCHW
// SPI SD CARD (MMC SD)

#elif CPUSTYLE_R7S721

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
	// в процессоре для обмена с SDIO используется выделенный блок DMA
	// TODO: найти способ контроля состояния FIFO
	for (;;)
	{
		//const uint_fast32_t fifocnt = SDMMC1->FIFOCNT;
		//if ((fifocnt & SDMMC_FIFOCNT_FIFOCOUNT) == 0)
			break;
	}

#elif CPUSTYLE_STM32F7XX

	for (;;)
	{
		const uint_fast32_t fifocnt = SDMMC1->FIFOCNT;
		if ((fifocnt & SDMMC_FIFOCNT_FIFOCOUNT) == 0)
			break;
	}

#elif CPUSTYLE_STM32F4XX

	for (;;)
	{
		const uint_fast32_t fifocnt = SDIO->FIFOCNT;
		if ((fifocnt & SDIO_FIFOCNT_FIFOCOUNT) == 0)
			break;
	}

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

#else
	#error Wrong CPUSTYLE_xxx
#endif
}

// подготовка к обмену data path state machine
static void sdhost_dpsm_prepare(uintptr_t addr, uint_fast8_t txmode, uint_fast32_t len, uint_fast32_t lenpower)
{
#if ! WITHSDHCHW
// SPI SD CARD (MMC SD)

#elif CPUSTYLE_R7S721


#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1


	//Clean DPSM interrupt flags and enable DPSM interrupts
	SDMMC1->ICR = (SDMMC_ICR_DBCKENDC | SDMMC_ICR_DATAENDC | SDMMC_ICR_DCRCFAILC | SDMMC_ICR_DTIMEOUTC);
	SDMMC1->MASK |= (SDMMC_MASK_DATAENDIE | SDMMC_MASK_DCRCFAILIE | SDMMC_MASK_DTIMEOUTIE);

	SDMMC1->DLEN = (SDMMC1->DLEN & ~ (SDMMC_DLEN_DATALENGTH_Msk)) |
		((len << SDMMC_DLEN_DATALENGTH_Pos) & SDMMC_DLEN_DATALENGTH_Msk) |
		0;
	SDMMC1->DTIMER = 0x0FFFFFFF;

	SDMMC1->DCTRL =
		//SDMMC_DCTRL_DTEN_Msk |		// Data transfer enabled bit
		! txmode * SDMMC_DCTRL_DTDIR_Msk |		// 1: From card to controller.
		((lenpower * SDMMC_DCTRL_DBLOCKSIZE_0) & SDMMC_DCTRL_DBLOCKSIZE_Msk) |	// 9: 512 bytes, 3: 8 bytes
		0;

#elif CPUSTYLE_STM32F7XX

	SDMMC1->DTIMER = 0x0FFFFFFF;

	SDMMC1->DLEN = (SDMMC1->DLEN & ~ (SDMMC_DLEN_DATALENGTH_Msk)) |
		((len << SDMMC_DLEN_DATALENGTH_Pos) & SDMMC_DLEN_DATALENGTH_Msk) |
		0;

	SDMMC1->DCTRL =
		1 * SDMMC_DCTRL_DTEN_Msk |		// Data transfer enabled bit
		! txmode * SDMMC_DCTRL_DTDIR_Msk |		// 1: From card to controller.
		0 * SDMMC_DCTRL_DTMODE_Msk |		// 0: Block data transfer
		SDMMC_DCTRL_DMAEN_Msk |
		lenpower * SDMMC_DCTRL_DBLOCKSIZE_0 |	// 9: 512 bytes, 3: 8 bytes
		//0 * SDMMC_DCTRL_RWSTART |
		//0 * SDMMC_DCTRL_RWSTOP |
		//1 * SDMMC_DCTRL_RWMOD |			// 1: Read Wait control using SDMMC_CK
		//0 * SDMMC_DCTRL_SDMMCEN |		// If this bit is set, the DPSM performs an SD I/O-card-specific operation.
		0;

#elif CPUSTYLE_STM32F4XX

	SDIO->DTIMER = 0x0FFFFFFF;

	SDIO->DLEN = (len & SDIO_DLEN_DATALENGTH);

	SDIO->DCTRL =
		1 * SDIO_DCTRL_DTEN |		// Data transfer enabled bit
		! txmode * SDIO_DCTRL_DTDIR |		// 1: From card to controller.
		0 * SDIO_DCTRL_DTMODE |		// 0: Block data transfer
		SDIO_DCTRL_DMAEN |
		lenpower * SDIO_DCTRL_DBLOCKSIZE_0 |	// 9: 512 bytes, 3: 8 bytes
		//0 * SDIO_DCTRL_RWSTART |
		//0 * SDIO_DCTRL_RWSTOP |
		//1 * SDIO_DCTRL_RWMOD |			// 1: Read Wait control using SDIO_CK
		//0 * SDIO_DCTRL_SDIOEN |		// If this bit is set, the DPSM performs an SD I/O-card-specific operation.
		0;

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU
	//PRINTF("sdhost_dpsm_prepare: tx=%d, status=%08X, psts=%08X\n", txmode, SD0->INT_STATUS, SD0->PRESENT_STATE);

#else
	#error Wrong CPUSTYLE_xxx
#endif
}


#define DEFAULT_TRANSFER_MODE 0

#define XSDPS_APP_CMD_PREFIX 0
#define CMD0	 0x00U
#define CMD1	 0x01U
#define CMD2	 0x02U
#define CMD3	 0x03U
#define CMD4	 0x04U
#define CMD5	 0x05U
#define CMD6	 0x06U
#define ACMD6	(XSDPS_APP_CMD_PREFIX + 0x06U)
#define CMD7	 0x07U
#define CMD8	 0x08U
#define CMD9	 0x09U	// SD_CMD_SEND_CSD, R2
#define CMD10	 0x0AU
#define CMD11	 0x0BU
#define CMD12	 0x0CU
#define CMD13	 0x0DU	// SD_CMD_SEND_STATUS
#define ACMD13	 (XSDPS_APP_CMD_PREFIX + 0x0DU)
#define CMD16	 0x10U
#define CMD17	 0x11U	// SD_CMD_READ_SINGLE_BLOCK
#define CMD18	 0x12U	// SD_CMD_READ_MULT_BLOCK
#define CMD19	 0x13U
#define CMD20	 0x14U
#define CMD21	 0x15U
#define CMD23	 0x17U
#define ACMD23	 (XSDPS_APP_CMD_PREFIX + 0x17U)
#define CMD24	 0x18U
#define CMD25	 0x19U	// SD_CMD_WRITE_MULT_BLOCK
#define CMD41	 0x29U
#define ACMD41	 (XSDPS_APP_CMD_PREFIX + 0x29U)
#define ACMD42	 (XSDPS_APP_CMD_PREFIX + 0x2AU)
#define ACMD51	 (XSDPS_APP_CMD_PREFIX + 0x33U)
#define CMD52	 0x34U
#define CMD55	 0x37U
#define CMD58	 0x3AU

#define XSDPS_CARD_SD		1U
#define XSDPS_CARD_MMC		2U
#define XSDPS_CARD_SDIO		3U
#define XSDPS_CARD_SDCOMBO	4U
#define XSDPS_CHIP_EMMC		5U

#if CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	/** @name Transfer Mode and Command Register
	 *
	 * The Transfer Mode register is used to control the data transfers and
	 * Command register is used for command generation
	 * Read/Write except for reserved bits.
	 * @{
	 */

	#define XSDPS_TM_DMA_EN_MASK			(0x01uL << 0) /**< DMA Enable */
	#define XSDPS_TM_BLK_CNT_EN_MASK		(0x01uL << 1) /**< Block Count Enable */
	#define XSDPS_TM_AUTO_CMD12_EN_MASK		(0x01uL << 2) /**< Auto CMD12 Enable */
	#define XSDPS_TM_DAT_DIR_SEL_MASK		(0x01uL << 4) /**< Data Transfer Direction Select, 1 - Read (Card to Host) */
	#define XSDPS_TM_MUL_SIN_BLK_SEL_MASK	(0x01uL << 5) /**< Multi/Single Block Select */

	#define XSDPS_CMD_RESP_SEL_MASK			(0x03uL << 16) /**< Response Type Select */
	#define XSDPS_CMD_RESP_NONE_MASK		(0x00uL << 16) /**< No Response */
	#define XSDPS_CMD_RESP_L136_MASK		(0x01uL << 16) /**< Response length 138 */
	#define XSDPS_CMD_RESP_L48_MASK			(0x02uL << 16) /**< Response length 48 */
	#define XSDPS_CMD_RESP_L48_BSY_CHK_MASK	(0x03uL << 16) /**< Response length 48 & check busy after	response */
	#define XSDPS_CMD_CRC_CHK_EN_MASK		(0x01uL << 19) /**< Command CRC Check Enable */
	#define XSDPS_CMD_INX_CHK_EN_MASK		(0x01uL << 20) /**< Command Index Check Enable */
	#define XSDPS_DAT_PRESENT_SEL_MASK		(0x01uL << 21) /**< Data Present Select */
	#define XSDPS_CMD_TYPE_MASK				0x00C00000uL /**< Command Type */
	#define XSDPS_CMD_TYPE_NORM_MASK		0x00000000uL /**< CMD Type - Normal */
	#define XSDPS_CMD_TYPE_SUSPEND_MASK		0x00400000uL /**< CMD Type - Suspend */
	#define XSDPS_CMD_TYPE_RESUME_MASK		0x00800000uL /**< CMD Type - Resume */
	#define XSDPS_CMD_TYPE_ABORT_MASK		0x00C00000uL /**< CMD Type - Abort */
	#define XSDPS_CMD_MASK					0x3F000000uL /**< Command Index Mask - 	Set to CMD0-63,	AMCD0-63 */

	#define RESP_NONE	(XSDPS_CMD_RESP_NONE_MASK)
	#define RESP_R1		(XSDPS_CMD_RESP_L48_MASK | XSDPS_CMD_CRC_CHK_EN_MASK | XSDPS_CMD_INX_CHK_EN_MASK)

	#define RESP_R1B	(XSDPS_CMD_RESP_L48_BSY_CHK_MASK | XSDPS_CMD_CRC_CHK_EN_MASK | XSDPS_CMD_INX_CHK_EN_MASK)

	#define RESP_R2		(XSDPS_CMD_RESP_L136_MASK | XSDPS_CMD_CRC_CHK_EN_MASK)
	#define RESP_R3		(XSDPS_CMD_RESP_L48_MASK)

	#define RESP_R6		(XSDPS_CMD_RESP_L48_BSY_CHK_MASK | XSDPS_CMD_CRC_CHK_EN_MASK | XSDPS_CMD_INX_CHK_EN_MASK)

	#define SDMMC_CMD_CMDINDEX_Pos 24
	#define SDMMC_CMD_CMDSTOP 0

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	#define XSDPS_CMD_RESP_NONE_MASK (0x00uL << SDMMC_CMD_WAITRESP_Pos)	// No response, expect CMDSENT flag
	#define XSDPS_CMD_RESP_L48_MASK (0x01uL << SDMMC_CMD_WAITRESP_Pos)
	#define XSDPS_CMD_RESP_L48_BSY_CHK_MASK (0x01uL << SDMMC_CMD_WAITRESP_Pos)
	#define XSDPS_CMD_RESP_L136_MASK (0x03uL << SDMMC_CMD_WAITRESP_Pos)	// 11: Long response, expect CMDREND or CCRCFAIL flag
	#define XSDPS_CMD_CRC_CHK_EN_MASK (0)	// 10: Short response, expect CMDREND flag (No CRC)
	#define XSDPS_CMD_INX_CHK_EN_MASK (0)

	#define RESP_NONE	(XSDPS_CMD_RESP_NONE_MASK)
	#define RESP_R1		(XSDPS_CMD_RESP_L48_MASK | XSDPS_CMD_CRC_CHK_EN_MASK | XSDPS_CMD_INX_CHK_EN_MASK)

	#define RESP_R1B	(XSDPS_CMD_RESP_L48_BSY_CHK_MASK | XSDPS_CMD_CRC_CHK_EN_MASK | XSDPS_CMD_INX_CHK_EN_MASK)

	#define RESP_R2		(0x03uL << SDMMC_CMD_WAITRESP_Pos)	/*(XSDPS_CMD_RESP_L136_MASK | XSDPS_CMD_CRC_CHK_EN_MASK) */// (0x03uL << SDMMC_CMD_WAITRESP_Pos)	// 11: Long response, expect CMDREND or CCRCFAIL flag
	#define RESP_R3		(0x02uL << SDMMC_CMD_WAITRESP_Pos) /*(XSDPS_CMD_RESP_L48_MASK)*/ // 10: Short response, expect CMDREND flag (No CRC)

	#define RESP_R6		(XSDPS_CMD_RESP_L48_BSY_CHK_MASK | XSDPS_CMD_CRC_CHK_EN_MASK | XSDPS_CMD_INX_CHK_EN_MASK)

	#define XSDPS_DAT_PRESENT_SEL_MASK SDMMC_CMD_CMDTRANS

#endif


#if CPUSTYLE_XC7Z || CPUSTYLE_XCZU

static uint_fast32_t getTransferMode(int txmode, unsigned BlkCnt)
{
	uint_fast32_t v;
	if (txmode)
	{

		// WRITE
		if (BlkCnt == 1U) {
			v =
				XSDPS_TM_MUL_SIN_BLK_SEL_MASK |
				XSDPS_TM_BLK_CNT_EN_MASK |		// Block_Count_Enable (0 - infinite transfer)
				XC7Z_SDRDWRDMA * XSDPS_TM_DMA_EN_MASK |
				0;
		} else {
			v =
				XSDPS_TM_MUL_SIN_BLK_SEL_MASK |
				XSDPS_TM_BLK_CNT_EN_MASK |	// Block_Count_Enable (0 - infinite transfer)
				XC7Z_SDRDWRDMA * XSDPS_TM_DMA_EN_MASK |
				XC7Z_AUTO_CMD12 * XSDPS_TM_AUTO_CMD12_EN_MASK |
				0;
		}
	}
	else
	{
		// READ

		if (BlkCnt == 1U) {
			v =
				XSDPS_TM_DAT_DIR_SEL_MASK |	// 1 - Read (Card to Host)
				XSDPS_TM_MUL_SIN_BLK_SEL_MASK |
				XSDPS_TM_BLK_CNT_EN_MASK |	// Block_Count_Enable (0 - infinite transfer)
				XC7Z_SDRDWRDMA * XSDPS_TM_DMA_EN_MASK |
				0;
		} else {
			v =
				XSDPS_TM_DAT_DIR_SEL_MASK |	// 1 - Read (Card to Host)
				XSDPS_TM_MUL_SIN_BLK_SEL_MASK |
				XSDPS_TM_BLK_CNT_EN_MASK |		// Block_Count_Enable (0 - infinite transfer)
				XC7Z_SDRDWRDMA * XSDPS_TM_DMA_EN_MASK |
				XC7Z_AUTO_CMD12 * XSDPS_TM_AUTO_CMD12_EN_MASK |
				0;
		}
	}
	return v;
}

#else

static uint_fast32_t getTransferMode(int txmode, unsigned BlkCnt)
{
	return DEFAULT_TRANSFER_MODE;
}

#endif /* CPUSTYLE_XC7Z || CPUSTYLE_XCZU */

// CPUSTYLE_R7S721 SD_CMD bits
// 
// SD_CMD[15:14]	MD7:MD6		Multiple Block Transfer Mode (enabled at multiple block transfer)
// SD_CMD[13]		MD5			Single/Multiple Block Transfer (enabled when the command with data is handled)
// SD_CMD[12]		MD4			Write/Read Mode (enabled when the command with data is handled)
// SD_CMD[11]		MD3			Data Mode (Command Type)
// SD_CMD[10..8]	MD2:MD0		Mode/Response Type
// SD_CMD[7..6]		C1:C0		00: CMD, 01: ACMD
// SD_CMD[5..0]		CF45:CF40	Command Index
#define R7S721_SD_CMD_ACMD_bm	(1U << 6)

static portholder_t encode_cmd(uint_fast8_t cmd, uint_fast32_t TransferMode)
{
#if ! WITHSDHCHW
// SPI SD CARD (MMC SD)
	return (cmd & 0x3f) | 0x40;

#elif CPUSTYLE_R7S721

	switch (cmd)
	{
	case 5:					// CMD5 SD_CMD_SDIO_SEN_OP_COND
		return 0x0705;

	case SD_CMD_HS_SWITCH:	// CMD6
		return 0x1C06;

	case 8:					// CMD8 SD_CMD_HS_SEND_EXT_CSD
		return 0x0408;

	case SD_CMD_SDIO_RW_DIRECT:	// CMD52
		return 0x0434;

	case SD_CMD_SDIO_RW_EXTENDED:	// command have four versions in depends of mode
		return cmd;

	default:
		break;
	}
//#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
//	switch (cmd)
//	{
//	case SD_CMD_WRITE_SINGLE_BLOCK:
//	case SD_CMD_WRITE_MULT_BLOCK:
//	case SD_CMD_READ_SINGLE_BLOCK:
//	case SD_CMD_READ_MULT_BLOCK:
//		return cmd | SDMMC_CMD_CMDTRANS;
//	case SD_CMD_STOP_TRANSMISSION:
//		return cmd | SDMMC_CMD_CMDSTOP;
//	default:
//		return cmd;
//	}

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
	// возврат значения подготовленного для записи в регистр CMD_TRANSFER_MODE

	uint_fast32_t RetVal = (cmd & 0x3FuL) << SDMMC_CMD_CMDINDEX_Pos;
	unsigned cardType = XSDPS_CARD_SD;
	switch(cmd) {
	case CMD0:
		RetVal |= RESP_NONE;
		break;
	case CMD1:
		RetVal |= RESP_R3;
		break;
	case CMD2:
		RetVal |= RESP_R2;
		break;
	case CMD3:
		if (cardType == XSDPS_CARD_SD) {
			RetVal |= RESP_R6;
		} else {
			RetVal |= RESP_R1;
		}
		break;
	case CMD4:
		RetVal |= RESP_NONE;
		break;
	case CMD5:
		RetVal |= RESP_R1B;
		break;
	case CMD6:
		if (cardType == XSDPS_CARD_SD) {
			RetVal |= RESP_R1 | XSDPS_DAT_PRESENT_SEL_MASK;
		} else {
			RetVal |= RESP_R1B;
		}
		break;
	case CMD7:
		RetVal |= RESP_R1;
		break;
	case CMD8:
		if (cardType == XSDPS_CARD_SD) {
			RetVal |= RESP_R1;
		} else {
			RetVal |= RESP_R1 | XSDPS_DAT_PRESENT_SEL_MASK;
		}
		break;
	case CMD9:	// SD_CMD_SEND_CSD
		RetVal |= RESP_R2;
		break;
	case CMD11:
	case CMD10:
	case CMD12:
		RetVal |= RESP_R1;
		RetVal |= SDMMC_CMD_CMDSTOP;
		break;
	case CMD13:	// SD_CMD_SEND_STATUS
		RetVal |= RESP_R1;
		break;
	case CMD16:
		RetVal |= RESP_R1;
		break;
	case CMD20:
		RetVal |= RESP_R1B;
		break;
	case CMD17:	// SD_CMD_READ_SINGLE_BLOCK
	case CMD18: // SD_CMD_READ_MULT_BLOCK
	case CMD19:
	case CMD21:
	case CMD23:
	case CMD24:
	case CMD25:	// SD_CMD_WRITE_MULT_BLOCK
		RetVal |= RESP_R1 | XSDPS_DAT_PRESENT_SEL_MASK;
		break;
	case CMD52:
	case CMD55:
		RetVal |= RESP_R1;
		break;
	case CMD58:
		break;
	default :
		PRINTF("Wrong case: cmd=%02X\n", cmd);
		ASSERT(0);
		break;
	}

#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
	RetVal |=
		0 * SDMMC_CMD_WAITINT |
		0 * SDMMC_CMD_WAITPEND |
		1 * SDMMC_CMD_CPSMEN |
		0;
#endif /* CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1 */

	return RetVal | TransferMode;

#endif
	return cmd;
}

static portholder_t encode_appcmd(uint_fast8_t cmd, uint_fast32_t TransferMode)
{
#if ! WITHSDHCHW
// SPI SD CARD (MMC SD)
	return (cmd & 0x3f) | 0x40;

#elif CPUSTYLE_R7S721
	switch (cmd)
	{
	case SD_CMD_APP_SD_SET_BUSWIDTH:		// ACMD6
		return 6 | R7S721_SD_CMD_ACMD_bm;	// 0x0046

	case SD_CMD_SD_APP_STATUS:				// ACMD13
		return 13 | R7S721_SD_CMD_ACMD_bm;	// 0x004D

	case SD_CMD_SD_APP_SEND_NUM_WRITE_BLOCKS:// ACMD22
		return 22 | R7S721_SD_CMD_ACMD_bm;

	case 23:								// ACMD23
		return 23 | R7S721_SD_CMD_ACMD_bm;	// 0x0057

	case SD_CMD_SD_APP_OP_COND:				// ACMD41
		return 0x0069;

	case SD_CMD_SD_APP_SET_CLR_CARD_DETECT:	// ACMD42
		return 0x006A;

	case SD_CMD_SD_APP_SEND_SCR:			// ACMD51
		return 0x0073;

	default:
		return (cmd & 0x3f) | R7S721_SD_CMD_ACMD_bm;
	}

//#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
//	switch (cmd)
//	{
//	case SD_CMD_SD_APP_SEND_SCR:
//	case SD_CMD_SD_APP_STATUS:
//		return cmd | SDMMC_CMD_CMDTRANS;
//	default:
//		return cmd;
//	}

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
	// возврат значения подготовленного для записи в регистр CMD_TRANSFER_MODE

	uint_fast32_t RetVal = (cmd & 0x3FuL) << SDMMC_CMD_CMDINDEX_Pos;
	unsigned cardType = XSDPS_CARD_SD;
	switch(cmd) {
	case ACMD6:
		RetVal |= RESP_R1;
		break;
	case ACMD13:
		RetVal |= RESP_R1 | (uint_fast32_t)XSDPS_DAT_PRESENT_SEL_MASK;
		break;
	case ACMD23:
		RetVal |= RESP_R1 | (uint_fast32_t)XSDPS_DAT_PRESENT_SEL_MASK;
		break;
	case ACMD41:
		RetVal |= RESP_R3;
		break;
	case ACMD42:
		RetVal |= RESP_R1;
		break;
	case ACMD51:
		RetVal |= RESP_R1 | XSDPS_DAT_PRESENT_SEL_MASK;
		break;
	default :
		PRINTF("Wrong case: acmd=%02X\n", cmd);
		ASSERT(0);
		break;
	}

#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
	RetVal |=
		0 * SDMMC_CMD_WAITINT |
		0 * SDMMC_CMD_WAITPEND |
		1 * SDMMC_CMD_CPSMEN |
		0;
#endif /* CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1 */

	return RetVal | TransferMode;

#endif
	return cmd;
}

// Запустить на выполнение команду, не возвращающую responce
static void sdhost_no_resp(portholder_t cmd, uint_fast32_t arg)
{
#if ! WITHSDHCHW
// SPI SD CARD (MMC SD)
	uint_fast8_t crc = 0x00;

	crc = crc7b8(crc, cmd);		
	crc = crc7b8(crc, arg >> 24);	
	crc = crc7b8(crc, arg >> 16);	
	crc = crc7b8(crc, arg >> 8);	
	crc = crc7b8(crc, arg >> 0);	

	SDCARD_CS_LOW();
	spi_progval8_p1(targetsdcard, 0xff);	// dummy byte
	spi_progval8_p2(targetsdcard, cmd);	// command
	spi_progval8_p2(targetsdcard, arg >> 24);
	spi_progval8_p2(targetsdcard, arg >> 16);
	spi_progval8_p2(targetsdcard, arg >> 8);
	spi_progval8_p2(targetsdcard, arg >> 0);
	spi_progval8_p2(targetsdcard, crc | 0x01);		// CRC and end transmit bit

	spi_complete(targetsdcard);
	SDCARD_CS_HIGH();

#elif CPUSTYLE_R7S721

	while ((SDHI0.SD_INFO2 & (1u << 14)) != 0)	// CBSY
		; //PRINTF(PSTR("sdhost_no_resp: CBSY\n"));

	SDHI0.SD_INFO1_MASK = 0xFFFE;
	SDHI0.SD_INFO2_MASK = 0x7F80;

	// Issue command
	SDHI0.SD_ARG0 = arg >> 0;
	SDHI0.SD_ARG1 = arg >> 16;
	SDHI0.SD_CMD = cmd;				// Выполнение команды начинается после записи в этот регистр

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	SDMMC1->ARG = arg;
	SDMMC1->CMD = cmd;

#elif CPUSTYLE_STM32F7XX

	SDMMC1->ARG = arg;
	SDMMC1->CMD = cmd;

#elif CPUSTYLE_STM32F4XX

	SDIO->ARG = arg;
	SDIO->CMD = 
		(cmd) |
		//0 * SDIO_CMD_WAITRESP_0 |	// 0: no response, 1: short response, 3: long response
		0 * SDIO_CMD_WAITINT |
		0 * SDIO_CMD_WAITPEND |
		1 * SDIO_CMD_CPSMEN |
		0 * SDIO_CMD_SDIOSUSPEND |
		0;

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	// sdhost_no_resp
	SD0->ARG = arg;
	SD0->INT_STATUS = ~ 0;
	SD0->CMD_TRANSFER_MODE = cmd;

#else
	#error Wrong CPUSTYLE_xxx
#endif
}

// Запустить на выполнение команду, возвращающую short responce
// Команда без пересылки данных
static void sdhost_short_resp(portholder_t cmd, uint_fast32_t arg, uint_fast8_t nocrc)
{
#if ! WITHSDHCHW
// SPI SD CARD (MMC SD)

	uint_fast8_t crc = 0x00;

	crc = crc7b8(crc, cmd);		
	crc = crc7b8(crc, arg >> 24);	
	crc = crc7b8(crc, arg >> 16);	
	crc = crc7b8(crc, arg >> 8);	
	crc = crc7b8(crc, arg >> 0);	

	SDCARD_CS_LOW();
	spi_progval8_p1(targetsdcard, 0xff);	// dummy byte
	spi_progval8_p2(targetsdcard, cmd);	// command
	spi_progval8_p2(targetsdcard, arg >> 24);
	spi_progval8_p2(targetsdcard, arg >> 16);
	spi_progval8_p2(targetsdcard, arg >> 8);
	spi_progval8_p2(targetsdcard, arg >> 0);
	spi_progval8_p2(targetsdcard, crc | 0x01);		// CRC and end transmit bit

	spi_complete(targetsdcard);
	SDCARD_CS_HIGH();

#elif CPUSTYLE_R7S721

	while ((SDHI0.SD_INFO2 & (1u << 14)) != 0)	// CBSY
		; //PRINTF(PSTR("sdhost_short_resp: CBSY\n"));

	SDHI0.SD_INFO1_MASK = 0xFFFE;
	SDHI0.SD_INFO2_MASK = 0x7F80;

	// Issue command
	SDHI0.SD_ARG0 = arg >> 0;
	SDHI0.SD_ARG1 = arg >> 16;
	SDHI0.SD_CMD = cmd;				// Выполнение команды начинается после записи в этот регистр

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	SDMMC1->ARG = arg;
	SDMMC1->CMD = cmd;

#elif CPUSTYLE_STM32F7XX

	SDMMC1->ARG = arg;
	SDMMC1->CMD = cmd;

#elif CPUSTYLE_STM32F4XX

	SDIO->ARG = arg;
	SDIO->CMD = 
		(cmd) |
		//1 * SDIO_CMD_WAITRESP_0 |	// 0: no response, 1: short response, 3: long response
		0 * SDIO_CMD_WAITINT |
		0 * SDIO_CMD_WAITPEND |
		1 * SDIO_CMD_CPSMEN |
		0 * SDIO_CMD_SDIOSUSPEND |
		0;

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	// sdhost_short_resp
	SD0->ARG = arg;
	SD0->INT_STATUS = ~ 0;
	SD0->CMD_TRANSFER_MODE = cmd;

#else
	#error Wrong CPUSTYLE_xxx
#endif
}

// Запустить на выполнение команду, возвращающую long responce
static void sdhost_long_resp(portholder_t cmd, uint_fast32_t arg)
{
#if ! WITHSDHCHW
// SPI SD CARD (MMC SD)

	uint_fast8_t crc = 0x00;

	crc = crc7b8(crc, cmd);		
	crc = crc7b8(crc, arg >> 24);	
	crc = crc7b8(crc, arg >> 16);	
	crc = crc7b8(crc, arg >> 8);	
	crc = crc7b8(crc, arg >> 0);	

	SDCARD_CS_LOW();
	spi_progval8_p1(targetsdcard, 0xff);	// dummy byte
	spi_progval8_p2(targetsdcard, cmd);	// command
	spi_progval8_p2(targetsdcard, arg >> 24);
	spi_progval8_p2(targetsdcard, arg >> 16);
	spi_progval8_p2(targetsdcard, arg >> 8);
	spi_progval8_p2(targetsdcard, arg >> 0);
	spi_progval8_p2(targetsdcard, crc | 0x01);		// CRC and end transmit bit

	spi_complete(targetsdcard);
	SDCARD_CS_HIGH();

#elif CPUSTYLE_R7S721

	while ((SDHI0.SD_INFO2 & (1u << 14)) != 0)	// CBSY
		; //PRINTF(PSTR("sdhost_long_resp: CBSY\n"));

	SDHI0.SD_INFO1_MASK = 0xFFFE;
	SDHI0.SD_INFO2_MASK = 0x7F80;

	// Issue command
	SDHI0.SD_ARG0 = arg >> 0;
	SDHI0.SD_ARG1 = arg >> 16;
	SDHI0.SD_CMD = cmd;				// Выполнение команды начинается после записи в этот регистр

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	SDMMC1->ARG = arg;
	SDMMC1->CMD = cmd;

#elif CPUSTYLE_STM32F7XX

	SDMMC1->ARG = arg;
	SDMMC1->CMD = cmd;

#elif CPUSTYLE_STM32F4XX

	SDIO->ARG = arg;
	SDIO->CMD = 
		(cmd) |
		//3 * SDIO_CMD_WAITRESP_0 |	// 0: no response, 1: short response, 3: long response
		0 * SDIO_CMD_WAITINT |
		0 * SDIO_CMD_WAITPEND |
		1 * SDIO_CMD_CPSMEN |
		0 * SDIO_CMD_SDIOSUSPEND |
		0;

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	// sdhost_long_resp
	SD0->ARG = arg;
	SD0->INT_STATUS = ~ 0;
	SD0->CMD_TRANSFER_MODE = cmd;

#else
	#error Wrong CPUSTYLE_xxx
#endif
}

// Проверка совпадения ответа с кодом команды
static uint_fast8_t sdhost_verify_resp(uint_fast8_t cmd)
{
#if ! WITHSDHCHW
// SPI SD CARD (MMC SD)
	return 0;

#elif CPUSTYLE_R7S721

	return 0;

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	if ((SDMMC1->RESPCMD & SDMMC_RESPCMD_RESPCMD) != (cmd & SDMMC_CMD_CMDINDEX))
	{
		PRINTF(PSTR("sdhost_verify_resp error, RESPCMD=%02X, expeted %02X\n"), SDMMC1->RESPCMD & SDMMC_RESPCMD_RESPCMD, cmd & SDMMC_CMD_CMDINDEX);
		return 1;
	}
	return 0;

#elif CPUSTYLE_STM32F7XX

	if ((SDMMC1->RESPCMD & SDMMC_RESPCMD_RESPCMD) != (cmd & SDMMC_CMD_CMDINDEX))
	{
		PRINTF(PSTR("sdhost_verify_resp error, RESPCMD=%02X, expeted %02X\n"), SDMMC1->RESPCMD & SDMMC_RESPCMD_RESPCMD, cmd & SDMMC_CMD_CMDINDEX);
		return 1;
	}
	return 0;

#elif CPUSTYLE_STM32F4XX

	if ((SDIO->RESPCMD & SDIO_RESPCMD_RESPCMD) != (cmd & SDIO_CMD_CMDINDEX))
	{
		PRINTF(PSTR("sdhost_verify_resp error, RESPCMD=%02X, expeted %02X\n"), SDIO->RESPCMD & SDIO_RESPCMD_RESPCMD, cmd & SDIO_CMD_CMDINDEX);
		return 1;
	}
	return 0;

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU
	return 0;

#else
	#error Wrong CPUSTYLE_xxx
	return 1;
#endif
}
// Дождаться окончания выполнения команды (без обмена данными и без получения ответа) и вернуть код ошибки
static uint_fast8_t sdhost_get_none_resp(void)
{
#if ! WITHSDHCHW
// SPI SD CARD (MMC SD)
	SDCARD_CS_LOW();
	spi_read_byte(targetsdcard, 0xff);
	spi_read_byte(targetsdcard, 0xff);
	spi_read_byte(targetsdcard, 0xff);
	spi_read_byte(targetsdcard, 0xff);
	spi_read_byte(targetsdcard, 0xff);
	SDCARD_CS_HIGH();
	return 0;

#elif CPUSTYLE_R7S721

	uint_fast8_t ec = 0;

	while ((SDHI0.SD_INFO1 & (1U << 0)) == 0)	// INFO0
		;
	ec = (SDHI0.SD_ERR_STS2 & 0x007F) != 0;

	SDHI0.SD_INFO1 = (uint16_t) ~ (1U << 0);	// INFO0=0
	//SDHI0.SD_INFO1 = (uint16_t) ~ (1U << 2);	// INFO2=0

	//PRINTF(PSTR("Aftrer command w=%lu %04X %04X %04X\n"), w, SDHI0.SD_CMD, SDHI0.SD_ARG0, SDHI0.SD_ARG1);
	//PRINTF(PSTR("SD_INFO1=%04X, SD_INFO2=%04X\n"), SDHI0.SD_INFO1, SDHI0.SD_INFO2);	// SD_INFO1=0600, SD_INFO2=2080
	//PRINTF(PSTR("SD_ERR_STS1=%04X, SD_ERR_STS2=%04X\n"), SDHI0.SD_ERR_STS1, SDHI0.SD_ERR_STS2);	// SD_ERR_STS1=0000, SD_ERR_STS2=2000
	//PRINTF(PSTR("SD_SIZE=%04X\n"), SDHI0.SD_SIZE);
	
	return ec;

#elif CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	uint_fast8_t ec = 0;
	while (ec == 0)
	{
		const uint_fast32_t sta = SDMMC1->STA;
		if ((sta & SDMMC_STA_CTIMEOUT) != 0)
			ec = 1;
		if ((sta & SDMMC_STA_CMDSENT) != 0)
			break;
#if defined (SDMMC_STA_DTIMEOUT)
		if ((sta & SDMMC_STA_DTIMEOUT) != 0)
			ec = 1;
#endif /* defined (SDMMC_STA_DTIMEOUT) */
#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
		if ((sta & SDMMC_STA_BUSYD0END) != 0)
			break;
#endif /* CPUSTYLE_STM32H7XX */
	}
	
	SDMMC1->ICR = SDMMC_ICR_CMDSENTC;

	// Если была ошибка CRC при приёме ответа - сбросить её
	if ((SDMMC1->STA & SDMMC_STA_CCRCFAIL) != 0)
		SDMMC1->ICR = SDMMC_ICR_CCRCFAILC;
	// Если была ошибка таймаута при приёме ответа - сбросить её
	if ((SDMMC1->STA & SDMMC_STA_CTIMEOUT) != 0)
		SDMMC1->ICR = SDMMC_ICR_CTIMEOUTC;

	SDMMC1->ICR = SDIO_STATIC_FLAGS;
	return 0;

#elif CPUSTYLE_STM32F4XX

	uint_fast8_t ec = 0;
	while (ec == 0)
	{
		const uint_fast32_t sta = SDIO->STA;
		if ((sta & SDIO_STA_CTIMEOUT) != 0)
			ec = 1;
		if ((sta & SDIO_STA_CMDSENT) != 0)
			break;
	}
	
	SDIO->ICR = SDIO_ICR_CMDSENTC;

	// Если была ошибка CRC при приёме ответа - сбросить её
	if ((SDIO->STA & SDIO_STA_CCRCFAIL) != 0)
		SDIO->ICR = SDIO_ICR_CCRCFAILC;
	// Если была ошибка таймаута при приёме ответа - сбросить её
	if ((SDIO->STA & SDIO_STA_CTIMEOUT) != 0)
		SDIO->ICR = SDIO_ICR_CTIMEOUTC;

	SDIO->ICR = SDIO_STATIC_FLAGS;
	return 0;

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU
	// sdhost_get_none_resp
	return zynq_wait_for_command();

#else
	#error Wrong CPUSTYLE_xxx
	return 1;
#endif
}

// Дождаться окончания выполнения команды (без обмена данными) и вернуть код ошибки
static uint_fast8_t sdhost_get_resp(void)
{
#if ! WITHSDHCHW
// SPI SD CARD (MMC SD)

	unsigned long i;
	uint_fast8_t response;

	SDCARD_CS_LOW();
	for (i = 0; i <= RESPLIMIT; ++ i)
	{
		response = spi_read_byte(targetsdcard, 0xff);
		if ((response & 0x80) != 0)
			continue;
		return 0;
	}
	SDCARD_CS_HIGH();
	return 1;

#elif CPUSTYLE_R7S721

	uint_fast8_t ec = 0;

	while ((SDHI0.SD_INFO1 & (1U << 0)) == 0)	// INFO0
		;
	ec = (SDHI0.SD_ERR_STS2 & 0x007F) != 0;

	SDHI0.SD_INFO1 = (uint16_t) ~ (1U << 0);	// INFO0=0
	//SDHI0.SD_INFO1 = (uint16_t) ~ (1U << 2);	// INFO2=0

	//PRINTF(PSTR("Aftrer command w=%lu %04X %04X %04X\n"), w, SDHI0.SD_CMD, SDHI0.SD_ARG0, SDHI0.SD_ARG1);
	//PRINTF(PSTR("SD_INFO1=%04X, SD_INFO2=%04X\n"), SDHI0.SD_INFO1, SDHI0.SD_INFO2);	// SD_INFO1=0600, SD_INFO2=2080
	//PRINTF(PSTR("SD_ERR_STS1=%04X, SD_ERR_STS2=%04X\n"), SDHI0.SD_ERR_STS1, SDHI0.SD_ERR_STS2);	// SD_ERR_STS1=0000, SD_ERR_STS2=2000
	//PRINTF(PSTR("SD_SIZE=%04X\n"), SDHI0.SD_SIZE);
	
	return ec;

#elif CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	uint_fast8_t ec = 0;
	while (ec == 0)
	{
		const uint_fast32_t sta = SDMMC1->STA;
		if ((sta & SDMMC_STA_CTIMEOUT) != 0)
			ec = 1;
		if ((sta & SDMMC_STA_CCRCFAIL) != 0)
			ec = 1;
		if ((sta & SDMMC_STA_CMDREND) != 0)
			break;
#if defined (SDMMC_STA_DTIMEOUT)
		if ((sta & SDMMC_STA_DTIMEOUT) != 0)
			ec = 1;
#endif /* defined (SDMMC_STA_DTIMEOUT) */
#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
		if ((sta & SDMMC_STA_BUSYD0END) != 0)
			break;
#endif /* CPUSTYLE_STM32H7XX */
	}
	if (ec != 0)
	{
		PRINTF(PSTR("sdhost_get_resp error, STA=%08X, DCOUNT=%08X\n"), SDMMC1->STA, SDMMC1->DCOUNT & SDMMC_DCOUNT_DATACOUNT);
	}
	SDMMC1->ICR = SDMMC_ICR_CMDRENDC;
	// Если была ошибка CRC при приёме ответа - сбросить её
	if ((SDMMC1->STA & SDMMC_STA_CCRCFAIL) != 0)
		SDMMC1->ICR = SDMMC_ICR_CCRCFAILC;
	// Если была ошибка таймаута при приёме ответа - сбросить её
	if ((SDMMC1->STA & SDMMC_STA_CTIMEOUT) != 0)
		SDMMC1->ICR = SDMMC_ICR_CTIMEOUTC;
#if defined (SDMMC_STA_DTIMEOUT)
	if ((SDMMC1->STA & SDMMC_STA_DTIMEOUT) != 0)
		SDMMC1->ICR = SDMMC_ICR_DTIMEOUTC;
#endif /* defined (SDMMC_STA_DTIMEOUT) */

	SDMMC1->ICR = SDIO_STATIC_FLAGS;
	return ec;

#elif CPUSTYLE_STM32F4XX

	uint_fast8_t ec = 0;
	while (ec == 0)
	{
		const uint_fast32_t sta = SDIO->STA;
		if ((sta & SDIO_STA_CTIMEOUT) != 0)
			ec = 1;
		if ((sta & SDIO_STA_CCRCFAIL) != 0)
			ec = 1;
		if ((sta & SDIO_STA_CMDREND) != 0)
			break;
	}
	if (ec != 0)
	{
			//PRINTF(PSTR("sdhost_get_resp error, STA=%08X\n"), SDIO->STA);
	}

	SDIO->ICR = SDIO_ICR_CMDRENDC;
	// Если была ошибка CRC при приёме ответа - сбросить её
	if ((SDIO->STA & SDIO_STA_CCRCFAIL) != 0)
		SDIO->ICR = SDIO_ICR_CCRCFAILC;
	// Если была ошибка таймаута при приёме ответа - сбросить её
	if ((SDIO->STA & SDIO_STA_CTIMEOUT) != 0)
		SDIO->ICR = SDIO_ICR_CTIMEOUTC;

	SDIO->ICR = SDIO_STATIC_FLAGS;
	return ec;

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU
	// sdhost_get_resp
	return zynq_wait_for_command();

#else
	#error Wrong CPUSTYLE_xxx
	return 1;
#endif
}

// Дождаться окончания выполнения команды (без обмена данными и без получения ответа) и вернуть код ошибки
// Ответ передается без сформирванного CRC, поэтому данная ошибка игнорируется.
// All responses, except for the R3 response type, are protected by a CRC.
static uint_fast8_t sdhost_get_resp_nocrc(void)
{
#if ! WITHSDHCHW
// SPI SD CARD (MMC SD)

	unsigned long i;
	uint_fast8_t response;

	SDCARD_CS_LOW();
	for (i = 0; i <= RESPLIMIT; ++ i)
	{
		response = spi_read_byte(targetsdcard, 0xff);
		if ((response & 0x80) != 0)
			continue;
		return 0;
	}
	SDCARD_CS_HIGH();
	return 1;

#elif CPUSTYLE_R7S721

	uint_fast8_t ec = 0;

	while ((SDHI0.SD_INFO1 & (1U << 0)) == 0)	// INFO0
		;
	ec = (SDHI0.SD_ERR_STS2 & 0x007F) != 0;


	SDHI0.SD_INFO1 = (uint16_t) ~ (1U << 0);	// INFO0=0
	//SDHI0.SD_INFO1 = (uint16_t) ~ (1U << 2);	// INFO2=0

	//PRINTF(PSTR("Aftrer command w=%lu %04X %04X %04X\n"), w, SDHI0.SD_CMD, SDHI0.SD_ARG0, SDHI0.SD_ARG1);
	//PRINTF(PSTR("SD_INFO1=%04X, SD_INFO2=%04X\n"), SDHI0.SD_INFO1, SDHI0.SD_INFO2);	// SD_INFO1=0600, SD_INFO2=2080
	//PRINTF(PSTR("SD_ERR_STS1=%04X, SD_ERR_STS2=%04X\n"), SDHI0.SD_ERR_STS1, SDHI0.SD_ERR_STS2);	// SD_ERR_STS1=0000, SD_ERR_STS2=2000
	//PRINTF(PSTR("SD_SIZE=%04X\n"), SDHI0.SD_SIZE);
	
	return ec;

#elif CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	uint_fast8_t ec = 0;
	while (ec == 0)
	{
		const uint_fast32_t sta = SDMMC1->STA;
		if ((sta & SDMMC_STA_CTIMEOUT) != 0)
			ec = 1;
		if ((sta & SDMMC_STA_CCRCFAIL) != 0)
			break; //ec = 1;
		if ((sta & SDMMC_STA_CMDREND) != 0)
			break;
#if defined (SDMMC_STA_DTIMEOUT)
		if ((sta & SDMMC_STA_DTIMEOUT) != 0)
			ec = 1;
#endif /* defined (SDMMC_STA_DTIMEOUT) */
#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
		if ((sta & SDMMC_STA_BUSYD0END) != 0)
			break;
#endif /* CPUSTYLE_STM32H7XX */
	}

	SDMMC1->ICR = SDMMC_ICR_CMDRENDC;
	// Если была ошибка CRC при приёме ответа - сбросить её
	if ((SDMMC1->STA & SDMMC_STA_CCRCFAIL) != 0)
		SDMMC1->ICR = SDMMC_ICR_CCRCFAILC;
	// Если была ошибка таймаута при приёме ответа - сбросить её
	if ((SDMMC1->STA & SDMMC_STA_CTIMEOUT) != 0)
		SDMMC1->ICR = SDMMC_ICR_CTIMEOUTC;

	SDMMC1->ICR = SDIO_STATIC_FLAGS;
	return ec;

#elif CPUSTYLE_STM32F4XX

	uint_fast8_t ec = 0;
	while (ec == 0)
	{
		const uint_fast32_t sta = SDIO->STA;
		if ((sta & SDIO_STA_CTIMEOUT) != 0)
			ec = 1;
		if ((sta & SDIO_STA_CCRCFAIL) != 0)
			break; //ec = 1;
		if ((sta & SDIO_STA_CMDREND) != 0)
			break;
	}

	SDIO->ICR = SDIO_ICR_CMDRENDC;
	// Если была ошибка CRC при приёме ответа - сбросить её
	if ((SDIO->STA & SDIO_STA_CCRCFAIL) != 0)
		SDIO->ICR = SDIO_ICR_CCRCFAILC;
	// Если была ошибка таймаута при приёме ответа - сбросить её
	if ((SDIO->STA & SDIO_STA_CTIMEOUT) != 0)
		SDIO->ICR = SDIO_ICR_CTIMEOUTC;

	SDIO->ICR = SDIO_STATIC_FLAGS;
	return ec;

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU
	// sdhost_get_resp_nocrc
	return zynq_wait_for_command();

#else
	#error Wrong CPUSTYLE_xxx
	return 1;
#endif
}

static uint_fast32_t sdhost_get_resp32bit(void)
{
#if ! WITHSDHCHW
// SPI SD CARD (MMC SD)
	unsigned long r = 0;
	r = r * 256 + spi_read_byte(targetsdcard, 0xff);
	r = r * 256 + spi_read_byte(targetsdcard, 0xff);
	r = r * 256 + spi_read_byte(targetsdcard, 0xff);
	r = r * 256 + spi_read_byte(targetsdcard, 0xff);
	SDCARD_CS_HIGH();
	return r;

#elif CPUSTYLE_R7S721
	// SD_RSP00: R23 to R8
	// SD_RSP01: R39 to R24
	// SD_RSP02: R55 to R40
	// SD_RSP03: R71 to R56
	// SD_RSP04: R87 to R72
	// SD_RSP05: R103 to R88
	// SD_RSP06: R119 to 104
	// SD_RSP07: R127 to 120 (8 bit)
	return ((uint32_t) SDHI0.SD_RSP01 << 16) | SDHI0.SD_RSP00;	// [39:8] OCR register

#elif CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	return SDMMC1->RESP1;	// Card Status [39:8]

#elif CPUSTYLE_STM32F4XX

	return SDIO->RESP1;		// Card Status[39:8]

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	return SD0->RESP_0;	// Card Status [39:8]

#else
	#error Wrong CPUSTYLE_xxx
	return 0;
#endif
}

// Response length 136
static void sdhost_get_resp128bit(uint8_t * resp128)
{
#if ! WITHSDHCHW
// SPI SD CARD (MMC SD)
	resp128 [0] = spi_read_byte(targetsdcard, 0xff);
	resp128 [1] = spi_read_byte(targetsdcard, 0xff);
	resp128 [2] = spi_read_byte(targetsdcard, 0xff);
	resp128 [3] = spi_read_byte(targetsdcard, 0xff);
	resp128 [4] = spi_read_byte(targetsdcard, 0xff);
	resp128 [5] = spi_read_byte(targetsdcard, 0xff);
	resp128 [6] = spi_read_byte(targetsdcard, 0xff);
	resp128 [7] = spi_read_byte(targetsdcard, 0xff);
	resp128 [8] = spi_read_byte(targetsdcard, 0xff);
	resp128 [9] = spi_read_byte(targetsdcard, 0xff);
	resp128 [10] = spi_read_byte(targetsdcard, 0xff);
	resp128 [11] = spi_read_byte(targetsdcard, 0xff);
	resp128 [12] = spi_read_byte(targetsdcard, 0xff);
	resp128 [13] = spi_read_byte(targetsdcard, 0xff);
	resp128 [14] = spi_read_byte(targetsdcard, 0xff);
	resp128 [15] = 0;				// CRC-7 and stop bit
	SDCARD_CS_HIGH();

#elif CPUSTYLE_R7S721
		//sdhost_get_R2: SD_RSP[127..8]=1D4144534420202002B0110DDA00E9
		// SD_RSP00: R23 to R8
		// SD_RSP01: R39 to R24
		// SD_RSP02: R55 to R40
		// SD_RSP03: R71 to R56
		// SD_RSP04: R87 to R72
		// SD_RSP05: R103 to R88
		// SD_RSP06: R119 to 104
		// SD_RSP07: R127 to 120 (8 bit)
		resp128 [0] = SDHI0.SD_RSP07;
		resp128 [1] = SDHI0.SD_RSP06 >> 8;
		resp128 [2] = SDHI0.SD_RSP06 >> 0;
		resp128 [3] = SDHI0.SD_RSP05 >> 8;
		resp128 [4] = SDHI0.SD_RSP05 >> 0;
		resp128 [5] = SDHI0.SD_RSP04 >> 8;
		resp128 [6] = SDHI0.SD_RSP04 >> 0;
		resp128 [7] = SDHI0.SD_RSP03 >> 8;
		resp128 [8] = SDHI0.SD_RSP03 >> 0;
		resp128 [9] = SDHI0.SD_RSP02 >> 8;
		resp128 [10] = SDHI0.SD_RSP02 >> 0;
		resp128 [11] = SDHI0.SD_RSP01 >> 8;
		resp128 [12] = SDHI0.SD_RSP01 >> 0;
		resp128 [13] = SDHI0.SD_RSP00 >> 8;
		resp128 [14] = SDHI0.SD_RSP00 >> 0;
		resp128 [15] = 0;				// CRC-7 and stop bit

#elif CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

		// RESP1;Card Status [127:96]
		// RESP2;Card Status [95:64]
		// RESP3;Card Status [63:32]
		// RESP4;Card Status [31:1]0b
		
		resp128 [0] = SDMMC1->RESP1 >> 24;
		resp128 [1] = SDMMC1->RESP1 >> 16;
		resp128 [2] = SDMMC1->RESP1 >> 8;
		resp128 [3] = SDMMC1->RESP1 >> 0;

		resp128 [4] = SDMMC1->RESP2 >> 24;
		resp128 [5] = SDMMC1->RESP2 >> 16;
		resp128 [6] = SDMMC1->RESP2 >> 8;
		resp128 [7] = SDMMC1->RESP2 >> 0;

		resp128 [8] = SDMMC1->RESP3 >> 24;
		resp128 [9] = SDMMC1->RESP3 >> 16;
		resp128 [10] = SDMMC1->RESP3 >> 8;
		resp128 [11] = SDMMC1->RESP3 >> 0;

		resp128 [12] = SDMMC1->RESP4 >> 24;
		resp128 [13] = SDMMC1->RESP4 >> 16;
		resp128 [14] = SDMMC1->RESP4 >> 8;
		resp128 [15] = 0;				// CRC-7 and stop bit
#elif CPUSTYLE_STM32F4XX
		// RESP1;Card Status [127:96]
		// RESP2;Card Status [95:64]
		// RESP3;Card Status [63:32]
		// RESP4;Card Status [31:1]0b
		
		resp128 [0] = SDIO->RESP1 >> 24;	// R127 to 120 (8 bit)
		resp128 [1] = SDIO->RESP1 >> 16;
		resp128 [2] = SDIO->RESP1 >> 8;
		resp128 [3] = SDIO->RESP1 >> 0;

		resp128 [4] = SDIO->RESP2 >> 24;
		resp128 [5] = SDIO->RESP2 >> 16;
		resp128 [6] = SDIO->RESP2 >> 8;
		resp128 [7] = SDIO->RESP2 >> 0;

		resp128 [8] = SDIO->RESP3 >> 24;
		resp128 [9] = SDIO->RESP3 >> 16;
		resp128 [10] = SDIO->RESP3 >> 8;
		resp128 [11] = SDIO->RESP3 >> 0;

		resp128 [12] = SDIO->RESP4 >> 24;
		resp128 [13] = SDIO->RESP4 >> 16;
		resp128 [14] = SDIO->RESP4 >> 8;
		resp128 [15] = 0;				// CRC-7 and stop bit

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

		resp128 [0] = SD0->RESP_3 >> 16;	// R127 to 120 (8 bit)
		resp128 [1] = SD0->RESP_3 >> 8;
		resp128 [2] = SD0->RESP_3 >> 0;
		resp128 [3] = SD0->RESP_2 >> 24;
		resp128 [4] = SD0->RESP_2 >> 16;
		resp128 [5] = SD0->RESP_2 >> 8;
		resp128 [6] = SD0->RESP_2 >> 0;
		resp128 [7] = SD0->RESP_1 >> 24;
		resp128 [8] = SD0->RESP_1 >> 16;
		resp128 [9] = SD0->RESP_1 >> 8;
		resp128 [10] = SD0->RESP_1 >> 0;
		resp128 [11] = SD0->RESP_0 >> 24;
		resp128 [12] = SD0->RESP_0 >> 16;
		resp128 [13] = SD0->RESP_0 >> 8;
		resp128 [14] = SD0->RESP_0 >> 0;
		resp128 [15] = 0;				// CRC-7 and stop bit

#else
	#error Wrong CPUSTYLE_xxx
#endif
}

// R1 (normal response command)
static uint_fast8_t sdhost_get_R1(uint_fast8_t cmd, uint_fast32_t * resp32)
{
	const uint_fast8_t ec = sdhost_get_resp();
	if (ec == 0)
	{
		if (sdhost_verify_resp(cmd) != 0)
			return 1;
		* resp32 = sdhost_get_resp32bit();
	}
	return ec;
}

// R7
static uint_fast8_t sdhost_get_R7(uint_fast8_t cmd, uint_fast32_t * resp32)
{
	const uint_fast8_t ec = sdhost_get_resp();
	if (ec == 0)
	{
		if (sdhost_verify_resp(cmd) != 0)
			return 1;
		* resp32 = sdhost_get_resp32bit();
	}
	return ec;
}


// R2 (CID, CSD register)
// Code length = 136 bits
static uint_fast8_t sdhost_get_R2(uint8_t * resp128)
{
	const uint_fast8_t ec = sdhost_get_resp();	/* не проверяется совпадение command index */
	if (ec == 0)
	{
		sdhost_get_resp128bit(resp128);
	}
	return ec;
}

// R3 (OCR register)
// All responses, except for the R3 response type, are protected by a CRC.
static uint_fast8_t sdhost_get_R3(uint_fast32_t * resp32)
{
	const uint_fast8_t ec = sdhost_get_resp_nocrc();
	if (ec == 0)
	{
		* resp32 = sdhost_get_resp32bit();
	}
	return ec;
}

/**
  * @brief  Enable the CMDTRANS mode.
  * @param  p : Pointer to SDMMC register base  
  * @retval None
  */  
//#define __SDMMC_CMDTRANS_ENABLE(p)  ((p)->CMD |= SDMMC_CMD_CMDTRANS)

/**
  * @brief  Disable the CMDTRANS mode.
  * @param  p : Pointer to SDMMC register base  
  * @retval None
  */  
//#define __SDMMC_CMDTRANS_DISABLE(p)  ((p)->CMD &= ~SDMMC_CMD_CMDTRANS)

/**
  * @brief  Enable the CMDSTOP mode.
  * @param  p : Pointer to SDMMC register base
  * @retval None
  */
//#define __SDMMC_CMDSTOP_ENABLE(p)  ((p)->CMD |= SDMMC_CMD_CMDSTOP)

/**
  * @brief  Disable the CMDSTOP mode.
  * @param  p : Pointer to SDMMC register base
  * @retval None
  */
//#define __SDMMC_CMDSTOP_DISABLE(p)  ((p)->CMD &= ~SDMMC_CMD_CMDSTOP)

static uint_fast8_t sdhost_stop_transmission(void)
{
	uint_fast32_t resp;
  //__SDMMC_CMDSTOP_ENABLE(SDMMC1);
  //__SDMMC_CMDTRANS_DISABLE(SDMMC1);
	sdhost_short_resp(encode_cmd(SD_CMD_STOP_TRANSMISSION, DEFAULT_TRANSFER_MODE), 0, 0);	// CMD12
	if (sdhost_get_R1(SD_CMD_STOP_TRANSMISSION, & resp) != 0)	// get R1b
	{
  //__SDMMC_CMDSTOP_DISABLE(SDMMC1);
		PRINTF(PSTR("sdhost_stop_transmission error\n"));
		return 1;
	}
  //__SDMMC_CMDSTOP_DISABLE(SDMMC1);
	return 0;
}


// Ожидаем завершения.
static uint_fast8_t sdhost_sdcard_waitstatus(void)
{
	uint_fast8_t cardstate = 0xFF;
	unsigned long n;
	for (n = 1000000; -- n;)
	{
		uint_fast32_t resp;

		// get status
		sdhost_short_resp(encode_cmd(SD_CMD_SEND_STATUS, DEFAULT_TRANSFER_MODE), sdhost_sdcard_RCA << 16, 0);	// CMD13
		if (sdhost_get_R1(SD_CMD_SEND_STATUS, & resp) != 0)
		{
			PRINTF(PSTR("sdhost_sdcard_waitstatus: SD_CMD_SEND_STATUS error\n"));
			return 1;
		}

		cardstate = (resp >> 9) & 0x0F;
		/*
			0 = Idle
			1 = Ready
			2 = Ident
			3 = Stby
			4 = Transfer
			5 = Data
			6 = Rcv
			7 = Programming
			8 = Disconnected
			9 = Btst
			10-15 = reserved
		*/
		switch (cardstate)
		{
		case 4:	// Transfer
		case 1:	// Ready
			//PRINTF(PSTR("sdhost_sdcard_waitstatus OK, resp=%08X, cardstate=%u\n"), (unsigned long) resp, (unsigned) cardstate);
			return 0;

		case 7:	// Programming
			continue;	// ожидаем окончания записи

		//case 6:	// Rcv
		//	continue;	// потребовалось при использовании блочной записи

		default:
			PRINTF(PSTR("sdhost_sdcard_waitstatus, resp=%08X, cardstate=%u\n"), (unsigned) resp, (unsigned) cardstate);
			return 1;
		}
	}
	PRINTF(PSTR("sdhost_sdcard_waitstatus, timeout error, cardstate=%u\n"), cardstate);
	return 1;
}

// Получить кратность аргумента для команды SD_CMD_READ_SINGLE_BLOCK/SD_CMD_WRITE_SINGLE_BLOCK
//  В зависимости от типа SD карты адрес это LBA или смещение в байтах

// 512 требуется для SDSC
//	(a) Argument 0001h is byte address 0001h in the SDSC and 0001h block in SDHC and SDXC
//	(b) Argument 0200h is byte address 0200h in the SDSC and 0200h block in SDHC and SDXC
// sdhost_CardType = SDIO_HIGH_CAPACITY_SD_CARD; // (CCS=1)

//	SDSC Card (CCS=0) uses byte unit address and
//	SDHC and SDXC Cards (CCS=1) use block unit address (512 bytes unit).

static uint_fast32_t sdhost_getaddresmultiplier(void)
{
// 	set at same time
//	sdhost_SDType = SD_HIGH_CAPACITY;
//	sdhost_CardType = SDIO_STD_CAPACITY_SD_CARD_V2_0; /*!< SD Card 2.0 */
	//if ((cmd58answer & 0x40000000) != 0)	//CCS (Card Capacity Status)
	//if ((sdhost_SDType & SD_HIGH_CAPACITY) != 0)	//CCS (Card Capacity Status)
	if (sdhost_CardType == SDIO_HIGH_CAPACITY_SD_CARD) // (CCS=1)
	{
		// Сюда должны попадать в случае SDHC and SDXC
		//PRINTF(PSTR("SDHC or SDXC (High Capacity)\n"));
		return 1; //mmcAddressMultiplier = 1;	// Для SDHC (адресация идёт в 512-ти байтовых блоках)
	}
	else
	{
		// Сюда должны попадать в случае SDSC
		//PRINTF(PSTR("SDSD - up to 2GB\n"));
		return 512; //mmcAddressMultiplier = MMC_SECTORSIZE;	// Для обычных SD карт
	}
}

	/*
	if (1)
	{
		// 128 MB карта
		// set block length
		if (sdhost_sdcard_setblocklen(sdhost_getblocksize) != 0)
			return 1;
	}
	else if (sdhost_CardType == SDIO_HIGH_CAPACITY_SD_CARD)
	{
		// set block length
		if (sdhost_sdcard_setblocklen(512) != 0)
			return 1;
	}
	else
	{
		// set block length
		if (sdhost_sdcard_setblocklen(1) != 0)
			return 1;
	}
	*/

// Получить значекние аргумента для команды SD_CMD_SET_BLOCKLEN
//  В зависимости от типа карты размер блока 1 или 512
/*
	In case of SDSC Card, block length is set by this command.
	In case of SDHC and SDXC Cards, block length of the memory access
	commands are fixed to 512 bytes. 
	The length of LOCK_UNLOCK command is set by this command
*/
static uint_fast32_t sdhost_getblocksize(void)
{
	return 512;
}


// Выдать префиксную команду APP_CMD и следующую за ней ACMDxxx
// Получить R1 responce
static uint_fast8_t sdhost_short_acmd_resp_R1(uint_fast8_t cmd, uint_fast32_t arg, uint_fast32_t * resp, uint_fast32_t transferMode)
{
	sdhost_short_resp(encode_cmd(SD_CMD_APP_CMD, DEFAULT_TRANSFER_MODE), sdhost_sdcard_RCA << 16, 0);
	if (sdhost_get_R1(SD_CMD_APP_CMD, resp) != 0)
	{
		TP();
		return 1;
	}
	sdhost_short_resp(encode_appcmd(cmd, transferMode), arg, 0);
	return sdhost_get_R1(cmd, resp);
}

// Выдать префиксную команду APP_CMD и следующую за ней ACMDxxx
// Получить R3 responce
// All responses, except for the R3 response type, are protected by a CRC.
static uint_fast8_t sdhost_short_acmd_resp_R3(uint_fast8_t cmd, uint_fast32_t arg, uint_fast32_t * resp, uint_fast32_t transferMode)
{
	sdhost_short_resp(encode_cmd(SD_CMD_APP_CMD, DEFAULT_TRANSFER_MODE), sdhost_sdcard_RCA << 16, 0);
	if (sdhost_get_R1(SD_CMD_APP_CMD, resp) != 0)
	{
		TP();
		return 1;
	}
	sdhost_short_resp(encode_appcmd(cmd, transferMode), arg, 1);	// no CRC check
	return sdhost_get_R3(resp);
}

///////////////////////////

#if 0

#define SD_CMD_NO_RESP (SDMMC_CMD_WAITRESP_0 * 0)
#define SD_CMD_SHORT_RESP (SDMMC_CMD_WAITRESP_0 * 1)
#define SD_CMD_LONG_RESP (SDMMC_CMD_WAITRESP_0 * 3)

#define SD_OK        0
#define SD_ERROR     1

static uint32_t SDSendCommand(uint32_t cmd, uint32_t arg, uint32_t rsp)
{
   PRINTF(PSTR("\r\nCMD%d ARG=%08x  "), cmd, arg);
   SDMMC1->ARG = arg;
   //Clear interrupt flags, unmask interrupts according to cmd response
   SDMMC1->ICR = (SDMMC_ICR_CMDRENDC | SDMMC_ICR_CMDSENTC | SDMMC_ICR_CCRCFAILC | SDMMC_ICR_CTIMEOUTC);
   if(rsp == SD_CMD_NO_RESP)
   {
      //No response wait for CMDSENT interrupt
      SDMMC1->MASK |= (SDMMC_MASK_CMDSENTIE | SDMMC_MASK_CCRCFAILIE | SDMMC_MASK_CTIMEOUTIE);
   }
   else 
   {
      //Short or long response wait for CMDREND interrupt
      SDMMC1->MASK |= (SDMMC_MASK_CMDRENDIE | SDMMC_MASK_CCRCFAILIE | SDMMC_MASK_CTIMEOUTIE);
   }
   SDMMC1->CMD = SDMMC_CMD_CPSMEN | rsp | cmd;
   if(WaitEvents(EV_SD_READY | EV_SD_ERROR, WAIT_ANY) != EV_SD_READY)
   { 
      PRINTF(PSTR("E1(cmd=%d arg=%x STA=%x)  "), cmd,arg,SDMMC1->STA);
      return SD_ERROR; 
   }
   //ASSERT(WaitEvents(EV_SD_READY | EV_SD_ERROR, WAIT_ANY) == EV_SD_READY);
   //Check for the correct response (long response and OCR response will result in 0x3F value in RESPCMD)
   if(rsp == SD_CMD_SHORT_RESP && SDMMC1->RESPCMD != (cmd & SDMMC_CMD_CMDINDEX))
   {
      PRINTF(PSTR("E2(cmd=%d arg=%x STA=%x)  "), cmd,arg,SDMMC1->STA);
      return SD_ERROR;
   }
   //ASSERT(!(rsp == SD_CMD_SHORT_RESP && SDMMC1->RESPCMD != cmd));
   PRINTF(PSTR("OK"));
   return SD_OK;
}

static uint32_t SDWriteBlock(uint32_t address, const void* buffer, uint32_t size)
{

	//if(SDCardState == 0)return RES_NOTRDY;
   //Wait until card is ready for the data operations
   for(;;)
   {
      //Make 10 tries to get card status before fail
      for(int t = 0; SDSendCommand(13, sdhost_sdcard_RCA << 16, SD_CMD_SHORT_RESP); t++)
      {
         if(t > 10)return RES_NOTRDY;
      }
      if((SDMMC1->RESP1 & (SD_STAT_CURRENT_STATE|SD_STAT_READY_FOR_DATA)) == (SD_STAT_CURRENT_STATE_TRAN|SD_STAT_READY_FOR_DATA))break;
      local_delay_ms((1));
   }

   PRINTF(PSTR(" WR1:%x "), SDMMC1->RESP1);
 	dcache_clean_invalidate((uintptr_t) buffer, 512 * size);	// Сейчас эту память будем записывать по DMA, потом содержимое не требуется
  //SCB_CleanDCache_by_Addr(buffer, size*512);

   //Program data length register
   SDMMC1->DLEN = size * 512;
   
   //Clean DPSM interrupt flags and enable DPSM interrupts
   SDMMC1->ICR = (SDMMC_ICR_DBCKENDC | SDMMC_ICR_DATAENDC | SDMMC_ICR_DCRCFAILC | SDMMC_ICR_DTIMEOUTC);
   SDMMC1->MASK |= (SDMMC_MASK_DATAENDIE | SDMMC_MASK_DCRCFAILIE | SDMMC_MASK_DTIMEOUTIE);

   SDMMC1->DTIMER = 12000000 /*0.5sec timeout*/;

   //Program data control register
   SDMMC1->DCTRL = SDMMC_DCTRL_DBLOCKSIZE_0 * 9 /*512 bytes block*/ |
                     SDMMC_DCTRL_DTDIR * 0 /* From controller to card*/; /*|
                     SDMMC_DCTRL_DTEN;*/

   //Program SDMMC1 DMA
   SDMMC1->IDMABASE0 = (uintptr_t) buffer;
   SDMMC1->IDMACTRL |= SDMMC_IDMA_IDMAEN;

   
   uint8_t cmd = (size == 1) ? 24 /*CMD24 single block write*/: 25 /*CMD25 multiple blocks write*/;
   if(SDSendCommand(cmd | SDMMC_CMD_CMDTRANS, address, SD_CMD_SHORT_RESP))
   {
      PRINTF(PSTR("CMD%d failed!"), cmd);
      //turn off DMA
      return RES_ERROR;
   }


   //Wait for data transfer finish
   WaitEvents(EV_SD_DATA, WAIT_ANY);
   SDMMC1->IDMACTRL &= ~SDMMC_IDMA_IDMAEN;   
   if(size > 1)
   {
      //Send CMD12 STOP command in case of multiple block transfer      
      if(SDSendCommand(12, 0, SD_CMD_SHORT_RESP))return RES_ERROR;
   }
   //Do we need to check FIFOCNT and wait until it is 0?
   //PRINTF(PSTR(" FIFO:%d WSTA:%x "), SDMMC1->FIFOCNT, SDMMC1->STA);
   
   if((SDMMC1->STA & (SDMMC_STA_DATAEND | SDMMC_STA_DCRCFAIL | SDMMC_STA_DTIMEOUT)) == SDMMC_STA_DATAEND)
	   return RES_OK;
   return RES_ERROR;
}

#endif

static int multisectorWriteProblems(UINT count)
{
#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1 || CPUSTYLE_XC7Z || CPUSTYLE_XCZU
	if (count > 1)
	{
		return 1;
	}
#endif /* CPUSTYLE_XXX */
	return 0;
}

static int multisectorReadProblems(UINT count)
{
#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
	if (count > 1)
	{
		return 1;
	}
#endif /* CPUSTYLE_XXX */
	return 0;
}

// write a size Byte big block beginning at the address.
static 
DRESULT SD_disk_write(
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write */
	)
{
	sector += USERFIRSTSBLOCK;

	enum { txmode = 1 };
	//return SDWriteBlock(sector, buff, count);
	//if ((uintptr_t) buff & 0x1F)
	//{
	//	PRINTF(PSTR("SD_disk_write: unalligned: buff=%p, sector=%lu, count=%lu\n"), buff, (unsigned long) sector, (unsigned long) count);
	//}
	//PRINTF(PSTR("SD_disk_write: buff=%p, sector=%lu, count=%lu\n"), buff, (unsigned long) sector, (unsigned long) count);
	uint_fast32_t resp;
	
	if (sdhost_sdcard_waitstatus() != 0)
	{
		PRINTF(PSTR("SD_disk_write: sdhost_sdcard_waitstatus error\n"));
		return RES_ERROR;
	}

	if ((sdhost_SDType & SD_HIGH_CAPACITY) == 0)	//CCS (Card Capacity Status)
	{
		// set block length
		sdhost_short_resp(encode_cmd(SD_CMD_SET_BLOCKLEN, DEFAULT_TRANSFER_MODE), sdhost_getblocksize(), 0);	// CMD16
		if (sdhost_get_R1(SD_CMD_SET_BLOCKLEN, & resp) != 0)
		{
			PRINTF(PSTR("sdhost_sdcard_setblocklen error\n"));
			return RES_ERROR;
		}
	}

	dcache_clean_invalidate((uintptr_t) buff, 512 * count);	// Сейчас эту память будем записывать по DMA, потом содержимое не требуется

	if (count < 2)
	{
		// wriite single block
		//PRINTF(PSTR("write single block\n"));
		// Сперва настраивается DMA, затем выдается команда SD_CMD_WRITE_SINGLE_BLOCK
		// Работает и на STM32Fxxx

		DMA_SDIO_setparams((uintptr_t) buff, 512, count, txmode);
#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
		// H7 need here: 
		sdhost_dpsm_prepare((uintptr_t) buff, txmode, 512 * count, 9);		// подготовка к обмену data path state machine - при записи после выдачи команды
#endif /* CPUSTYLE_STM32H7XX */
		// write block
		sdhost_short_resp(encode_cmd(SD_CMD_WRITE_SINGLE_BLOCK, getTransferMode(txmode, count)), sector * sdhost_getaddresmultiplier(), 0);	// CMD24
		if (sdhost_get_R1(SD_CMD_WRITE_SINGLE_BLOCK, & resp) != 0)
		{
			DMA_sdio_cancel();
			PRINTF(PSTR("SD_CMD_WRITE_SINGLE_BLOCK error\n"));
			return RES_ERROR;
		}

#if ! (CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1)
		// other then H7 need here
		sdhost_dpsm_prepare((uintptr_t) buff, txmode, 512 * count, 9);		// подготовка к обмену data path state machine - при записи после выдачи команды
#endif /* ! CPUSTYLE_STM32H7XX */

		if (sdhost_dpsm_wait((uintptr_t) buff, txmode, 512 * count) != 0)
		{
			DMA_sdio_cancel();
			PRINTF(PSTR("SD_disk_write 1: sdhost_dpsm_wait error\n"));
			return RES_ERROR;
		}
		else if (DMA_sdio_waitdone(txmode) != 0)
		{
			DMA_sdio_cancel();
			PRINTF(PSTR("SD_disk_write 1: DMA_sdio_waitdone error\n"));
			return RES_ERROR;
		}
		else
		{
			sdhost_dpsm_wait_fifo_empty();
			return RES_OK;
		}
	}
	else
	{
		// write multiblock
		//PRINTF(PSTR("write multiblock, count=%d\n"), count);
		//PRINTF(PSTR("SD_CMD_SD_APP_SET_NWB_PREERASED okay\n"));

		if (sdhost_hardware_cmd12() == 0 && sdhost_use_cmd23 != 0)	// set block count
		{
			// set block count
			sdhost_short_resp(encode_cmd(SD_CMD_SET_BLOCK_COUNT, DEFAULT_TRANSFER_MODE), count, 0);	// CMD23
			if (sdhost_get_R1(SD_CMD_SET_BLOCK_COUNT, & resp) != 0)	// get R1
			{
				PRINTF(PSTR("SD_CMD_SET_BLOCK_COUNT 1 error, count=%u\n"), (unsigned) count);
				//return RES_ERROR;
				// set block count
				sdhost_short_resp(encode_cmd(SD_CMD_SET_BLOCK_COUNT, DEFAULT_TRANSFER_MODE), count, 0);	// CMD23
				if (sdhost_get_R1(SD_CMD_SET_BLOCK_COUNT, & resp) != 0)	// get R1
				{
					PRINTF(PSTR("SD_CMD_SET_BLOCK_COUNT 2 error, count=%u\n"), (unsigned) count);
					DMA_sdio_cancel();
					return RES_ERROR;
				}
			}
		}

		// Pre-erased Setting prior to a Multiple Block Write Operation
		// Setting a number of write blocks to be pre-erased (ACMD23)
		if (sdhost_short_acmd_resp_R1(SD_CMD_SD_APP_SET_NWB_PREERASED, count & 0x007FFFFFuL, & resp, DEFAULT_TRANSFER_MODE) != 0) // ACMD23
		{
			PRINTF(PSTR("SD_CMD_SD_APP_SET_NWB_PREERASED error, count=%u\n"), (unsigned) count);
			return RES_ERROR;
		}

		// write multiblock
		// Сперва настраивается DMA, затем выдается команда SD_CMD_WRITE_MULT_BLOCK
		// Работает и на STM32Fxxx
		DMA_SDIO_setparams((uintptr_t) buff, 512, count, txmode);
#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
		// H7 need here:
		sdhost_dpsm_prepare((uintptr_t) buff, txmode, 512 * count, 9);		// подготовка к обмену data path state machine - при записи после выдачи команды
#endif /* CPUSTYLE_STM32H7XX */

		// write blocks
		sdhost_short_resp(encode_cmd(SD_CMD_WRITE_MULT_BLOCK, getTransferMode(txmode, count)), sector * sdhost_getaddresmultiplier(), 0);	// CMD25
		if (sdhost_get_R1(SD_CMD_WRITE_MULT_BLOCK, & resp) != 0)
		{
			DMA_sdio_cancel();
			PRINTF(PSTR("SD_CMD_WRITE_MULT_BLOCK error, count=%u\n"), (unsigned) count);
			return RES_ERROR;
		}

#if ! (CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1)
		// other then H7 need here
		sdhost_dpsm_prepare((uintptr_t) buff, txmode, 512 * count, 9);		// подготовка к обмену data path state machine - при записи после выдачи команды
#endif /* ! CPUSTYLE_STM32H7XX */

		if (sdhost_dpsm_wait((uintptr_t) buff, txmode, 512 * count) != 0)
		{
			PRINTF(PSTR("SD_disk_write: sdhost_dpsm_wait error, count=%u\n"), (unsigned) count);
			DMA_sdio_cancel();
			if (sdhost_stop_transmission() != 0)
				PRINTF(PSTR("SD_disk_write 2: sdhost_stop_transmission error, count=%u\n"), (unsigned) count);
			return RES_ERROR;
		}
		else if (DMA_sdio_waitdone(txmode) != 0)
		{
			DMA_sdio_cancel();
			PRINTF(PSTR("SD_disk_write 2: DMA_sdio_waitdone error, count=%u\n"), (unsigned) count);
			return RES_ERROR;
		}
		else
		{
			sdhost_dpsm_wait_fifo_empty();
			DMA_sdio_cancel();
			if (sdhost_hardware_cmd12() == 0 && sdhost_use_cmd23 == 0)
			{
				if (sdhost_stop_transmission() != 0)
				{
					PRINTF(PSTR("SD_disk_write 3: sdhost_stop_transmission error, count=%u\n"), (unsigned) count);
					return RES_ERROR;
				}
			}
		}
		//PRINTF(PSTR("write multiblock, count=%d okay\n"), count);
		return RES_OK;
	}
	//PRINTF(PSTR("SD_disk_write: buff=%p, sector=%lu, count=%lu okay\n"), buff, (unsigned long) sector, (unsigned long) count);
}


// read a size Byte big block beginning at the address.
static 
DRESULT SD_disk_read(
	BYTE drv,			/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read */
	)
{
	sector += USERFIRSTSBLOCK;
	enum { txmode = 0 };
	//if ((uintptr_t) buff & 0x1F)
	//{
	//	PRINTF(PSTR("SD_disk_read: unalligned: buff=%p, sector=%lu, count=%lu\n"), buff, (unsigned long) sector, (unsigned long) count);
	//}
	//PRINTF(PSTR("SD_disk_read: buff=%p, sector=%lu, count=%lu\n"), buff, (unsigned long) sector, (unsigned long) count);
	uint_fast32_t resp;
	
	if (sdhost_sdcard_waitstatus() != 0)
	{
		PRINTF(PSTR("SD_disk_read: sdhost_sdcard_waitstatus error\n"));
		return RES_ERROR;
	}
	
	
	//PRINTF(PSTR("SD_disk_read: sdhost_CardType=%08X, sdhost_SDType=%08X\n"), (unsigned long) sdhost_CardType, (unsigned long) sdhost_SDType);

	if ((sdhost_SDType & SD_HIGH_CAPACITY) == 0)	//CCS (Card Capacity Status)
	{
		// set block length
		sdhost_short_resp(encode_cmd(SD_CMD_SET_BLOCKLEN, DEFAULT_TRANSFER_MODE), sdhost_getblocksize(), 0);	// CMD16
		if (sdhost_get_R1(SD_CMD_SET_BLOCKLEN, & resp) != 0)
		{
			PRINTF(PSTR("sdhost_sdcard_setblocklen error\n"));
			return RES_ERROR;
		}
	}


	dcache_clean_invalidate((uintptr_t) buff, 512 * count);	// Сейчас эту память будем записывать по DMA, потом содержимое не требуется

	if (count < 2)
	{
		//PRINTF(PSTR("read one block\n"));
		// read one block
		DMA_SDIO_setparams((uintptr_t) buff, 512, count, txmode);
		sdhost_dpsm_prepare((uintptr_t) buff, txmode, 512 * count, 9);		// подготовка к обмену data path state machine - при чтении перед выдачей команды
		//TP();

		// read block
		sdhost_short_resp(encode_cmd(SD_CMD_READ_SINGLE_BLOCK, getTransferMode(txmode, count)), sector * sdhost_getaddresmultiplier(), 0);	// CMD17
		if (sdhost_get_R1(SD_CMD_READ_SINGLE_BLOCK, & resp) != 0)
		{
			DMA_sdio_cancel();
			PRINTF(PSTR("SD_CMD_READ_SINGLE_BLOCK error\n"));
			return RES_ERROR;
		}
		if (sdhost_dpsm_wait((uintptr_t) buff, txmode, 512 * count) != 0)
		{
			DMA_sdio_cancel();
			PRINTF(PSTR("SD_disk_read 1: sdhost_dpsm_wait error\n"));
			return RES_ERROR;
		}
		else if (DMA_sdio_waitdone(txmode) != 0)
		{
			DMA_sdio_cancel();
			PRINTF(PSTR("SD_disk_read 1: DMA_sdio_waitdone error\n"));
			return RES_ERROR;
		}
		else
		{
			sdhost_dpsm_wait_fifo_empty();
			return RES_OK;
		}
	}
	else
	{
		//PRINTF(PSTR("read multiple blocks: count=%d\n"), count);
		// read multiple blocks
		if (sdhost_hardware_cmd12() == 0 && sdhost_use_cmd23 != 0)	// set block count
		{
			// set block count
			sdhost_short_resp(encode_cmd(SD_CMD_SET_BLOCK_COUNT, DEFAULT_TRANSFER_MODE), count, 0);	// CMD23
			if (sdhost_get_R1(SD_CMD_SET_BLOCK_COUNT, & resp) != 0)	// get R1
			{
				PRINTF(PSTR("SD_CMD_SET_BLOCK_COUNT 3 error\n"));
				//return RES_ERROR;
				// set block count
				sdhost_short_resp(encode_cmd(SD_CMD_SET_BLOCK_COUNT, DEFAULT_TRANSFER_MODE), count, 0);	// CMD23
				if (sdhost_get_R1(SD_CMD_SET_BLOCK_COUNT, & resp) != 0)	// get R1
				{
					PRINTF(PSTR("SD_CMD_SET_BLOCK_COUNT 4 error\n"));
					DMA_sdio_cancel();
					return RES_ERROR;
				}
			}
		}

		DMA_SDIO_setparams((uintptr_t) buff, 512, count, txmode);
		sdhost_dpsm_prepare((uintptr_t) buff, txmode, 512 * count, 9);		// подготовка к обмену data path state machine - при чтении перед выдачей команды

		// read block
		sdhost_short_resp(encode_cmd(SD_CMD_READ_MULT_BLOCK, getTransferMode(txmode, count)), sector * sdhost_getaddresmultiplier(), 0);	// CMD18
		if (sdhost_get_R1(SD_CMD_READ_MULT_BLOCK, & resp) != 0)
		{
			DMA_sdio_cancel();
			PRINTF(PSTR("SD_CMD_READ_MULT_BLOCK error, count=%u\n"), (unsigned) count);
			return RES_ERROR;
		}
		if (sdhost_dpsm_wait((uintptr_t) buff, txmode, 512 * count) != 0)
		{
			PRINTF(PSTR("SD_disk_read 2: sdhost_dpsm_wait error, count=%u\n"), (unsigned) count);
			DMA_sdio_cancel();
			if (sdhost_stop_transmission() != 0)
				PRINTF(PSTR("SD_disk_read 2: sdhost_stop_transmission error, count=%u\n"), (unsigned) count);
			return RES_ERROR;
		}
		else if (DMA_sdio_waitdone(txmode) != 0)
		{
			DMA_sdio_cancel();
			PRINTF(PSTR("SD_disk_read 2: DMA_sdio_waitdone error, count=%u\n"), (unsigned) count);
			return RES_ERROR;
		}
		else
		{
			sdhost_dpsm_wait_fifo_empty();
			DMA_sdio_cancel();
			if (sdhost_hardware_cmd12() == 0 && sdhost_use_cmd23 == 0)
			{
				if (sdhost_stop_transmission() != 0)
				{
					PRINTF(PSTR("SD_disk_read 3: sdhost_stop_transmission error, count=%u\n"), (unsigned) count);
					return RES_ERROR;
				}
			}
			return RES_OK;
		}
	}
}

static uint_fast8_t sdhost_sdcard_checkversion(void)
{
	uint_fast32_t resp;

	const unsigned COUNTLIMIT = 16;
	unsigned count;

	for (count = 0; count < COUNTLIMIT; ++ count)
	{
		sdhost_no_resp(encode_cmd(SD_CMD_GO_IDLE_STATE, DEFAULT_TRANSFER_MODE), 0x00000000uL);	// CMD0
		sdhost_get_none_resp();

		sdhost_short_resp(encode_cmd(SDIO_SEND_IF_COND, DEFAULT_TRANSFER_MODE), SD_CHECK_PATTERN, 0);	// CMD8 -> R7 (Card interface condition)
		if (sdhost_get_R7(SDIO_SEND_IF_COND, & resp) == 0)	// R7: (Card interface condition)
		{
			sdhost_SDType = SD_HIGH_CAPACITY;
			sdhost_CardType = SDIO_STD_CAPACITY_SD_CARD_V2_0; /*!< SD Card 2.0 */
			PRINTF(PSTR("SD CARD is V2, R1 resp: stuff=%08X\n"), resp);
			return 0;
		}
	}
	/* не получается... */

	sdhost_SDType = SD_STD_CAPACITY;
	sdhost_CardType = SDIO_STD_CAPACITY_SD_CARD_V1_1; /*!< SD Card 1.0 */

	sdhost_short_resp(encode_cmd(SD_CMD_APP_CMD, DEFAULT_TRANSFER_MODE), sdhost_sdcard_RCA << 16, 0);	// CMD55 APP_CMD
	if (sdhost_get_R1(SD_CMD_APP_CMD, & resp) != 0)
	{
		
		PRINTF(PSTR("sdhost_sdcard_checkversion failure\n"));
		return 1;
	}
	PRINTF(PSTR("SD CARD is V1, R1 resp: stuff=%08X\n"), resp);
	return 0;
}

static uint_fast8_t sdhost_sdcard_poweron(void)
{
	uint_fast32_t respOCR;

	//PRINTF(PSTR("SD CARD power on start\n"));
	sdhost_sdcard_RCA = 0;

	if (sdhost_sdcard_checkversion() != 0)
	{
		PRINTF("sdhost_sdcard_checkversion failed\n");
		return 1;
	}

	//PRINTF(PSTR("Set voltage conditions\n"));

	const unsigned COUNTLIMIT = 10000;
	unsigned count;
	for (count = 0; count < COUNTLIMIT; ++ count)
	{
#if WITHSDHCHW
		if (sdhost_short_acmd_resp_R3(SD_CMD_SD_APP_OP_COND, SD_VOLTAGE_WINDOW_SD | sdhost_SDType, & respOCR, DEFAULT_TRANSFER_MODE) != 0)	// ACMD41
		{
			PRINTF(PSTR("voltage send process: sdhost_short_acmd_resp_R3(SD_CMD_SD_APP_OP_COND) failure\n"));
			return 1;
		}
#else /* WITHSDHCHW */
		sdhost_short_resp(encode_cmd(58, DEFAULT_TRANSFER_MODE), 0, 0);	// CMD58
		if (sdhost_get_R3(& respOCR) != 0)	// Response of ACMD41 (R3)
		{
			PRINTF(PSTR("Set voltage conditions error\n"));
			return 1;
		}
#endif /* WITHSDHCHW */
 		PRINTF(PSTR("voltage send waiting: R3 resp: respOCR=%08X\n"), respOCR);
		if ((respOCR & (1UL << 31)) == 0)	// check for voltage range is okay
			continue;
		PRINTF(PSTR("voltage send okay: R3 resp: respOCR=%08X\n"), respOCR);
		if ((respOCR & SD_HIGH_CAPACITY) != 0)
		{
			// set by Card Capacity Status (CCS)
            sdhost_CardType = SDIO_HIGH_CAPACITY_SD_CARD; // (CCS=1)
			PRINTF(PSTR("SD CARD is high capacity (CCS=1)\n"));
		}
		PRINTF(PSTR("SD CARD power on done, no errors\n"));
		return 0;
	}
	PRINTF(PSTR("SD CARD power on done, error\n"));
	return 1;
}
// sdhost_CardType = SDIO_HIGH_CAPACITY_SD_CARD; // (CCS=1)
static uint_fast8_t sdhost_read_registers_acmd(uint16_t acmd, uint8_t * buff, unsigned size, unsigned lenpower, unsigned sizeofarray)
{
	enum { txmode = 0 };	// read from card to memory
	uint_fast32_t resp;
	
	ASSERT(size == (1U << lenpower));
	if (sdhost_sdcard_waitstatus() != 0)
	{
		PRINTF(PSTR("sdhost_read_registers_acmd: sdhost_sdcard_waitstatus error\n"));
		return 1;
	}
	
	//PRINTF(PSTR("sdhost_read_registers_acmd: sdhost_CardType=%08X, sdhost_SDType=%08X\n"), (unsigned long) sdhost_CardType, (unsigned long) sdhost_SDType);

	sdhost_dpsm_prepare((uintptr_t) buff, txmode, size, lenpower);		// подготовка к обмену data path state machine - при чтенииперед выдачей команды
	dcache_clean_invalidate((uintptr_t) buff, sizeofarray);	// Сейчас эту память будем записывать по DMA, потом содержимое не требуется
	DMA_SDIO_setparams((uintptr_t) buff, size, 1, txmode);

	// read block
	if (sdhost_short_acmd_resp_R1(acmd, 0, & resp, getTransferMode(txmode, 1)) != 0)	// ACMD51
	{
		DMA_sdio_cancel();
		PRINTF(PSTR("sdhost_read_registers_acmd: sdhost_get_R1 (acmd=0x%02X) error\n"), acmd);
		return 1;
	}
	if (sdhost_dpsm_wait((uintptr_t) buff, txmode, size) != 0)
	{
		DMA_sdio_cancel();
		PRINTF(PSTR("sdhost_read_registers_acmd: sdhost_dpsm_wait error\n"));
		return 1;
	}
	else if (DMA_sdio_waitdone(txmode) != 0)
	{
		DMA_sdio_cancel();
		PRINTF(PSTR("sdhost_read_registers_acmd: DMA_sdio_waitdone error\n"));
		return 1;
	}
	else
	{
		sdhost_dpsm_wait_fifo_empty();
		return 0;
	}
}

static uint_fast8_t sdhost_sdcard_identification(void)
{
	uint_fast32_t resp;

	//PRINTF(PSTR("SD CARD identification start\n"));

	sdhost_long_resp(encode_cmd(SD_CMD_ALL_SEND_CID, DEFAULT_TRANSFER_MODE), 0);	// CMD2
	if (sdhost_get_R2(sdhost_sdcard_CID) != 0)
	{
		PRINTF(PSTR("sdhost_sdcard_identification: SD_CMD_ALL_SEND_CID error\n"));
		return 1;
	}

#if WITHSDHCHW

	sdhost_short_resp(encode_cmd(SD_CMD_SET_REL_ADDR, DEFAULT_TRANSFER_MODE), 0, 0);	// CMD3
	if (sdhost_get_R1(SD_CMD_SET_REL_ADDR, & resp) != 0)
	{
		PRINTF(PSTR("sdhost_sdcard_identification: SD_CMD_SET_REL_ADDR error\n"));
		return 1;
	}
	else
	{
		sdhost_sdcard_RCA = 0xFFFF & (resp >> 16);
		//PRINTF(PSTR("RCA=%08X\n"), sdhost_sdcard_RCA);
	}
#endif /* WITHSDHCHW */

	// Get CSD
	sdhost_long_resp(encode_cmd(SD_CMD_SEND_CSD, DEFAULT_TRANSFER_MODE), sdhost_sdcard_RCA << 16);	// CMD9 The contents of the CSD register are sent as a response to CMD9.
	if (sdhost_get_R2(sdhost_sdcard_CSD) != 0)
	{
		PRINTF(PSTR("sdhost_sdcard_identification: SD_CMD_SEND_CSD error\n"));
		return 1;
	}
	//PRINTF(PSTR("SD_CMD_SEND_CSD okay\n"));
	//printhex(0, sdhost_sdcard_CSD, sizeof sdhost_sdcard_CSD);

#if WITHSDHCHW
	// Select card
	sdhost_short_resp(encode_cmd(SD_CMD_SEL_DESEL_CARD, DEFAULT_TRANSFER_MODE), sdhost_sdcard_RCA << 16, 0);
	if (sdhost_get_R1(SD_CMD_SEL_DESEL_CARD, & resp) != 0)
	{
		PRINTF(PSTR("sdhost_sdcard_identification: SD_CMD_SEL_DESEL_CARD error\n"));
		return 1;
	}
	//PRINTF(PSTR("SD_CMD_SEL_DESEL_CARD okay\n"));
#endif /* WITHSDHCHW */

	// debug
	if (sdhost_sdcard_waitstatus() != 0)
	{
		PRINTF(PSTR("sdhost_sdcard_identification: sdhost_sdcard_waitstatus error\n"));
		return 1;
	}

	// Get SCR
#if WITHSDHCHW4BIT
	uint_fast8_t bussupport4b = 1;
	uint_fast8_t bussupport1b = 1;
#else /* WITHSDHCHW4BIT */
	uint_fast8_t bussupport4b = 0;
	uint_fast8_t bussupport1b = 1;
#endif /* WITHSDHCHW4BIT */
	sdhost_use_cmd23 = 0;	// set block count command uasage
	sdhost_use_cmd20 = 0;
//#if 1 && WITHSDHCHW
	static ALIGNX_BEGIN uint8_t sdhost_sdcard_SCR [32] ALIGNX_END;	// надо только 8 байт, но какая-то проюлема с кэш - работает при 32 и более

	if (sdhost_read_registers_acmd(SD_CMD_SD_APP_SEND_SCR, sdhost_sdcard_SCR, 8, 3, sizeof sdhost_sdcard_SCR) == 0)		// ACMD51
	{
		PRINTF("SD_CMD_SD_APP_SEND_SCR okay: SCR=%02X%02X%02X%02X%02X%02X%02X%02X\n",
			sdhost_sdcard_SCR [0], sdhost_sdcard_SCR [1], sdhost_sdcard_SCR [2], sdhost_sdcard_SCR [3], sdhost_sdcard_SCR [4], sdhost_sdcard_SCR [5], sdhost_sdcard_SCR [6], sdhost_sdcard_SCR [7]
			);

		// sdhost_sdcard_SCR [0]: 63..56
		// sdhost_sdcard_SCR [1]: 55..48
		// sdhost_sdcard_SCR [2]: 47..40
		// sdhost_sdcard_SCR [3]: 39..32
		// sdhost_sdcard_SCR [4]: 31..24
		// sdhost_sdcard_SCR [5]: 23..16
		// sdhost_sdcard_SCR [6]: 15..8
		// sdhost_sdcard_SCR [7]: 7..0
		PRINTF(PSTR("SCR Structure=0x%02X\n"), array_get_bits(sdhost_sdcard_SCR, 64, 63, 4));			// [63:60]
		PRINTF(PSTR("SD Memory Card - Spec. Version=0x%02X\n"), array_get_bits(sdhost_sdcard_SCR, 64, 63, 4));	// [59:56]
		PRINTF(PSTR("SD Memory Card - Spec3. Version=0x%02X\n"), array_get_bits(sdhost_sdcard_SCR, 64, 47, 1));	// [47]
		PRINTF(PSTR("SD Memory Card - ext security=0x%02X\n"), array_get_bits(sdhost_sdcard_SCR, 64, 46, 4));	// [46:43]
		PRINTF(PSTR("DAT Bus widths supported=0x%02X\n"), array_get_bits(sdhost_sdcard_SCR, 64, 51, 4));	// [51:48]
		PRINTF(PSTR("CPRM Security Support=0x%02X\n"), array_get_bits(sdhost_sdcard_SCR, 64, 54, 3));			// [54:52]
		PRINTF(PSTR("CMD Support=0x%02X\n"), array_get_bits(sdhost_sdcard_SCR, 64, 33, 2));			// [33:32]

		bussupport1b = array_get_bits(sdhost_sdcard_SCR, 64, 48, 1); //(sdhost_sdcard_SCR [1] & 0x01) != 0;
		bussupport4b = array_get_bits(sdhost_sdcard_SCR, 64, 50, 1); //(sdhost_sdcard_SCR [1] & 0x04) != 0;
		sdhost_use_cmd20 = array_get_bits(sdhost_sdcard_SCR, 64, 32, 1); //(sdhost_sdcard_SCR [3] & 0x01) != 0;
		sdhost_use_cmd23 = array_get_bits(sdhost_sdcard_SCR, 64, 33, 1); //(sdhost_sdcard_SCR [3] & 0x02) != 0;	// set block count

		PRINTF("Support: 1b=%d, 4b=%d, cmd20=%d, cmd23=%d\n", bussupport1b, bussupport4b, sdhost_use_cmd20, sdhost_use_cmd23);
	}
//#endif /* WITHSDHCHW */

#if WITHSDHCHW

#if WITHSDHCHW4BIT
	if (bussupport4b != 0)
	{
		// Set 4 bit bus width
		if (sdhost_short_acmd_resp_R1(SD_CMD_APP_SD_SET_BUSWIDTH, 0x0002, & resp, DEFAULT_TRANSFER_MODE) != 0) // ACMD6
		{
			PRINTF(PSTR("SD_CMD_APP_SD_SET_BUSWIDTH error\n"));
			return 1;
		}
		PRINTF(PSTR("sdhost_sdcard_identification: SD_CMD_APP_SD_SET_BUSWIDTH 4 okay\n"));
		hardware_sdhost_setbuswidth(1);		// 4-bit width
	}
	else if (bussupport1b != 0)
	{
		// Set 1 bit bus width
		if (sdhost_short_acmd_resp_R1(SD_CMD_APP_SD_SET_BUSWIDTH, 0x0000, & resp, DEFAULT_TRANSFER_MODE) != 0) // ACMD6
		{
			PRINTF(PSTR("sdhost_sdcard_identification: SD_CMD_APP_SD_SET_BUSWIDTH error\n"));
			return 1;
		}
		PRINTF(PSTR("sdhost_sdcard_identification: SD_CMD_APP_SD_SET_BUSWIDTH 1 okay\n"));
		hardware_sdhost_setbuswidth(0);		// 1-bit width
	}
	else
	{
		PRINTF("sdhost_sdcard_identification: No 4 or 1 bit supported... Error.\n");
		return 1;
	}

#else /* WITHSDHCHW4BIT */

	if (bussupport1b != 0)
	{
		// Set 1 bit bus width
		if (sdhost_short_acmd_resp_R1(SD_CMD_APP_SD_SET_BUSWIDTH, 0x0000, & resp, DEFAULT_TRANSFER_MODE) != 0) // ACMD6
		{
			PRINTF(PSTR("sdhost_sdcard_identification: SD_CMD_APP_SD_SET_BUSWIDTH error\n"));
			return 1;
		}
		PRINTF(PSTR("sdhost_sdcard_identification: SD_CMD_APP_SD_SET_BUSWIDTH 1 okay\n"));
		hardware_sdhost_setbuswidth(0);		// 1-bit width
	}
	else
	{
		PRINTF("sdhost_sdcard_identification: 1 bit not supported... Error.\n");
		return 1;
	}

#endif /* WITHSDHCHW4BIT */
#endif /* WITHSDHCHW */

	// Get SD status
#if 1 && WITHSDHCHW
	// STM32MP1 work
	static ALIGNX_BEGIN uint8_t sdhost_sdcard_SDSTATUS [64] ALIGNX_END;
	if (sdhost_read_registers_acmd(SD_CMD_SD_APP_STATUS, sdhost_sdcard_SDSTATUS, 64, 6, sizeof sdhost_sdcard_SDSTATUS) == 0)		// ACMD13
	{

		PRINTF(PSTR("SECURED_MODE=%02x\n"), array_get_bits(sdhost_sdcard_SDSTATUS, 512, 509, 1));
		PRINTF(PSTR("DAT_BUS_WIDTH=%02x\n"), array_get_bits(sdhost_sdcard_SDSTATUS, 512, 511, 2));
		PRINTF(PSTR("SD_CARD_TYPE=%04x\n"), array_get_bits(sdhost_sdcard_SDSTATUS, 512, 495, 16));

		PRINTF(PSTR("SIZE_OF_PROTECTED_AREA=%08x\n"), array_get_bits(sdhost_sdcard_SDSTATUS, 512, 479, 32));

		PRINTF(PSTR("SPEED_CLASS=%02x\n"), array_get_bits(sdhost_sdcard_SDSTATUS, 512, 447, 8));
		PRINTF(PSTR("PERFORMANCE_MOVE=%02x\n"), array_get_bits(sdhost_sdcard_SDSTATUS, 512, 439, 8));
		PRINTF(PSTR("AU_SIZE=%02x\n"), array_get_bits(sdhost_sdcard_SDSTATUS, 512, 431, 4));
		PRINTF(PSTR("ERASE_SIZE=%04x\n"), array_get_bits(sdhost_sdcard_SDSTATUS, 512, 423, 16));

	}
#endif /* WITHSDHCHW */

	// debug
	if (sdhost_sdcard_waitstatus() != 0)
	{
		PRINTF(PSTR("sdhost_sdcard_identification: sdhost_sdcard_waitstatus error\n"));
		return 1;
	}
	PRINTF(PSTR("sdhost_sdcard_identification: done\n"));
	return 0;
}

static 
char sd_initialize2(void)
{
	sd_power_cycle();

	PRINTF(PSTR("hardware_sdhost_setspeed to 400 kHz\n"));
	hardware_sdhost_setspeed(400000uL);
	hardware_sdhost_setbuswidth(0);		// 1-bit width
	if (sdhost_sdcard_poweron() == 0)
	{
		if (sdhost_sdcard_identification() == 0)
		{
			switch (sdhost_CardType)
			{
			case SDIO_STD_CAPACITY_SD_CARD_V2_0:
			case SDIO_HIGH_CAPACITY_SD_CARD:
				PRINTF(PSTR("hardware_sdhost_setspeed to 24 MHz\n"));
				hardware_sdhost_setspeed(24000000u);
				break;

			default:
				PRINTF(PSTR("hardware_sdhost_setspeed to 12 MHz\n"));
				hardware_sdhost_setspeed(12000000uL);
				break;
			}

#if WITHDEBUG
			{
				uint_fast64_t v = sdhost_sdcard_parse_CSD(sdhost_sdcard_CSD);
				PRINTF(PSTR("SD Card size = %lu KB (%lu MB) (%08lx:%08lx bytes)\n"), 
					(unsigned long) (v / 1024), 
					(unsigned long) (v / 1024 / 1024), 
					(unsigned long) (v >> 32), 
					(unsigned long) (v >> 0));

			}
#endif /* WITHDEBUG */
			
			return MMC_SUCCESS;
		}
		PRINTF("sdhost_sdcard_identification failed\n");
	}
	TP();
	return MMC_RESPONSE_ERROR;
}

static 
uint_fast64_t SD_ReadCardSize(void)
{
	return sdhost_sdcard_parse_CSD(sdhost_sdcard_CSD);
}



// for _USE_MKFS
static 
DRESULT SD_Get_Block_Size (
	BYTE drv,		/* Physical drive nmuber (0..) */
	DWORD  *buff	/* Data buffer to store read data */
	)
{
	enum { KB = 1024, MB = KB * KB };
	static const unsigned long aus [16] =
	{
		0,
		16 * KB,	// 1h 16 KB
		32 * KB,
		64 * KB,
		128 * KB,
		256 * KB,
		512 * KB,
		1 * MB,
		2 * MB,
		4 * MB,
		8 * MB,
		12 * MB,
		16 * MB,
		24 * MB,
		32 * MB,
		64 * MB,
	};
	static ALIGNX_BEGIN uint8_t sdhost_sdcard_SDSTATUS [64] ALIGNX_END;

	if (sdhost_read_registers_acmd(SD_CMD_SD_APP_STATUS, sdhost_sdcard_SDSTATUS, 64, 6, sizeof sdhost_sdcard_SDSTATUS) == 0)		// ACMD13
	{
		const unsigned au = array_get_bits(sdhost_sdcard_SDSTATUS, 512, 431, 4);	// [431:428] AU_SIZE
		const unsigned es = array_get_bits(sdhost_sdcard_SDSTATUS, 512, 423, 16);	// [423:408] ERASE_SIZE
		PRINTF(PSTR("ioctl: GET_BLOCK_SIZE: AU_SIZE=0x%02x, ERASE_SIZE=0x%04x\n"), au, es);
		* buff = aus [au] * es;
		return RES_OK;
	}
	return RES_ERROR;
}

// for _USE_MKFS
static 
DRESULT SD_Get_Sector_Count (
	BYTE drv,		/* Physical drive nmuber (0..) */
	LBA_t  *buff	/* Data buffer to store read data */
	)
{
	LBA_t val = SD_ReadCardSize() / FF_MAX_SS;
	if (val > USERFIRSTSBLOCK)
	{
		//PRINTF(PSTR("SD_Get_Sector_Count: drv=%d\n"), (int) drv);
		* buff = val - USERFIRSTSBLOCK;
		return RES_OK;
	}
	return RES_ERROR;
}

/* запись буферизированных данных на носитель */
static 
DRESULT SD_Sync(BYTE drv)
{
	//PRINTF(PSTR("SD_Sync: drv=%d\n"), (int) drv);
	if (sdhost_sdcard_waitstatus() != 0)
		return RES_ERROR;
	return RES_OK;
}

static 
DSTATUS SD_Initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	//PRINTF(PSTR("SD_Initialize: drv=%d\n"), (int) drv);
	if (1)
	{
#if WITHSDHCHW
		if (HARDWARE_SDIOSENSE_CD() == 0)
		{
			PRINTF(PSTR("SD_Initialize: STA_NODISK\n"));
			return STA_NOINIT;
		}
		//if (HARDWARE_SDIOSENSE_WP() != 0)
		//	return STA_PROTECT;
		char ec = sd_initialize2();
		PRINTF("sd_initialize2 return code ec=%02X\n", ec);
		return (ec == MMC_SUCCESS) ? 0 : STA_NOINIT;	// STA_NOINIT or STA_NODISK or STA_PROTECT
#endif /* WITHSDHCHW */
	}
	return STA_NOINIT;
}


static 
DSTATUS SD_Status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	//PRINTF(PSTR("SD_Status: drv=%d\n"), (int) drv);
	if (1)
	{
#if WITHSDHCHW
		if (HARDWARE_SDIOSENSE_CD() == 0)
			return STA_NODISK;
		if (HARDWARE_SDIOSENSE_WP() != 0)
			return STA_PROTECT;
		return 0;	// STA_NOINIT or STA_NODISK or STA_PROTECT
#endif /* WITHSDHCHW */
	}
	return STA_NODISK;
}



// write a size Byte big block beginning at the address.
static
DRESULT SD_disk_writeMisalign(
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write */
	)
{
	if (multisectorWriteProblems(count) || ((uintptr_t) buff % DCACHEROWSIZE) != 0)
	{
		while (count --)
		{
			static FATFSALIGN_BEGIN BYTE tmpbuf[FF_MAX_SS] FATFSALIGN_END;
			const BYTE *abuff = buff;
			if (((uintptr_t) buff % DCACHEROWSIZE) != 0)
			{
				memcpy(tmpbuf, buff, FF_MAX_SS);
				abuff = tmpbuf;
			}

			DRESULT ec = SD_disk_write(drv, abuff, sector, 1);
			if (ec != RES_OK)
				return ec;
			buff += FF_MAX_SS;
			sector += 1;
		}
		return RES_OK;
	}
	return SD_disk_write(drv, buff, sector, count);
}

// read a size Byte big block beginning at the address.
static
DRESULT SD_disk_readMisalign(
	BYTE drv,			/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read */
	)
{
	if (multisectorReadProblems(count) || ((uintptr_t) buff % DCACHEROWSIZE) != 0)
	{
		while (count --)
		{
			static FATFSALIGN_BEGIN BYTE tmpbuf[FF_MAX_SS] FATFSALIGN_END;
			BYTE *abuff = buff;
			if (((uintptr_t) buff % DCACHEROWSIZE) != 0)
			{
				abuff = tmpbuf;
			}

			DRESULT ec = SD_disk_read(drv, abuff, sector, 1);
			if (ec != RES_OK)
				return ec;
			if (((uintptr_t) buff % DCACHEROWSIZE) != 0)
			{
				memcpy(buff, tmpbuf, FF_MAX_SS);
			}
			buff += FF_MAX_SS;
			sector += 1;
		}
		return RES_OK;
	}
	return SD_disk_read(drv, buff, sector, count);
}

const struct drvfunc SD_drvfunc =
{
	SD_Initialize,
	SD_Status,
	SD_Sync,
	SD_disk_writeMisalign,
	SD_disk_readMisalign,
	SD_Get_Sector_Count,
	SD_Get_Block_Size,
};

#else /* WITHSDHCHW */


// Tokens (necessary  because at NPO/IDLE (and CS active) only 0xff is on the data/command line)
#define MMC_START_DATA_BLOCK_TOKEN          0xfe   // Data token start byte, Start Single Block Read
#define MMC_START_DATA_MULTIPLE_BLOCK_READ  0xfe   // Data token start byte, Start Multiple Block Read
#define MMC_START_DATA_BLOCK_WRITE          0xfe   // Data token start byte, Start Single Block Write
#define MMC_START_DATA_MULTIPLE_BLOCK_WRITE 0xfc   // Data token start byte, Start Multiple Block Write
#define MMC_STOP_DATA_MULTIPLE_BLOCK_WRITE  0xfd   // Data toke stop byte, Stop Multiple Block Write


// an affirmative R1 response (no errors)
#define MMC_R1_RESPONSE       0x00

// commands: first bit 0 (start bit), second 1 (transmission bit); CMD-number + Offsett 0x40
#define MMC_GO_IDLE_STATE          0x40     //CMD0
#define MMC_SEND_OP_COND           0x41     //CMD1
#define MMC_SEND_IF_COND           0x48     //CMD8 - added by mgs
#define MMC_READ_CSD               0x49     //CMD9
#define MMC_SEND_CID               0x4a     //CMD10
#define MMC_STOP_TRANSMISSION      0x4c     //CMD12
#define MMC_SEND_STATUS            0x4d     //CMD13
#define MMC_SET_BLOCKLEN           0x50     //CMD16 Set block length for next read/write
#define MMC_READ_SINGLE_BLOCK      0x51     //CMD17 Read block from memory
#define MMC_READ_MULTIPLE_BLOCK    0x52     //CMD18
#define MMC_CMD_WRITEBLOCK         0x54     //CMD20 Write block to memory
#define MMC_WRITE_BLOCK            0x58     //CMD24
#define MMC_WRITE_MULTIPLE_BLOCK   0x59     //CMD25
#define MMC_WRITE_CSD              0x5b     //CMD27 PROGRAM_CSD
#define MMC_SET_WRITE_PROT         0x5c     //CMD28
#define MMC_CLR_WRITE_PROT         0x5d     //CMD29
#define MMC_SEND_WRITE_PROT        0x5e     //CMD30
#define MMC_TAG_SECTOR_START       0x60     //CMD32
#define MMC_TAG_SECTOR_END         0x61     //CMD33
#define MMC_UNTAG_SECTOR           0x62     //CMD34
#define MMC_TAG_EREASE_GROUP_START 0x63     //CMD35
#define MMC_TAG_EREASE_GROUP_END   0x64     //CMD36
#define MMC_UNTAG_EREASE_GROUP     0x65     //CMD37
#define MMC_EREASE                 0x66     //CMD38
#define MMC_READ_OCR               0x67     //CMD39
#define MMC_CRC_ON_OFF             0x68     //CMD40

#define MMC_ACMD23	(0x40 + 23)	// ACMD23
#define MMC_ACMD41	(0x40 + 41)	// ACMD41

#define MMC_APP_CMD	(0x40 + 55)		// prefix command - application command - next ACMDxx
#define MMC_GEN_CMD	(0x40 + 56)		// Used either to transfer a Data Block to the card or to get a Data Block from the card for general purpose/application specific commands

/*
	This document describes the SPI mode to control the MMC/SDCs. 
	The SPI mode is an alternative operating mode that defined to use the MMC/SDCs without 
	native host interface. The communication protocol of the SPI mode is a little simple 
	compared to its native operating mode. The MMC/SDC can be attached to the most 
	microcontrollers via a generic SPI interface or GPIO ports. Therefore the SPI mode 
	is suitable for low cost embedded applications with no native host interface is available. 
	There are four different SPI modes, 0 to 3, depends on clock phase and polarity. 
	Mode 0 is defined for SDC. For the MMC, it is not the SPI timing, both latch and shift 
	actions are defined with rising edge of the SCLK, but it seems to work at mode 0 in the SPI mode. 
	Thus the Mode 0 (CPHA=0, CPOL=0) is the proper setting to control MMC/SDC, 
	but mode 3 (CPHA=1, CPOL=1) also works as well in most case.
  */


/* SD card definitions */
static char mmcInit(void);	// Вызывается, подразумевая что CS установлен

/* Assert the CS signal, active low (CS=0) */
static void SDCARD_CS_LOW(void)
{
	spi_select2(targetsdcard, SPIC_MODE0, SPIC_SPEEDSDCARD);	// MODE0/MODE3 - смотри комментарий выше
}

/* De-assert the CS signal (CS=1) */
static void SDCARD_CS_HIGH(void)
{
	spi_unselect(targetsdcard);
}
// установить скорость SPI для указанного канала контроллера
static void sdcard_spi_setfreq(unsigned long freq)
{
	hardware_spi_master_setfreq(SPIC_SPEEDSDCARD, freq);
}

#define RESPLIMIT 64000U
#define RESPLIMITREAD 640000L

// mmc Get Responce (one byte)
static unsigned char mmcGetResponseR1(void)
{
	unsigned long i;

	unsigned char response;

	for (i = 0; i <= RESPLIMIT; ++ i)
	{
		response = spi_read_byte(targetsdcard, 0xff);
		//if ((response & 0x80) == 0)
		if ((response & 0x80) == 0)
			break;
	}
	return response;
}
// mmc Get Responce (one byte) with busy
static unsigned char mmcGetResponseR1b(void)
{
	unsigned long i;

	unsigned char response;

	for (i = 0; i <= RESPLIMIT; ++ i)
	{
		response = spi_read_byte(targetsdcard, 0xff);
		//PRINTF(PSTR("mmcGetResponseR1b 1: %02x\n"), response);
		if ((response & 0x80) == 0)
			break;
	}
	// skip busy
	for (; i <= RESPLIMIT; ++ i)
	{
		unsigned char v = spi_read_byte(targetsdcard, 0xff);
		//PRINTF(PSTR("mmcGetResponseR1b 2: %02x\n"), v);
		if (v != 0)
			break;
	}
	return response;
}

static unsigned char waitwhilebusy(void)
{
	unsigned long i;
	// skip busy
	for (i = 0; i <= RESPLIMITREAD; ++ i)
	{
		unsigned char v = spi_read_byte(targetsdcard, 0xff);
		//PRINTF(PSTR("mmcGetResponseR1b 2: %02x\n"), v);
		if (v != 0)
			return 0;
	}
	return 1;
}

// mmc Get Responce (five bytes)
static unsigned char mmcGetResponseR3(unsigned long * rp)
{
	//Response comes 1-8bytes after command
	//the first bit will be a 0
	//followed by an error code
	//data will be 0xff until response
	unsigned long i;

	unsigned char response;

	for (i = 0; i <= RESPLIMIT; ++ i)
	{
		response = spi_read_byte(targetsdcard, 0xff);
		if ((response & 0x80) != 0)
			continue;
		{
			//R1print("R3", response);
			unsigned long r = 0;
			r = r * 256 + spi_read_byte(targetsdcard, 0xff);
			r = r * 256 + spi_read_byte(targetsdcard, 0xff);
			r = r * 256 + spi_read_byte(targetsdcard, 0xff);
			r = r * 256 + spi_read_byte(targetsdcard, 0xff);

			* rp = r;
			break;
		}
	}
	return response;
}

// mmc Get Responce (five bytes)
static unsigned char mmcGetResponseR7(unsigned long * rp)
{
	//Response comes 1-8bytes after command
	//the first bit will be a 0
	//followed by an error code
	//data will be 0xff until response
	unsigned long i;

	unsigned char response;

	for (i = 0; i <= RESPLIMIT; ++ i)
	{
		response = spi_read_byte(targetsdcard, 0xff);
		if ((response & 0x80) != 0)
			continue;
		{
			//R1print("R7", response);
			unsigned long r = 0;
			r = r * 256 + spi_read_byte(targetsdcard, 0xff);
			r = r * 256 + spi_read_byte(targetsdcard, 0xff);
			r = r * 256 + spi_read_byte(targetsdcard, 0xff);
			r = r * 256 + spi_read_byte(targetsdcard, 0xff);
			spi_read_byte(targetsdcard, 0xff);	// crc and end bit
			* rp = r;
			break;
		}

	}
	return response;
}


static uint_fast8_t mmcGetXXResponse(uint_fast8_t resp)
{
	//Response comes 1-8bytes after command
	//the first bit will be a 0
	//followed by an error code
	//data will be 0xff until response
	long i = 0;

	uint_fast8_t response;

	while (i <= RESPLIMITREAD)	// у больших карт памяти сильно увеличено время ожидания начала чтения данных
	{
		response = spi_read_byte(targetsdcard, 0xff);
		if (response == resp)
			break;
		i ++;
	}
	//PRINTF(PSTR("waiting count: %d\n"), i);
	return response;
}

/* вспомогательная подпрограмма расчёта CRC для одного бита.
   CRC хранится в старших 7 битах.
*/
static  uint_fast8_t crc7b1(uint_fast8_t crc, uint_fast8_t v1)
{
	if ((v1 != 0) != ((crc & 0x80) != 0))
	{
		crc <<= 1;
		crc ^= (0x09 << 1);
	}
	else
	{
		crc <<= 1;
	}

	return crc & 0xFF;
}

static void mmcSendDummyByte(void)
{
	spi_progval8_p1(targetsdcard, 0xff);	// dummy byte
	spi_complete(targetsdcard);
}

/* вспомогательная подпрограмма расчёта CRC для одного байта */
static uint_fast8_t crc7b8(uint_fast8_t crc, uint_fast8_t v8)
{
	crc = crc7b1(crc, v8 & 0x80);
	crc = crc7b1(crc, v8 & 0x40);
	crc = crc7b1(crc, v8 & 0x20);
	crc = crc7b1(crc, v8 & 0x10);
	crc = crc7b1(crc, v8 & 0x08);
	crc = crc7b1(crc, v8 & 0x04);
	crc = crc7b1(crc, v8 & 0x02);
	crc = crc7b1(crc, v8 & 0x01);

	return crc;
}
// send command to MMC`
// with real CRC value
static void mmcSendCmdCRC7(uint_fast8_t cmd, uint_fast32_t data)
{
	uint_fast8_t crc;

	crc = crc7b8(0x00, cmd);		
	crc = crc7b8(crc, data >> 24);	
	crc = crc7b8(crc, data >> 16);	
	crc = crc7b8(crc, data >> 8);	
	crc = crc7b8(crc, data >> 0);	

	spi_progval8_p1(targetsdcard, cmd);	// command
	spi_progval8_p2(targetsdcard, data >> 24);
	spi_progval8_p2(targetsdcard, data >> 16);
	spi_progval8_p2(targetsdcard, data >> 8);
	spi_progval8_p2(targetsdcard, data >> 0);
	spi_progval8_p2(targetsdcard, crc | 0x01);		// CRC and end transmit bit
	spi_complete(targetsdcard);
}
// send command to MMC
static void mmcSendCmd(uint_fast8_t cmd, uint_fast32_t data)
{
	spi_progval8_p1(targetsdcard, cmd);	// command
	spi_progval8_p2(targetsdcard, data >> 24);
	spi_progval8_p2(targetsdcard, data >> 16);
	spi_progval8_p2(targetsdcard, data >> 8);
	spi_progval8_p2(targetsdcard, data >> 0);
	spi_progval8_p2(targetsdcard, 0xff);	// CRC
	spi_complete(targetsdcard);
}

static uint_fast8_t mmcCardVersion2;
// Для НЕ SDHC (адресация идёт в байтах)
// Для SDHC (адресация идёт в 512-ти байтовых блоках)
// 512 требуется для SDSC
//	(a) Argument 0001h is byte address 0001h in the SDSC and 0001h block in SDHC and SDXC
//	(b) Argument 0200h is byte address 0200h in the SDSC and 0200h block in SDHC and SDXC
static uint_fast32_t mmcAddressMultiplier;

// set MMC in Idle mode
// Вызывается, подразумевая что CS установлен
static char mmcGoIdle(void)
{
	unsigned char response;
	unsigned long cmd8answer = 0;

	SDCARD_CS_HIGH();
	/////spi_read_byte(targetsdcard, 0xff);
	SDCARD_CS_LOW();
	mmcSendDummyByte();
	//Send Command 0 to put MMC in SPI mode
	mmcSendCmdCRC7(MMC_GO_IDLE_STATE, 0);	// CMD0 - Обязательно с правильным CRC
	//Now wait for READY RESPONSE
	if ((response = mmcGetResponseR1()) != 0x01)
	{
		return MMC_INIT_ERROR;
	}

	mmcSendDummyByte();
	mmcSendCmdCRC7(MMC_SEND_IF_COND, 0x000001aa);	// CMD8 - Обязательно с правильным CRC. 3.3 Volt VCC
	response = mmcGetResponseR7(& cmd8answer);
	if ((response & 0x04) != 0)	// illegal command or pattern not match
	{
		unsigned long cmd58answer = 0;
		mmcAddressMultiplier = MMC_SECTORSIZE;
		mmcCardVersion2 = 0;

		// if no responce
		// Ver2.00 or later SD Memory Card(voltage mismatch)
		// or Ver1.X SD Memory Card
		// or not SD Memory Card
		PRINTF(PSTR("Ver1.X SD Memory Card or not SD Memory Card\n")); 

		// check voltage range here
		mmcSendDummyByte();
		mmcSendCmd(0x40 + 58, 0);	// CMD58
		response = mmcGetResponseR3(& cmd58answer);
		PRINTF(PSTR("1: CMD58 has responce %02x, value = %08lx\n"), response, cmd58answer); 
		if (response == 0x00 || response == 0x01)
		{
			if ((cmd58answer & (1UL << 21)) || (cmd58answer & (1UL << 20)))
			{
				PRINTF(PSTR("3.3 volt VCC suitable.\n"));
			}
			else
			{
				PRINTF(PSTR("3.3 volt VCC NOT suitable.\n"));
				return MMC_INIT_ERROR;
			}
		}

		for (;;)
		{
			mmcSendDummyByte();
			mmcSendCmd(MMC_APP_CMD, 0);	// APP_CMD
			//Now wait for READY RESPONSE
			response = mmcGetResponseR1();
			PRINTF(PSTR("mmcGoIdle: 1: APP_CMD responce R1 = %02x\n"), response);
			if (response != 0x01 && response != 0x00)
				continue;

			mmcSendDummyByte();
			mmcSendCmd(MMC_ACMD41, 0);	// ACMD41(0)
			//Now wait for READY RESPONSE
			response = mmcGetResponseR1();
			PRINTF(PSTR("ACMD41 responce R1 = %02x\n"), response);
			if (response == 0x00)
				break;
		}
		//PRINTF(PSTR("ACMD41 has responce %02x\n"), response); 

		//return MMC_INIT_ERROR;
	}
	else if ((cmd8answer & 0xff) != 0xaa)
	{
		PRINTF(PSTR("Bad CMD8 answer. SD Card unuseable\n"));
		return MMC_INIT_ERROR;
	}
	else
	{
		unsigned long cmd58answer = 0;
		mmcCardVersion2 = 1;

		// Ver2.00 or later SD Memory Card
		PRINTF(PSTR("Ver2.00 or later SD Memory Card\n")); 	

		// check voltage range here
		mmcSendDummyByte();
		mmcSendCmd(0x40 + 58, 0);	// CMD58
		response = mmcGetResponseR3(& cmd58answer);
		PRINTF(PSTR("2: CMD58 has responce %02x, value = %08lx\n"), response, cmd58answer);
		if (response == 0x00 || response == 0x01)
		{
			if ((cmd58answer & (1UL << 21)) || (cmd58answer & (1UL << 20)))
			{
				PRINTF(PSTR("3.3 volt VCC suitable.\n"));
			}
			else
			{
				PRINTF(PSTR("3.3 volt VCC NOT suitable.\n"));
				return MMC_INIT_ERROR;
			}
		}

		mmcSendDummyByte();
		mmcSendCmd(0x40 + 59, 1);	// shitch CRC check on - required before ACMD41
		response = mmcGetResponseR1();
		//PRINTF(PSTR("CMD59 has responce %02x\n"), response); 

		for (;;)
		{
			mmcSendDummyByte();
			mmcSendCmdCRC7(MMC_APP_CMD, 0);	// APP_CMD
			//Now wait for READY RESPONSE
			response = mmcGetResponseR1();
			//PRINTF(PSTR("mmcGoIdle: 2: APP_CMD responce R1 = %02x\n"), response);
			if (response != 0x01 && response != 0x00)
				continue;

			mmcSendDummyByte();
			mmcSendCmdCRC7(MMC_ACMD41, 0x40000000);	// HCS (High Capacity Support)
			//Now wait for READY RESPONSE
			response = mmcGetResponseR1();
			//PRINTF(PSTR("ACMD41 responce R1 = %02x\n"), response);
			if (response == 0x00)
				break;
		}

		mmcSendDummyByte();
		mmcSendCmdCRC7(0x40 + 59, 0);	// shitch CRC check off
		response = mmcGetResponseR1();
		//PRINTF(PSTR("CMD59 has responce %02x\n"), response);

		// Check CCS bit (capacity) bit
		mmcSendDummyByte();
		mmcSendCmd(0x40 + 58, 0);	// CMD58
		response = mmcGetResponseR3(& cmd58answer);
		PRINTF(PSTR("3: CMD58 has responce %02x, value = %08lx\n"), response, cmd58answer); 
		if (response == 0x00 || response == 0x01)
		{
			if ((cmd58answer & 0x40000000) != 0)	//CCS (Card Capacity Status)
			{
				PRINTF(PSTR("SDHC or SDXC (High Capacity)\n"));
				mmcAddressMultiplier = 1;	// Для SDHC (адресация идёт в 512-ти байтовых блоках)
			}
			else
			{
				PRINTF(PSTR("SDSD - up to 2GB\n"));
				mmcAddressMultiplier = MMC_SECTORSIZE;	// Для обычных SD карт
			}
		}
	}

	//PRINTF(PSTR("mmcGoIdle() done.\n"));

	spi_read_byte(targetsdcard, 0xff);
	return (MMC_SUCCESS);
}


uint_fast64_t MMC_ReadCardSize(void)
{
	// Read contents of Card Specific Data (CSD)

	uint_fast64_t MMC_CardSize = 0;




	SDCARD_CS_LOW();

	// Asks the selected card to send its card-specific data (CSD)
	mmcSendDummyByte();
	mmcSendCmd(MMC_READ_CSD, 0x00000000);   // CMD 9

	if (mmcGetResponseR1() == 0x00)
	{
		// now look for the data token to signify the start of
		// the data
		if (mmcGetXXResponse(MMC_START_DATA_BLOCK_TOKEN) == MMC_START_DATA_BLOCK_TOKEN)
		{
			static ALIGNX_BEGIN uint8_t bv [16] ALIGNX_END;
			// Могут быть ограничения работы DMA с некоторыми видами памяти
			// Например в STM32F4x нет доступа к CCM памяти по DMA

			prog_spi_read_frame(targetsdcard, bv, 16);	// Программная реализация чтения работает с любой памятью, выдает 0xFF на MOSI
			
			spi_read_byte(targetsdcard, 0xff);  // least Ncr (max = 4 bytes) cycles after
			spi_read_byte(targetsdcard, 0xff);  // least Ncr (max = 4 bytes) cycles after
			spi_read_byte(targetsdcard, 0xff);  // least Ncr (max = 4 bytes) cycles after
			spi_read_byte(targetsdcard, 0xff);  // least Ncr (max = 4 bytes) cycles after
			// the card response is received
			spi_read_byte(targetsdcard, 0xff);
			MMC_CardSize = sdhost_sdcard_parse_CSD(bv);	// Обработка информациолнного блока (Code length is 128 bits = 16 bytes)

		}
	}
	else
	{
		mmcInit();
	}
	SDCARD_CS_HIGH();

	return (MMC_CardSize);

}

// Вызывается, подразумевая что CS установлен
static char mmcInit(void)
{
	char ec;
	int i;

	PRINTF(PSTR("SPI clock set to 400 kHz\n"));

	SDCARD_CS_HIGH();
	sdcard_spi_setfreq(400000);
	SDCARD_CS_LOW();

	for(i = 0; i <= 9; i ++)
		spi_read_byte(targetsdcard, 0xff);

	ec = mmcGoIdle();	// Вызывается, подразумевая что CS установлен

	if (ec == 0)
	{
		if (mmcCardVersion2 != 0)
		{
			PRINTF(PSTR("SPI clock set to 24 MHz\n"));
			SDCARD_CS_HIGH();
			sdcard_spi_setfreq(24000000);
			SDCARD_CS_LOW();
		}
		else
		{
			PRINTF(PSTR("SPI clock set to 12 MHz\n"));
			SDCARD_CS_HIGH();
			sdcard_spi_setfreq(12000000);
			SDCARD_CS_LOW();
		}
	}
	else
	{
			PRINTF(PSTR("mmcGoIdle return ec=%d\n"), ec);
	}
	
	return ec;
}

//--------------- set blocklength 2^n ------------------------------------------------------
static char mmcSetBlockLength (unsigned long blocklength)
{
	char ec = MMC_SUCCESS;
	// CS = LOW (on)
	SDCARD_CS_LOW();
	// Set the block length to read
	mmcSendDummyByte();
	mmcSendCmd(MMC_SET_BLOCKLEN, blocklength);

	// get response from MMC - make sure that its 0x00 (R1 ok response format)
	if (mmcGetResponseR1() != 0x00)
	{ 
		ec = mmcInit();	// Вызывается, подразумевая что CS установлен

		if (ec == 0)
		{
		  // Set the block length to read
			mmcSendDummyByte();
			mmcSendCmd(MMC_SET_BLOCKLEN, blocklength);
			ec = mmcGetResponseR1() == 0x00 ? MMC_SUCCESS : MMC_RESPONSE_ERROR;
		}
	}
	else
		ec = MMC_BLOCK_SET_ERROR;
	// перенесено сюда - перед отпусканием CS
	// Send 8 Clock pulses of delay.
	spi_read_byte(targetsdcard, 0xff);

	SDCARD_CS_HIGH();


	return ec;
} // Set block_length


/*
	0 = Idle
	1 = Ready
	2 = Ident
	3 = Stby
	4 = Transfer
	5 = Data
	6 = Rcv
	7 = Programming
	8 = Disconnected
	9 = Btst
	10-15 = reserved
*/

// Check if MMC card is still busy
static char mmcCheckBusy(void)
{
  //Response comes 1-8bytes after command
  //the first bit will be a 0
  //followed by an error code
  //data will be 0xff until response
  int i = 0;

  char response;
  char rvalue;
  while (i <= 64)
  {
    response = spi_read_byte(targetsdcard, 0xff);
    response &= 0x1f;
    switch (response)
    {
      case 0x05: rvalue = MMC_SUCCESS; break;
      case 0x0b: return MMC_CRC_ERROR;
      case 0x0d: return MMC_WRITE_ERROR;
      default:
        rvalue = MMC_OTHER_ERROR;
        break;
    }
    if (rvalue == MMC_SUCCESS)
		break;
    i ++;
  }
  i = 0;
  do
  {
    response = spi_read_byte(targetsdcard, 0xff);
    i ++;
  } while (response == 0);
  return rvalue;
}



// The card will respond with a standard response token followed by a data
// block suffixed with a 16 bit CRC.

// read a size Byte big block beginning at the address.
static char mmcReadSectors(
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read */
	)
{
	char rvalue = MMC_RESPONSE_ERROR;

	if (count == 1)
	{
		// single block read
		// CS = LOW (on)
		SDCARD_CS_LOW();

		mmcSendDummyByte();
		if (waitwhilebusy() != 0)	// 1) вставляю в начало всех функций чтения/записи/sync
		{
			SDCARD_CS_HIGH();
			return MMC_RESPONSE_ERROR;
		}

		// send read command MMC_READ_SINGLE_BLOCK=CMD17
		mmcSendDummyByte();
		mmcSendCmd(MMC_READ_SINGLE_BLOCK, sector * mmcAddressMultiplier);
		//mmcCheckBusy();	// INSERTED

		// Send 8 Clock pulses of delay, check if the MMC acknowledged the read block command
		// it will do this by sending an affirmative response
		// in the R1 format (0x00 is no errors)
		if (mmcGetResponseR1() == 0x00)
		{
			// now look for the data token to signify the start of
			// the data
			if (mmcGetXXResponse(MMC_START_DATA_BLOCK_TOKEN) == MMC_START_DATA_BLOCK_TOKEN)
			{
				// clock the actual data transfer and receive the bytes; mmcGetXXResponse automatically finds the Data Block
				spi_read_frame(targetsdcard, buff, MMC_SECTORSIZE);
				// get CRC bytes (not really needed by us, but required by MMC)
				spi_progval8_p1(targetsdcard, 0xff);
				spi_progval8_p2(targetsdcard, 0xff);
				spi_complete(targetsdcard);
				rvalue = MMC_SUCCESS;
			}
			else
			{
				// the data token was never received
				//PRINTF(PSTR("MMC_DATA_TOKEN_ERROR\n"));
				rvalue = MMC_DATA_TOKEN_ERROR;      // 3
			}
		}
		else
		{
			// the MMC never acknowledge the read command
			//PRINTF(PSTR("MMC_RESPONSE_ERROR\n"));
			rvalue = MMC_RESPONSE_ERROR;          // 2
		}

		SDCARD_CS_HIGH();
		////////spi_read_byte(targetsdcard, 0xff);
		return rvalue;
	}
	else
	{
		// multi block read
		// CS = LOW (on)
		SDCARD_CS_LOW();

		mmcSendDummyByte();
		if (waitwhilebusy() != 0)	// 1) вставляю в начало всех функций чтения/записи/sync
		{
			SDCARD_CS_HIGH();
			return MMC_RESPONSE_ERROR;
		}


		// send read command MMC_READ_SINGLE_BLOCK=CMD17
		mmcSendDummyByte();
		mmcSendCmd(MMC_READ_MULTIPLE_BLOCK, sector * mmcAddressMultiplier);
		//mmcCheckBusy();	// INSERTED

		// Send 8 Clock pulses of delay, check if the MMC acknowledged the read block command
		// it will do this by sending an affirmative response
		// in the R1 format (0x00 is no errors)
		if (mmcGetResponseR1() == 0x00)
		{
			do
			{
				// now look for the data token to signify the start of
				// the data
				if (mmcGetXXResponse(MMC_START_DATA_BLOCK_TOKEN) == MMC_START_DATA_BLOCK_TOKEN)
				{
					// clock the actual data transfer and receive the bytes; mmcGetXXResponse automatically finds the Data Block
					spi_read_frame(targetsdcard, buff, MMC_SECTORSIZE);
					// get CRC bytes (not really needed by us, but required by MMC)
					spi_progval8_p1(targetsdcard, 0xff);
					spi_progval8_p2(targetsdcard, 0xff);
					spi_complete(targetsdcard);
					rvalue = MMC_SUCCESS;

					buff += MMC_SECTORSIZE;
				}
				else
				{
					// the data token was never received
					PRINTF(PSTR("mmcReadSectors: MMC_DATA_TOKEN_ERROR\n"));
					rvalue = MMC_DATA_TOKEN_ERROR;      // 3
					break;
				}
			} while (-- count);

			mmcSendCmd(MMC_STOP_TRANSMISSION, 0);
			mmcSendDummyByte();
			if (mmcGetResponseR1b() != 0x00)
			{
				PRINTF(PSTR("mmcReadSectors: MMC_STOP_TRANSMISSION error\n"));
				rvalue = MMC_RESPONSE_ERROR;    
			}
			mmcCheckBusy();
		}
		else
		{
			// the MMC never acknowledge the read command
			PRINTF(PSTR("mmcReadSectors: MMC_READ_MULTIPLE_BLOCK: MMC_RESPONSE_ERROR\n"));
			rvalue = MMC_RESPONSE_ERROR;          // 2
		}

		SDCARD_CS_HIGH();
		////////spi_read_byte(targetsdcard, 0xff);
		return rvalue;
	}
}// mmc_read_block

// write a size Byte big block beginning at the address.
static char mmcWriteSectors(
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write */
	)
{
	char rvalue = MMC_RESPONSE_ERROR;         // MMC_SUCCESS;
	//  char c = 0x00;

	if (count == 1)
	{
		// signle block write
		// Set the block length to read
		//if (mmcSetBlockLength (count) == MMC_SUCCESS)   // block length could be set
		// CS = LOW (on)
		SDCARD_CS_LOW();
		// send write command
		mmcSendDummyByte();
		if (waitwhilebusy() != 0)	// 1) вставляю в начало всех функций чтения/записи/sync
		{
			SDCARD_CS_HIGH();
			return MMC_RESPONSE_ERROR;
		}

		mmcSendDummyByte();
		mmcSendCmd(MMC_WRITE_BLOCK, sector * mmcAddressMultiplier);

		// check if the MMC acknowledged the write block command
		// it will do this by sending an affirmative response
		// in the R1 format (0x00 is no errors)
		if (mmcGetXXResponse(MMC_R1_RESPONSE) == MMC_R1_RESPONSE)
		{
			spi_progval8_p1(targetsdcard, 0xff);
			// send the data token to signify the start of the data
			spi_progval8_p2(targetsdcard, MMC_START_DATA_BLOCK_WRITE);
			spi_complete(targetsdcard);

			// clock the actual data transfer and transmitt the bytes
			dcache_clean((uintptr_t) buff, MMC_SECTORSIZE);
			spi_send_frame(targetsdcard, buff, MMC_SECTORSIZE);

			// put CRC bytes (not really needed by us, but required by MMC)
			spi_progval8_p1(targetsdcard, 0xff);
			spi_progval8_p2(targetsdcard, 0xff);
			spi_complete(targetsdcard);
			// read the data response xxx0<status>1 : status 010: Data accected, status 101: Data
			//   rejected due to a crc error, status 110: Data rejected due to a Write error.
			mmcCheckBusy();
			rvalue = MMC_SUCCESS;
		}
		else
		{
			// the MMC never acknowledge the write command
			//PRINTF(PSTR("MMC_BLOCK_SET_ERROR\n"));
			rvalue = MMC_BLOCK_SET_ERROR;   // 2
		}
		// give the MMC the required clocks to finish up what ever it needs to do
		//  for (i = 0; i < 9; ++i)
		//    spi_progval8(targetsdcard, 0xff);

		SDCARD_CS_HIGH();
		// Send 8 Clock pulses of delay.
		////////spi_read_byte(targetsdcard, 0xff);
		return rvalue;
	}
	else
	{
		// write multiblock
		//PRINTF(PSTR("write multiblock, count=%d\n"), count);
		// Set the block length to read
		//if (mmcSetBlockLength (count) == MMC_SUCCESS)   // block length could be set
		// CS = LOW (on)
		SDCARD_CS_LOW();
		mmcSendDummyByte();
		if (waitwhilebusy() != 0)	// 1) вставляю в начало всех функций чтения/записи/sync
		{
			SDCARD_CS_HIGH();
			return MMC_RESPONSE_ERROR;
		}

		{
			unsigned char response;
			mmcSendDummyByte();
			mmcSendCmd(MMC_APP_CMD, 0);	// APP_CMD
			//Now wait for READY RESPONSE
			response = mmcGetResponseR1();
			if (/*response != 0x01 && */response != 0x00)
			{
				PRINTF(PSTR("mmcWriteSectors: 1: APP_CMD responce R1 = %02x\n"), response);
				SDCARD_CS_HIGH();
				return MMC_BLOCK_SET_ERROR;
			}

			mmcSendDummyByte();
			mmcSendCmd(MMC_ACMD23, count & 0x7FFFFF);	// ACMD23(number of blocks for erase) - 23 bits
			//Now wait for READY RESPONSE
			response = mmcGetResponseR1();
			if (/*response != 0x01 && */response != 0x00)
			{
				PRINTF(PSTR("mmcWriteSectors: 1: MMC_ACMD23 responce R1 = %02x\n"), response);
				SDCARD_CS_HIGH();
				return MMC_BLOCK_SET_ERROR;
			}
		}

		// send write command
		mmcSendDummyByte();
		mmcSendCmd(MMC_WRITE_MULTIPLE_BLOCK, sector * mmcAddressMultiplier);

		// check if the MMC acknowledged the write block command
		// it will do this by sending an affirmative response
		// in the R1 format (0x00 is no errors)
		if (mmcGetXXResponse(MMC_R1_RESPONSE) == MMC_R1_RESPONSE)
		{
			do
			{
				spi_progval8_p1(targetsdcard, 0xff);
				// send the data token to signify the start of the data
				spi_progval8_p2(targetsdcard, MMC_START_DATA_MULTIPLE_BLOCK_WRITE);
				spi_complete(targetsdcard);


				// clock the actual data transfer and transmitt the bytes
				dcache_clean((uintptr_t) buff, MMC_SECTORSIZE);
				spi_send_frame(targetsdcard, buff, MMC_SECTORSIZE);

				// put CRC bytes (not really needed by us, but required by MMC)
				spi_progval8_p1(targetsdcard, 0xff);
				spi_progval8_p2(targetsdcard, 0xff);
				spi_complete(targetsdcard);
				//unsigned char rr = spi_read_byte(targetsdcard, 0xff);
				//PRINTF(PSTR("mmcWriteSectors: 1: rr=%02x\n"), rr);
				//if ((rr & 0x1f) != 0x05)
				//{
				//	break;
				//}
				if (mmcCheckBusy() != MMC_SUCCESS)
					break;


				buff += MMC_SECTORSIZE;
			} while (-- count);
			// stop thh train
			//spi_progval8_p1(targetsdcard, 0xff);
			// send the data token to signify the start of the data
			spi_progval8_p1(targetsdcard, MMC_STOP_DATA_MULTIPLE_BLOCK_WRITE);
			spi_complete(targetsdcard);

			// read the data response xxx0<status>1 : status 010: Data accected, status 101: Data
			//   rejected due to a crc error, status 110: Data rejected due to a Write error.
			//waitwhilebusy();	// 1) отсюда убираю и вставляю в начало всех функций чтения/записи/sync

			rvalue = count == 0 ? MMC_SUCCESS : MMC_WRITE_ERROR;
		}
		else
		{
			// the MMC never acknowledge the write command
			//PRINTF(PSTR("MMC_BLOCK_SET_ERROR\n"));
			rvalue = MMC_BLOCK_SET_ERROR;   // 2
		}
		// give the MMC the required clocks to finish up what ever it needs to do
		//  for (i = 0; i < 9; ++i)
		//    spi_progval8(targetsdcard, 0xff);

		SDCARD_CS_HIGH();
		// Send 8 Clock pulses of delay.
		////////spi_read_byte(targetsdcard, 0xff);
		//PRINTF(PSTR("write multiblock, count=%d done\n"), count);
		return rvalue;
	}
} // mmc_write_block


// for _USE_MKFS
static 
DRESULT MMC_Get_Sector_Count (
	BYTE drv,		/* Physical drive nmuber (0..) */
	LBA_t  *buff	/* Data buffer to store read data */
	)
{
	* buff = MMC_ReadCardSize() / FF_MAX_SS;
	return RES_OK;
}

// for _USE_MKFS
static 
DRESULT MMC_Get_Block_Size (
	BYTE drv,		/* Physical drive nmuber (0..) */
	DWORD  *buff	/* Data buffer to store read data */
	)
{
	return RES_ERROR;
}

static 
char mmc_detectcard(void)
{
	sd_power_cycle();

	char ec;
	SDCARD_CS_LOW();
	ec = mmcInit();
	SDCARD_CS_HIGH();


	if (ec != MMC_SUCCESS)
		return ec;

	ec = mmcSetBlockLength(MMC_SECTORSIZE);
	return ec;
}


static 
DRESULT MMC_disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read */
)
{
	if (mmcReadSectors(buff, sector, count) != MMC_SUCCESS)
		return RES_ERROR;
	return RES_OK;
}

static 
DRESULT MMC_disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write */
)
{
	if (mmcWriteSectors(buff, sector, count) != MMC_SUCCESS)
		return RES_ERROR;
	return RES_OK;
}

/* запись буферизированных данных на носитель */
static 
DRESULT MMC_Sync(BYTE drv)
{
	SDCARD_CS_LOW();
	mmcSendDummyByte();
	if (waitwhilebusy() != 0)	// 1) вставляю в начало всех функций чтения/записи/sync
	{
		SDCARD_CS_HIGH();
		return RES_ERROR;
	}
	SDCARD_CS_HIGH();
	return RES_OK;
}

static 
DSTATUS MMC_Initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	//PRINTF(PSTR("MMC_Initialize: drv=%d\n"), (int) drv);
	if (1)
	{
		if (HARDWARE_SDIOSENSE_CD() == 0)
			return STA_NODISK;
		//if (HARDWARE_SDIOSENSE_WP() != 0)
		//	return STA_PROTECT;
		char ec = mmc_detectcard();
		return (ec == MMC_SUCCESS) ? 0 : STA_NODISK;	// STA_NOINIT or STA_NODISK or STA_PROTECT
	}
	return STA_NODISK;
}

static 
DSTATUS MMC_Status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	//PRINTF(PSTR("disk_status: drv=%d\n"), (int) drv);
	if (1)
	{
		if (HARDWARE_SDIOSENSE_CD() == 0)
			return STA_NODISK;
		if (HARDWARE_SDIOSENSE_WP() != 0)
			return STA_PROTECT;
		return 0;	// STA_NOINIT or STA_NODISK or STA_PROTECT
	}
	return STA_NODISK;
}


const struct drvfunc MMC_drvfunc =
{
	MMC_Initialize,
	MMC_Status,
	MMC_Sync,
	MMC_disk_write,
	MMC_disk_read,
	MMC_Get_Sector_Count,
	MMC_Get_Block_Size,
};

#endif /* WITHSDHCHW */

#endif /* WITHUSESDCARD */

#if WITHSDHCHW

#if CPUSTYLE_STM32F7XX

void /*__attribute__((interrupt)) */ SDMMC1_IRQHandler(void)
{
}

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

static volatile int sd_event_xx;
static volatile int sd_event_value;


static void
SetEvents(events_t v)
{
	sd_event_xx = 1;
	sd_event_value = v;
}

static int
WaitEvents(events_t e, int type)
{
	unsigned long t;
	for (t = 0; t < 100000000; ++ t)
	{
		IRQL_t oldIrql;
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		if (sd_event_xx != 0 /*&& (sd_event_value & e) != 0 */)
		{
			sd_event_xx = 0;
			LowerIrql(oldIrql);
			return EV_SD_READY;
		}
		LowerIrql(oldIrql);
	}
	PRINTF("WaitEvents: timeout\n");
	return EV_SD_ERROR;
}


//SDMMC1 Interrupt
void /*__attribute__((interrupt)) */ SDMMC1_IRQHandler(void)
{
   const uint32_t f = SDMMC1->STA & SDMMC1->MASK;
   events_t e = 0;
   if (f & (SDMMC_STA_CCRCFAIL | SDMMC_STA_CTIMEOUT))
   {
      //Command path error
      e |= EV_SD_ERROR;
      //Mask error interrupts, leave flags in STA for the further analisys
      SDMMC1->MASK &= ~(SDMMC_STA_CCRCFAIL | SDMMC_STA_CTIMEOUT);
   }
   if (f & (SDMMC_STA_CMDREND | SDMMC_STA_CMDSENT))
   {
      //We have finished the command execution
      e |= EV_SD_READY;
      SDMMC1->MASK &= ~ (SDMMC_STA_CMDREND | SDMMC_STA_CMDSENT);
      SDMMC1->ICR = (SDMMC_ICR_CMDRENDC | SDMMC_ICR_CMDSENTC);
   }
   const uint_fast32_t fdata = f & (SDMMC_STA_DATAEND | SDMMC_STA_DBCKEND | SDMMC_STA_DCRCFAIL | SDMMC_STA_DTIMEOUT);
   if (fdata)
   {
      //We have finished the data send/receive
      e |= EV_SD_DATA;
      SDMMC1->MASK &= ~(SDMMC_STA_DATAEND | SDMMC_STA_DBCKEND | SDMMC_STA_DCRCFAIL | SDMMC_STA_DTIMEOUT);
   }

   SetEvents(e);
}

#endif /* CPUSTYLE_STM32H7XX */

#endif /* WITHSDHCHW */

#if WITHSDHCHW

void hardware_sdhost_setbuswidth(uint_fast8_t use4bit)
{
	//PRINTF(PSTR("hardware_sdhost_setbuswidth: use4bit=%u\n"), (unsigned) use4bit);

#if ! WITHSDHCHW4BIT
	use4bit = 0;
#endif /* ! WITHSDHCHW4BIT */

#if CPUSTYLE_R7S721

	if (use4bit != 0)
		SDHI0.SD_OPTION &= ~ (1U << 15);	// WIDTH 0: 4-bit width
	else
		SDHI0.SD_OPTION |= (1U << 15);		// WIDTH 1: 1-bit width

#elif CPUSTYLE_STM32F4XX

	SDIO->CLKCR = (SDIO->CLKCR & ~ (SDIO_CLKCR_WIDBUS)) |
		(use4bit != 0 ? 0x01 : 0x00) * SDIO_CLKCR_WIDBUS_0 |	// 01: 4-wide bus mode: SDMMC_D[3:0] used
		0;

#elif CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	SDMMC1->CLKCR = (SDMMC1->CLKCR & ~ (SDMMC_CLKCR_WIDBUS)) |
		(use4bit != 0 ? 0x01 : 0x00) * SDMMC_CLKCR_WIDBUS_0 |	// 01: 4-wide bus mode: SDMMC_D[3:0] used
		0;

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	SD0->HOST_CTRL_BLOCK_GAP_CTRL = (SD0->HOST_CTRL_BLOCK_GAP_CTRL & ~ (0x02uL)) |
				(use4bit != 0) * 0x02uL |	// Data_Transfer_Width_SD1_or_SD4
				0;

#elif CPUSTYLE_T113 || CPUSTYLE_F133
	//#warning CPUSTYLE_T113 or CPUSTYLE_F133 to be implemented
	SMHCHARD_PTR->SMHC_CTYPE = (SMHCHARD_PTR->SMHC_CTYPE & ~ UINT32_C(0x03)) |
		(use4bit ? UINT32_C(0x01) : UINT32_C(0x00)) |	// 00: 1-bit width, 01: 4-biut width
		0;

#elif CPUSTYLE_T507

	switch(use4bit ? 4 : 1)
	{
	case 1:
		SMHCHARD_PTR->SMHC_CTYPE = 0;
		break;
	case 4:
		SMHCHARD_PTR->SMHC_CTYPE = 1;
		break;
	case 8:
		SMHCHARD_PTR->SMHC_CTYPE = 2;
		break;
	default:
	}

#else
	#error Wrong CPUSTYLE_xxx
#endif
}

void hardware_sdhost_setspeed(unsigned long ticksfreq)
{
#if CPUSTYLE_R7S721
	// Использование автоматического расчёта предделителя
	//unsigned long ticksfreq = 400000uL;	// 400 kHz -> 260 kHz
	//unsigned long ticksfreq = 24000000u;	// 24 MHz -> 16.666 MHz
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_p1clock(ticksfreq), 0, (512 | 256 | 128 | 64 | 32 | 16 | 8 | 4 | 2), & value, 0);
	PRINTF(PSTR("hardware_sdhost_setspeed: ticksfreq=%lu, prei=%lu\n"), (unsigned long) ticksfreq, (unsigned long) prei);

	while ((SDHI0.SD_INFO2 & (1u << 13)) == 0)	// SCLKDIVEN
	{
		//PRINTF(PSTR("hardware_sdhost_setspeed: SCLKDIVEN set clock prohibited, SD_INFO2=%08X\n"), SDHI0.SD_INFO2);
		TP();
	}
	while ((SDHI0.SD_INFO2 & (1u << 14)) != 0)	// CBSY
	{
		//PRINTF(PSTR("hardware_sdhost_setspeed: CBSY set clock prohibited, SD_INFO2=%08X\n"), SDHI0.SD_INFO2);
		TP();
	}

	SDHI0.SD_CLK_CTRL =
		0 * (1U << 9) |		// SDCLKOFFEN=0
		1 * (1U << 8) |		// SCLKEN=1
		((1U << prei) >> 1) * (1U << 0) |
		0;

#elif CPUSTYLE_STM32F4XX

	#if defined (RCC_DCKCFGR2_SDIOSEL)
		// stm32f446xx и некоторые другие

		RCC->DCKCFGR2 = (RCC->DCKCFGR2 & ~ (RCC_DCKCFGR2_SDIOSEL)) |
			0 * RCC_DCKCFGR2_SDIOSEL |	// 0: 48 MHz clock is selected as SDMMC clock
			0;

	#elif defined (RCC_DCKCFGR_SDIOSEL)
		// stm32f469xx, stm32f479xx

		RCC->DCKCFGR = (RCC->DCKCFGR & ~ (RCC_DCKCFGR_SDIOSEL)) |
			0 * RCC_DCKCFGR_SDIOSEL |	// 0: 48 MHz clock is selected as SDMMC clock
			0;
	#else
		// Остальные
	#endif

	const uint_fast32_t stm32f4xx_pllq = stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
	// Использование автоматического расчёта делителя
	// PLLQ: Main PLL (PLL) division factor for USB OTG FS, SDIO and random number generator clocks
	// Should be 48 MHz or less for SDIO and 48 MHz with small tolerance.
	// See RCC_PLLCFGR_PLLQ usage
	const uint32_t SDIOCLK = PLL_FREQ / stm32f4xx_pllq;

	const unsigned value = ulmin(calcdivround2(SDIOCLK, ticksfreq) - 2, 255);

	SDIO->CLKCR = (SDIO->CLKCR & ~ (SDIO_CLKCR_CLKDIV_Msk)) |
		(value & SDIO_CLKCR_CLKDIV_Msk);

#elif CPUSTYLE_STM32F7XX

	RCC->DCKCFGR2 = (RCC->DCKCFGR2 & ~ (RCC_DCKCFGR2_SDMMC1SEL)) |
		0 * RCC_DCKCFGR2_SDMMC1SEL |	// 0: 48 MHz clock is selected as SDMMC clock
		0;

	const uint_fast32_t stm32f7xx_pllq = stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
	// Использование автоматического расчёта делителя
	// PLLQ: Main PLL (PLL) division factor for USB OTG FS, SDIO and random number generator clocks
	// Should be 48 MHz or less for SDIO and 48 MHz with small tolerance.
	// See RCC_PLLCFGR_PLLQ usage
	const uint32_t SDMMCCLK = PLL_FREQ / stm32f7xx_pllq;
	// Использование автоматического расчёта делителя
	// Источником тактирования SDMMC сейчас установлен внутренний генератор 48 МГц
	//const uint32_t stm32f4xx_48mhz = PLL_FREQ / stm32f7xx_pllq;
	const unsigned value = ulmin(calcdivround2(SDMMCCLK, ticksfreq) - 2, 255);

	//PRINTF(PSTR("hardware_sdhost_setspeed: stm32f7xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f7xx_pllq, stm32f4xx_48mhz);
	PRINTF(PSTR("hardware_sdhost_setspeed: CLKCR_CLKDIV=%lu\n"), (unsigned long) value);

	SDMMC1->CLKCR = (SDMMC1->CLKCR & ~ (SDMMC_CLKCR_CLKDIV)) |
		(value & SDMMC_CLKCR_CLKDIV);

#elif CPUSTYLE_STM32H7XX

	//RCC->DCKCFGR2 = (RCC->DCKCFGR2 & ~ (RCC_DCKCFGR2_SDMMC1SEL)) |
	//	0 * RCC_DCKCFGR2_SDMMC1SEL |	// 0: 48 MHz clock is selected as SDMMC clock
	//	0;

	const uint_fast32_t stm32h7xx_pllq = stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
	// Использование автоматического расчёта делителя
	// PLLQ: Main PLL (PLL) division factor for USB OTG FS, SDIO and random number generator clocks
	// Should be 48 MHz or less for SDIO and 48 MHz with small tolerance.
	// See RCC_PLLCFGR_PLLQ usage
	const uint32_t SDMMCCLK = PLL_FREQ / stm32h7xx_pllq;;
	// Использование автоматического расчёта делителя
	// Источником тактирования SDMMC сейчас установлен внутренний генератор 48 МГц
	//const uint32_t stm32f4xx_48mhz = PLL_FREQ / stm32h7xx_pllq;
	const unsigned value = ulmin(calcdivround2(SDMMCCLK / 2, ticksfreq), 0x03FF);

	PRINTF(PSTR("hardware_sdhost_setspeed: stm32h7xx_pllq=%lu, SDMMCCLK=%lu, PLL_FREQ=%lu\n"), (unsigned long) stm32h7xx_pllq, SDMMCCLK, PLL_FREQ);
	PRINTF(PSTR("hardware_sdhost_setspeed: CLKCR_CLKDIV=%lu\n"), (unsigned long) value);

	SDMMC1->CLKCR = (SDMMC1->CLKCR & ~ (SDMMC_CLKCR_CLKDIV_Msk)) |
		((value << SDMMC_CLKCR_CLKDIV_Pos) & SDMMC_CLKCR_CLKDIV_Msk);



#elif CPUSTYLE_STM32MP1

	//RCC->DCKCFGR2 = (RCC->DCKCFGR2 & ~ (RCC_DCKCFGR2_SDMMC1SEL)) |
	//	0 * RCC_DCKCFGR2_SDMMC1SEL |	// 0: 48 MHz clock is selected as SDMMC clock
	//	0;

	//const uint_fast32_t stm32h7xx_pllq = stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
	// Использование автоматического расчёта делителя
	// PLLQ: Main PLL (PLL) division factor for USB OTG FS, SDIO and random number generator clocks
	// Should be 48 MHz or less for SDIO and 48 MHz with small tolerance.
	// See RCC_PLLCFGR_PLLQ usage
	const uint32_t SDMMCCLK = stm32mp1_sdmmc1_2_get_freq();
	// Использование автоматического расчёта делителя
	// Источником тактирования SDMMC сейчас установлен внутренний генератор 48 МГц
	//const uint32_t stm32f4xx_48mhz = PLL_FREQ / stm32h7xx_pllq;
	const unsigned clkdiv = ulmin(calcdivround2(SDMMCCLK / 2, ticksfreq), 0x03FF);

	//PRINTF(PSTR("hardware_sdhost_setspeed: stm32h7xx_pllq=%lu, SDMMCCLK=%lu, PLL_FREQ=%lu\n"), (unsigned long) stm32h7xx_pllq, SDMMCCLK, PLL_FREQ);
	//PRINTF(PSTR("hardware_sdhost_setspeed: CLKCR_CLKDIV=%lu\n"), (unsigned long) value);

	SDMMC1->CLKCR = (SDMMC1->CLKCR & ~ (SDMMC_CLKCR_CLKDIV_Msk)) |
		((clkdiv << SDMMC_CLKCR_CLKDIV_Pos) & SDMMC_CLKCR_CLKDIV_Msk);


#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	const unsigned long ref = xc7z_get_sdio_freq();
	unsigned divider = calcdivround2(ref / 2, ticksfreq);
	divider = ulmin(divider, 255);
	divider = ulmax(divider, 1);
	PRINTF("hardware_sdhost_setspeed: ref=%lu, divider=%u\n", ref, divider);
	if (ticksfreq <= 400000uL)
	{
		SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL =
			(SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL & ~ (0x00FF00uL)) |
			(0x80uL << 8) |	// SDCLK_Frequency_Select: 80h - base clock divided by 256
			0;
	}
	else
	{
		SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL =
			(SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL & ~ (0x00FF00uL)) |
			((uint_fast32_t) divider << 8) |	// SDCLK_Frequency_Select: 10h - base clock divided by 32
			0;
	}

	SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL |= 0x01;	// Internal_Clock_Enable
	// Wait Internal_Clock_Stable
	while ((SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL & 0x02) == 0)
		;

#elif CPUSTYLE_T113 || CPUSTYLE_F133
	#warning CPUSTYLE_T113 or CPUSTYLE_F133 to be implemented
	SMHCHARD_PTR->SMHC_CTRL;

#elif CPUSTYLE_T507
	#warning CPUSTYLE_T507 to be implemented

#else
	#error Wrong CPUSTYLE_xxx
#endif
}

#if CPUSTYLE_R7S721

void r7s721_sdhi0_dma_handler(void)
{
	PRINTF(PSTR("r7s721_sdhi0_dma_handler trapped\n"));
	for (;;)
		;
}

#endif

void hardware_sdhost_initialize(void)
{
#if CPUSTYLE_R7S721

	// Инициализация SD CARD интерфейса на R7S721
	/* ---- Supply clock to the SDHI(channel 0) ---- */
	CPG.STBCR12 &= ~ ((1U << 3) | (1U << 2));	// Module Stop 123, 122  00: The SD host interface 00 runs.
	(void) CPG.STBCR12;			/* Dummy read */

	SDHI0.SOFT_RST = 0x0000;	// SDRST 0: Reset
	SDHI0.SOFT_RST = 0x0001;	// SDRST 1: Reset released

	hardware_sdhost_setbuswidth(0);
	hardware_sdhost_setspeed(400000uL);

    arm_hardware_set_handler_system(DMAINT14_IRQn, r7s721_sdhi0_dma_handler);

	HARDWARE_SDIOSENSE_INITIALIZE();
	HARDWARE_SDIO_INITIALIZE();	// Подсоединить контроллер к выводам процессора

#elif CPUSTYLE_STM32F4XX

	RCC->APB2ENR |= RCC_APB2ENR_SDIOEN;   // подаем тактирование на SDIO
	__DSB();

	// hwrdware flow control отключен - от этого зависит проверка состояния txfifo & rxfifo
	SDIO->CLKCR =
		1 * SDIO_CLKCR_CLKEN |
		(255 & SDIO_CLKCR_CLKDIV_Msk) |
		1 * SDIO_CLKCR_HWFC_EN |
		1 * SDIO_CLKCR_PWRSAV |		// выключается clock без обращений
		0;

	hardware_sdhost_setbuswidth(0);
	hardware_sdhost_setspeed(400000uL);

	arm_hardware_set_handler_system(SDIO_IRQn, SDIO_IRQHandler);
	arm_hardware_set_handler_system(DMA2_Stream6_IRQn, DMA2_Stream6_IRQHandler);

	HARDWARE_SDIOSENSE_INITIALIZE();
	HARDWARE_SDIO_INITIALIZE();	// Подсоединить контроллер к выводам процессора
	// разрешить тактирование карты памяти
	SDIO->POWER = 3 * SDIO_POWER_PWRCTRL_0;

#elif CPUSTYLE_STM32F7XX

	RCC->APB2ENR |= RCC_APB2ENR_SDMMC1EN;   // подаем тактирование на SDMMC1
	__DSB();

	// hwrdware flow control отключен - от этого зависит проверка состояния txfifo & rxfifo
	SDMMC1->CLKCR =
		1 * SDMMC_CLKCR_CLKEN |
		(255 & SDMMC_CLKCR_CLKDIV) |
		1 * SDMMC_CLKCR_HWFC_EN |
		1 * SDMMC_CLKCR_PWRSAV |		// выключается clock без обращений
		0;

	hardware_sdhost_setbuswidth(0);
	hardware_sdhost_setspeed(400000uL);

	arm_hardware_set_handler_system(SDMMC1_IRQn, SDMMC1_IRQHandler);
	arm_hardware_set_handler_system(DMA2_Stream6_IRQn, DMA2_Stream6_IRQHandler);

	HARDWARE_SDIOSENSE_INITIALIZE();
	HARDWARE_SDIO_INITIALIZE();	// Подсоединить контроллер к выводам процессора
	// разрешить тактирование карты памяти
	SDMMC1->POWER = 3 * SDMMC_POWER_PWRCTRL_0;

#elif CPUSTYLE_STM32H7XX

	RCC->AHB3ENR |= RCC_AHB3ENR_SDMMC1EN;   // подаем тактирование на SDMMC1
	(void) RCC->AHB3ENR;
	__DSB();

	HARDWARE_SDIOSENSE_INITIALIZE();
	HARDWARE_SDIO_INITIALIZE();	// Подсоединить контроллер к выводам процессора

	// hwrdware flow control отключен - от этого зависит проверка состояния txfifo & rxfifo
	SDMMC1->CLKCR =
		SDMMC_CLKCR_CLKDIV |
		1 * SDMMC_CLKCR_HWFC_EN |
		1 * SDMMC_CLKCR_PWRSAV |		// выключается clock без обращений
		0;

	hardware_sdhost_setbuswidth(0);
	hardware_sdhost_setspeed(400000uL);

	arm_hardware_set_handler_system(SDMMC1_IRQn, SDMMC1_IRQHandler);

	// разрешить тактирование карты памяти
	SDMMC1->POWER = 3 * SDMMC_POWER_PWRCTRL_0;

#elif CPUSTYLE_STM32MP1

	//	0x0: hclk6 clock selected as kernel peripheral clock
	//	0x1: pll3_r_ck clock selected as kernel peripheral clock
	//	0x2: pll4_p_ck clock selected as kernel peripheral clock
	//	0x3: hsi_ker_ck clock selected as kernel peripheral clock (default after reset)

	RCC->SDMMC12CKSELR = (RCC->SDMMC12CKSELR & ~ (RCC_SDMMC12CKSELR_SDMMC12SRC_Msk)) |
			(0x03uL << RCC_SDMMC12CKSELR_SDMMC12SRC_Pos) |	// hsi_ker_ck
			0;
	(void) RCC->SDMMC12CKSELR;

	RCC->MP_AHB6ENSETR = RCC_MP_AHB6ENSETR_SDMMC1EN;   // подаем тактирование на SDMMC1
	(void) RCC->MP_AHB6ENSETR;
	RCC->MP_AHB6LPENSETR = RCC_MP_AHB6LPENSETR_SDMMC1LPEN;   // подаем тактирование на SDMMC1
	(void) RCC->MP_AHB6LPENSETR;

	HARDWARE_SDIOSENSE_INITIALIZE();
	HARDWARE_SDIO_INITIALIZE();	// Подсоединить контроллер к выводам процессора

	// hwrdware flow control отключен - от этого зависит проверка состояния txfifo & rxfifo
	SDMMC1->CLKCR =
		SDMMC_CLKCR_CLKDIV |
		1 * SDMMC_CLKCR_HWFC_EN |
		1 * SDMMC_CLKCR_PWRSAV |		// выключается clock без обращений
		0;

	hardware_sdhost_setbuswidth(0);
	hardware_sdhost_setspeed(400000uL);

	arm_hardware_set_handler_system(SDMMC1_IRQn, SDMMC1_IRQHandler);

	// разрешить тактирование карты памяти
	SDMMC1->POWER = 3 * SDMMC_POWER_PWRCTRL_0;

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	const unsigned sdioix = 0;	// SD0

//    SCLR->SDIO_RST_CTRL |= (0x11uL << sdioix);
//    SCLR->SDIO_RST_CTRL &= ~ (0x11uL << sdioix);

	SCLR->SLCR_UNLOCK = 0x0000DF0DU;
	SCLR->APER_CLK_CTRL |= (0x01uL << (10 + sdioix));	// APER_CLK_CTRL.SDI0_CPU_1XCLKACT
    //EMIT_MASKWRITE(0XF8000150, 0x00003F33U ,0x00001001U),	// SDIO_CLK_CTRL
	SCLR->SDIO_CLK_CTRL = (SCLR->SDIO_CLK_CTRL & ~ (0x00003F33U)) |
		((uint_fast32_t) SCLR_SDIO_CLK_CTRL_DIVISOR_VALUE << 8) | // DIVISOR
		(0x00uL << 4) |	// SRCSEL - 0x: IO PLL
		(0x01uL << sdioix) | // CLKACT0 - SDIO 0 reference clock active
		0;


	SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL =
		(SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL & ~ ((0x0FuL << 16))) |
		(0x0EuL << 16);	// Data_Timeout_Counter_Value

	// SD_Bus_Power off
	SD0->HOST_CTRL_BLOCK_GAP_CTRL =
			(SD0->HOST_CTRL_BLOCK_GAP_CTRL & ~ (0x01uL << 8)) |
			0 * (0x01uL << 8) |	// 0 - Power off
			0;
	// SD_Bus_Voltage_Select
	SD0->HOST_CTRL_BLOCK_GAP_CTRL =
			(SD0->HOST_CTRL_BLOCK_GAP_CTRL & ~ (0x07uL << 9)) |
			(0x07uL << 9) |	// 111b - 3.3 Flattop
			0;
	// SD_Bus_Power on
	SD0->HOST_CTRL_BLOCK_GAP_CTRL =
			(SD0->HOST_CTRL_BLOCK_GAP_CTRL & ~ (0x01uL << 8)) |
			1 * (0x01uL << 8) |	// 1 - Power on
			0;
	// DMA_Select
	SD0->HOST_CTRL_BLOCK_GAP_CTRL =
			(SD0->HOST_CTRL_BLOCK_GAP_CTRL & ~ (0x03uL << 3)) |
			(0x00uL << 3) |	// SDMA select
			0;

	HARDWARE_SDIOSENSE_INITIALIZE();
	HARDWARE_SDIO_INITIALIZE();	// Подсоединить контроллер к выводам процессора
	ASSERT(((SD0->Vendor_Version_Number & 0xFFFF0000uL) >> 16) == 0x8901uL);

	hardware_sdhost_setbuswidth(0);
	hardware_sdhost_setspeed(400000uL);

//	PRINTF("SD0->CAPABILITIES=%08X\n", SD0->CAPABILITIES);
//	PRINTF("SD0->CAPABILITIES.SDMA_Support=%d\n", (SD0->CAPABILITIES >> 22) & 0x01);
//	PRINTF("SD0->CAPABILITIES.Voltage_Support_3_3_V=%d\n", (SD0->CAPABILITIES >> 24) & 0x01);

	//arm_hardware_set_handler_system(SDIO0_IRQn, SDIO0_IRQHandler);

#elif CPUSTYLE_T113 || CPUSTYLE_F133

	unsigned ix = SMHCHARD_IX;

	{
		// Automatic divisors calculation
		// Для SDHI2 отличаются значения поля выбора источника тактирования
		unsigned clksrc = 1;	// 001: PLL_PERI(1X)
		uint_fast32_t needfreq = UINT32_C(200) * 1000 * 1000;
		unsigned dvalue;
		unsigned prei = calcdivider(calcdivround2(allwnr_t113_get_peripll1x_freq(), needfreq), 4, (8 | 4 | 1 | 2), & dvalue, 1);
		SMHCHARD_CCU_CLK_REG =
			//(UINT32_C(1) << 31) |
			clksrc * (UINT32_C(1) << 24) |
			prei * (UINT32_C(1) << 8) |
			dvalue * (UINT32_C(1) << 0) |
			0;
		SMHCHARD_CCU_CLK_REG |= UINT32_C(1) << 31;	// SCLK_GATING
	}

	CCU->SMHC_BGR_REG |= UINT32_C(1) << (ix + 0); // SMHCx_GATING
	(void) CCU->SMHC_BGR_REG;
	CCU->SMHC_BGR_REG &= ~ (UINT32_C(1) << (ix + 16)); // SMHCx_RESET
	(void) CCU->SMHC_BGR_REG;
	CCU->SMHC_BGR_REG |= UINT32_C(1) << (ix + 16); // SMHCx_RESET
	(void) CCU->SMHC_BGR_REG;

	//PRINTF("SMHCHARD_FREQ=%u MHz\n", (unsigned) (SMHCHARD_FREQ / 1000 / 1000));

	hardware_sdhost_setbuswidth(0);
	hardware_sdhost_setspeed(400000uL);

	HARDWARE_SDIOSENSE_INITIALIZE();
	HARDWARE_SDIO_INITIALIZE();

#elif CPUSTYLE_T507

	unsigned ix = SMHCHARD_IX;

	{
		// Automatic divisors calculation
		unsigned clksrc = 1;	// 01: PLL_PERI0(2X)
		uint_fast32_t needfreq = UINT32_C(200) * 1000 * 1000;
		unsigned dvalue;
		unsigned prei = calcdivider(calcdivround2(allwnr_t507_get_pll_peri0_x2_freq(), needfreq), 4, (8 | 4 | 1 | 2), & dvalue, 1);
		SMHCHARD_CCU_CLK_REG =
			//(UINT32_C(1) << 31) |
			clksrc * (UINT32_C(1) << 24) |
			prei * (UINT32_C(1) << 8) |
			dvalue * (UINT32_C(1) << 0) |
			0;
		SMHCHARD_CCU_CLK_REG |= UINT32_C(1) << 31;	// SCLK_GATING
	}

	CCU->SMHC_BGR_REG |= UINT32_C(1) << (ix + 0); // SMHCx_GATING
	(void) CCU->SMHC_BGR_REG;
	CCU->SMHC_BGR_REG &= ~ (UINT32_C(1) << (ix + 16)); // SMHCx_RESET
	(void) CCU->SMHC_BGR_REG;
	CCU->SMHC_BGR_REG |= UINT32_C(1) << (ix + 16); // SMHCx_RESET
	(void) CCU->SMHC_BGR_REG;

	//PRINTF("SMHCHARD_FREQ=%u MHz\n", (unsigned) (SMHCHARD_FREQ / 1000 / 1000));

	hardware_sdhost_setbuswidth(0);
	hardware_sdhost_setspeed(400000uL);

	HARDWARE_SDIOSENSE_INITIALIZE();
	HARDWARE_SDIO_INITIALIZE();

#else
	#error Wrong CPUSTYLE_xxx
#endif
}

// в ответ на прерывание изменения состояния card detect
void hardware_sdhost_detect(uint_fast8_t Card_Inserted)
{
#if CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	//	This bit indicates whether a card has been
	//	inserted. Changing from 0 to 1 generates a Card
	//	Insertion interrupt in the Normal Interrupt Status
	//	register and changing from 1 to 0 generates a
	//	Card Removal Interrupt in the Normal Interrupt
	//	Status register. The Software Reset For All in the
	//	Software Reset register shall not affect this bit. If
	//	a Card is removed while its power is on and its
	//	clock is oscillating, the HC shall clear SD Bus
	//	Power in the Power Control register and SD Clock
	//	Enable in the Clock control register. In addition
	//	the HD should clear the HC by the Software Reset
	//	For All in Software register. The card detect is
	//	active regardless of the SD Bus Power.

	return;

	// SD_Bus_Power off
	SD0->HOST_CTRL_BLOCK_GAP_CTRL =
			(SD0->HOST_CTRL_BLOCK_GAP_CTRL & ~ (0x01uL << 8)) |
			0 * (0x01uL << 8) |	// 0 - Power off
			0;

	SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL |= (1u << 24);	// Software_Reset_for_All
		PRINTF("SD0->CAPABILITIES=%08X\n", (unsigned) SD0->CAPABILITIES);
		PRINTF("SD0->CAPABILITIES=%08X\n", (unsigned) SD0->CAPABILITIES);
		PRINTF("SD0->CAPABILITIES=%08X\n", (unsigned) SD0->CAPABILITIES);
		PRINTF("SD0->CAPABILITIES=%08X\n", (unsigned) SD0->CAPABILITIES);
	SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL &= ~ (1u << 24);	// Software_Reset_for_All

	// SD_Bus_Voltage_Select
	SD0->HOST_CTRL_BLOCK_GAP_CTRL =
			(SD0->HOST_CTRL_BLOCK_GAP_CTRL & ~ (0x07uL << 9)) |
			(0x07uL << 9) |	// 111b - 3.3 Flattop
			0;
	// SD_Bus_Power on
	SD0->HOST_CTRL_BLOCK_GAP_CTRL =
			(SD0->HOST_CTRL_BLOCK_GAP_CTRL & ~ (0x01uL << 8)) |
			1 * (0x01uL << 8) |	// 1 - Power on
			0;


    //hardware_sdhost_initialize();

 #endif
}

#endif /* WITHSDHCHW && ! CPUSTYLE_ALLWINNER */

