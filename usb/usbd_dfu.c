/*
 * usbd_dfu.c
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
*/


#include "hardware.h"
#include "pio.h"
#include "board.h"
#include "audio.h"
#include "formats.h"

#if WITHUSBHW && WITHUSBDFU

#include "usb_core.h"



typedef USBALIGN_BEGIN struct
{
  USBALIGN_BEGIN union
  {
    uint32_t d32 [USBD_DFU_XFER_SIZE / sizeof (uint32_t)];
    uint8_t  d8 [USBD_DFU_XFER_SIZE];
  } USBALIGN_END buffer;

  uint8_t              USBALIGN_BEGIN dev_state USBALIGN_END;
  uint8_t              USBALIGN_BEGIN dev_status [DFU_STATUS_DEPTH] USBALIGN_END;
  uint8_t              manif_state;

  uint32_t             wblock_num;
  uint32_t             wlength;
  uint32_t             data_ptr;
  volatile uint32_t    alt_setting;

} USBALIGN_END USBD_DFU_HandleTypeDef;


static USBD_DFU_HandleTypeDef gdfu;

static USBD_StatusTypeDef  USBD_DFU_Init (USBD_HandleTypeDef *pdev, int_fast8_t cfgidx);
static USBD_StatusTypeDef  USBD_DFU_DeInit (USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx);
static USBD_StatusTypeDef  USBD_DFU_Setup (USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req);
static USBD_StatusTypeDef  USBD_DFU_DataIn(USBD_HandleTypeDef *pdev, uint_fast8_t epnum);
static USBD_StatusTypeDef  USBD_DFU_DataOut(USBD_HandleTypeDef *pdev, uint_fast8_t epnum);
static USBD_StatusTypeDef  USBD_DFU_EP0_RxReady(USBD_HandleTypeDef *pdev);
static USBD_StatusTypeDef  USBD_DFU_EP0_TxSent(USBD_HandleTypeDef *pdev);
static USBD_StatusTypeDef  USBD_DFU_SOF(USBD_HandleTypeDef *pdev);
static USBD_StatusTypeDef  USBD_DFU_IsoINIncomplete(USBD_HandleTypeDef *pdev, uint_fast8_t epnum);
static USBD_StatusTypeDef  USBD_DFU_IsoOutIncomplete(USBD_HandleTypeDef *pdev, uint_fast8_t epnum);
static void DFU_Detach(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req);
static void DFU_Download(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req);
static void DFU_Upload(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req);
static void DFU_GetStatus(USBD_HandleTypeDef *pdev);
static void DFU_ClearStatus(USBD_HandleTypeDef *pdev);
static void DFU_GetState(USBD_HandleTypeDef *pdev);
static void DFU_Abort(USBD_HandleTypeDef *pdev);
static void DFU_Leave(USBD_HandleTypeDef *pdev);


/////////
// https://github.com/renesas-rz/rza1_qspi_flash/blob/master/qspi_flash.c

#define targetdataflash 0
#define SPIMODE_AT26DF081A	SPIC_MODE3

// Use block SPIBSC0
// 17. SPI Multi I/O Bus Controller
//
// P4_2: WP#
// P4_4: SCLK
// P4_5: CS#
// P4_6: MOSI
// P4_7: MISO
static void spidf_initialize(void)
{
#if 0
	// spi multi-io hang on
	CPG.STBCR9 &= ~ CPG_STBCR9_BIT_MSTP93;	// Module Stop 93	- 0: Clock supply to channel 0 of the SPI multi I/O bus controller is runnuing.
	(void) CPG.STBCR9;			/* Dummy read */
#endif

#if 0
	SPIBSC0.CMNCR |= SPIBSC_CMNCR_MD;	// SPI mode.
	(void) SPIBSC0.CMNCR;
	ASSERT(SPIBSC0.CMNCR & SPIBSC_CMNCR_MD);

	SPIBSC0.SPBCR = (SPIBSC0.SPBCR & ~ (SPIBSC_SPBCR_BRDV | SPIBSC_SPBCR_SPBR)) |
		(0 << SPIBSC_SPBCR_BRDV_SHIFT) |	// 0..3
		(2 << SPIBSC_SPBCR_SPBR_SHIFT) |	// 0..255
		0;

	arm_hardware_pio4_alternative(1U << 4, R7S721_PIOALT_4);	/* P4_4 SCLK / SPBCLK_0 */
	arm_hardware_pio4_alternative(1U << 5, R7S721_PIOALT_4);	/* P4_5 CS# / SPBSSL_0 */
	arm_hardware_pio4_alternative(1U << 6, R7S721_PIOALT_4);	/* P4_6 MOSI / SPBIO00_0 */
	arm_hardware_pio4_alternative(1U << 7, R7S721_PIOALT_4);	/* P4_7 MISO / SPBIO10_0 */

	/*
		The transfer format is determined based on the following registers.
		- Common control register (CMNCR)
		- SSL delay register (SSLDR)
		- Bit rate setting register (SPBCR)
		- SPI mode control register (SMCR)
		- SPI mode command setting register (SMCMR)
		- SPI mode address setting register (SMADR)
		- SPI mode option setting register (SMOPR)
		- SPI mode enable setting register (SMENR)
		- SPI mode read data register (SMRDR)
		- SPI mode write data register (SMWDR)
		- SPI mode dummy cycle setting register (SMDMCR)
		- SPI mode DDR enable register (SMDRENR)*
	*/
	SPIBSC0.SMENR = (SPIBSC0.SMENR & ~ (SPIBSC_SMENR_ADE | SPIBSC_SMENR_SPIDE)) |
		(0x00 << SPIBSC_SMENR_ADE_SHIFT) | /* No address send */
		(0x08 << SPIBSC_SMENR_SPIDE_SHIFT) | /* 8 bits transferred (enables data at address 0 of the SPI mode read/write data registers 0) */
		0;

	SPIBSC0.SMCMR =
		(0x9F << SPIBSC_SMCMR_CMD_SHIFT) | /* 0x9f read id register */
		(0x00 << SPIBSC_SMCMR_OCMD_SHIFT) | /* xxxx */
		0;

	SPIBSC0.SMCR = SPIBSC_SMCR_SPIE | SPIBSC_SMCR_SPIRE | SPIBSC_SMCR_SPIWE;

	SPIBSC0.SMWDR0.UINT32 = 0xFFFFFFFF;
	TP();
	while ((SPIBSC0.CMNSR & SPIBSC_CMNSR_TEND) == 0)
		;
	debug_printf_P(PSTR("SMRDR0=%08lX\n"), SPIBSC0.SMRDR0.UINT32);
	TP();
	SPIBSC0.SMWDR0.UINT32 = 0xFFFFFFFF;
	TP();
	while ((SPIBSC0.CMNSR & SPIBSC_CMNSR_TEND) == 0)
		;
	debug_printf_P(PSTR("SMRDR0=%08lX\n"), SPIBSC0.SMRDR0.UINT32);

#endif

#if 0
	SPIBSC0.SPBCR = 0x200;	// baud rate
	SPIBSC0.SSLDR = 0x00;	// delay
	SPIBSC0.DRCR = 0x0000;

	SPIBSC0.CMNCR =
		SPIBSC_CMNCR_MD |	// spi mode
		(0x03 << SPIBSC_CMNCR_MOIIO3_SHIFT) |
		(0x03 << SPIBSC_CMNCR_MOIIO2_SHIFT) |
		(0x03 << SPIBSC_CMNCR_MOIIO1_SHIFT) |
		(0x03 << SPIBSC_CMNCR_MOIIO0_SHIFT) |
		(0x03 << SPIBSC_CMNCR_IO3FV_SHIFT) |
		(0x03 << SPIBSC_CMNCR_IO2FV_SHIFT) |
		(0x03 << SPIBSC_CMNCR_IO0FV_SHIFT) |
		1 * SPIBSC_CMNCR_CPHAR |
		1 * SPIBSC_CMNCR_CPHAT |
		0;

	SPIBSC0.SMENR =
		0;

	SPIBSC0.SMCR =
		SPIBSC_SMCR_SPIE |
		SPIBSC_SMCR_SPIRE |
		SPIBSC_SMCR_SPIWE |
		0;
#endif

	// Connrect I/O pins
	arm_hardware_pio4_outputs(1U << 2, 1U << 2);				/* P4_2 WP / SPBIO20_0 */
	arm_hardware_pio4_outputs(1U << 3, 1U << 3);				/* P4_3 NC / SPBIO30_0 */
	//arm_hardware_pio4_alternative(1U << 4, R7S721_PIOALT_4);	/* P4_4 SCLK / SPBCLK_0 */
	arm_hardware_pio4_outputs(1U << 4, 1U << 4);	/* P4_4 SCLK / SPBCLK_0 */
	//arm_hardware_pio4_alternative(1U << 5, R7S721_PIOALT_4);	/* P4_5 CS# / SPBSSL_0 */
	arm_hardware_pio4_outputs(1U << 5, 1 * (1U << 5));			/* P4_5 CS# / SPBSSL_0 */
	//arm_hardware_pio4_alternative(1U << 6, R7S721_PIOALT_4);	/* P4_6 MOSI / SPBIO00_0 */
	arm_hardware_pio4_outputs(1U << 6, 1U << 6);	/* P4_6 MOSI / SPBIO00_0 */
	//arm_hardware_pio4_alternative(1U << 7, R7S721_PIOALT_4);	/* P4_7 MISO / SPBIO10_0 */
	arm_hardware_pio4_inputs(1U << 7);	/* P4_7 MISO / SPBIO10_0 */
}

#define SPIDF_MISO() ((R7S721_INPUT_PORT(4) & (1U << 7)) != 0)
#define SPIDF_MOSI(v) do { if (v) R7S721_TARGET_PORT_S(4, (1U << 6)); else R7S721_TARGET_PORT_C(4, (1U << 6)); } while (0)
#define SPIDF_SCLK(v) do { if (v) R7S721_TARGET_PORT_S(4, (1U << 4)); else R7S721_TARGET_PORT_C(4, (1U << 4)); } while (0)

static uint_fast8_t spidf_rbit(uint_fast8_t v)
{
	uint_fast8_t r;
	SPIDF_MOSI(v);
	SPIDF_SCLK(0);
	r = SPIDF_MISO();
	SPIDF_SCLK(1);
	return r;
}

static void spidf_wbit(uint_fast8_t v)
{
	SPIDF_MOSI(v);
	SPIDF_SCLK(0);
	SPIDF_SCLK(1);
}


uint_fast8_t spidf_read_byte(spitarget_t target, uint_fast8_t v)
{
	uint_fast8_t r = 0;

	r = r * 2 + spidf_rbit(v & 0x80);
	r = r * 2 + spidf_rbit(v & 0x40);
	r = r * 2 + spidf_rbit(v & 0x20);
	r = r * 2 + spidf_rbit(v & 0x10);
	r = r * 2 + spidf_rbit(v & 0x08);
	r = r * 2 + spidf_rbit(v & 0x04);
	r = r * 2 + spidf_rbit(v & 0x02);
	r = r * 2 + spidf_rbit(v & 0x01);

	return r;
}

static void spidf_write_byte(spitarget_t target, uint_fast8_t v)
{
	spidf_wbit(v & 0x80);
	spidf_wbit(v & 0x40);
	spidf_wbit(v & 0x20);
	spidf_wbit(v & 0x10);
	spidf_wbit(v & 0x08);
	spidf_wbit(v & 0x04);
	spidf_wbit(v & 0x02);
	spidf_wbit(v & 0x01);
}

void spidf_uninitialize(void)
{
	//arm_hardware_pio4_inputs(0xFC);		// Отключить процессор от SERIAL FLASH
#if 0
	// spi multi-io hang off
	CPG.STBCR9 |= CPG_STBCR9_BIT_MSTP93;	// Module Stop 93	- 1: Clock supply to channel 0 of the SPI multi I/O bus controller is halted.
	(void) CPG.STBCR9;			/* Dummy read */
#endif
}

void spidf_select(spitarget_t target, uint_fast8_t mode)
{
#if 1
	// Connrect I/O pins
	arm_hardware_pio4_outputs(0x7C, 0x7C);
#endif
	do {	R7S721_TARGET_PORT_C(4, (1U << 5)); } while (0);
}

void spidf_unselect(spitarget_t target)
{
	do {	R7S721_TARGET_PORT_S(4, (1U << 5)); } while (0);
#if 1
	arm_hardware_pio4_inputs(0x7C);		// Отключить процессор от SERIAL FLASH
#endif
}

void spidf_to_read(spitarget_t target)
{
}

void spidf_to_write(spitarget_t target)
{
}

void spidf_progval8_p1(spitarget_t target, uint_fast8_t sendval)
{
	spidf_write_byte(target, sendval);
}

void spidf_progval8_p2(spitarget_t target, uint_fast8_t sendval)
{
	spidf_write_byte(target, sendval);
}

uint_fast8_t spidf_complete(spitarget_t target)
{
	return 0;
}

uint_fast8_t spidf_progval8(spitarget_t target, uint_fast8_t sendval)
{
	return spidf_read_byte(target, sendval);
}



static unsigned char dataflash_read_status(
	spitarget_t target	/* addressing to chip */
	)
{
	unsigned char v;

	spidf_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spidf_progval8(target, 0x05);		/* read status register */

	spidf_to_read(target);
	v = spidf_read_byte(target, 0xff);
	spidf_to_write(target);

	spidf_unselect(target);	/* done sending data to target chip */

	return v;
}

static int timed_dataflash_read_status(
	spitarget_t target
	)
{
	unsigned long w = 40000;
	while (w --)
	{
		if ((dataflash_read_status(target) & 0x01) == 0)
			return 0;
	}
	debug_printf_P(PSTR("DATAFLASH timeout error\n"));
	return 1;
}

static int largetimed_dataflash_read_status(
	spitarget_t target
	)
{
	unsigned long w = 40000000;
	while (w --)
	{
		if ((dataflash_read_status(target) & 0x01) == 0)
			return 0;
	}
	debug_printf_P(PSTR("DATAFLASH erase timeout error\n"));
	return 1;
}
// Atmel Data Flash: Read: ID = 0x1F devId = 0x4501, mf_dlen=0x00

static int testchipDATAFLASH(void)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	unsigned char mf_id;	// Manufacturer ID
	unsigned char mf_devid1;	// device ID (part 1)
	unsigned char mf_devid2;	// device ID (part 2)
	unsigned char mf_dlen;	// Extended Device Information String Length


	/* Ожидание бита ~RDY в слове состояния. Для FRAM не имеет смысла.
	Вставлено для возможности использования DATAFLASH */

	if (timed_dataflash_read_status(target))
		return 1;

	spidf_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spidf_progval8(target, 0x9f);		/* read id register */

	spidf_to_read(target);

	//prog_spidf_to_read();
	mf_id = spidf_read_byte(target, 0xff);
	mf_devid1 = spidf_read_byte(target, 0xff);
	mf_devid2 = spidf_read_byte(target, 0xff);
	mf_dlen = spidf_read_byte(target, 0xff);

	spidf_to_write(target);

	spidf_unselect(target);	/* done sending data to target chip */

	debug_printf_P(PSTR("Read: ID = 0x%02X devId = 0x%02X%02X, mf_dlen=0x%02X\n"), mf_id, mf_devid1, mf_devid2, mf_dlen);
	//debug_printf_P(PSTR("Need: ID = 0x%02X devId = 0x%02X%02X, mf_dlen=0x%02X\n"), 0x1f, 0x45, 0x01, 0x00);


	// Read SFDP
	spidf_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spidf_progval8_p1(target, 0x5A);		/* The Read SFDP instruction code is 0x5A */

	const uint_fast32_t flashoffset = 0x000000uL;
	spidf_progval8_p2(target, flashoffset >> 16);
	spidf_progval8_p2(target, flashoffset >> 8);
	spidf_progval8_p2(target, flashoffset >> 0);
	spidf_progval8_p2(target, 0x00);	// dummy byte
	spidf_complete(target);	/* done sending data to target chip */

	spidf_to_read(target);

	uint_fast32_t signature = 0;
	signature = signature * 256 | spidf_read_byte(target, 0xff);
	signature = signature * 256 | spidf_read_byte(target, 0xff);
	signature = signature * 256 | spidf_read_byte(target, 0xff);
	signature = signature * 256 | spidf_read_byte(target, 0xff);

	spidf_to_write(target);

	spidf_unselect(target);	/* done sending data to target chip */

	debug_printf_P(PSTR("SFDP: signature=%08lX\n"), signature);

	return 0;
}
#if 0
static int eraseDATAFLASH(void)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	spidf_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spidf_progval8(target, 0x06);		/* write enable */
	spidf_unselect(target);	/* done sending data to target chip */

	if (timed_dataflash_read_status(target))
		return 1;

	spidf_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spidf_progval8(target, 0x60);		/* chip erase */
	spidf_unselect(target);	/* done sending data to target chip */

	if (largetimed_dataflash_read_status(target))
		return 1;

	if ((dataflash_read_status(target) & (0x01 << 5)) != 0)	// write error
	{
		debug_printf_P(PSTR("Erase error\n"));
		return 1;
	}
	return 0;
}
#endif

static int prepareDATAFLASH(void)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */


	const uint_fast8_t status = dataflash_read_status(target);

	if ((status & 0x1C) != 0)
	{
		if (timed_dataflash_read_status(target))
			return 1;
		debug_printf_P(PSTR("Clear write protect bits\n"));
		spidf_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
		spidf_progval8(target, 0x06);		/* write enable */
		spidf_unselect(target);	/* done sending data to target chip */

		// Write Status Register
		spidf_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
		spidf_progval8(target, 0x01);		/* write status register ccommand */
		spidf_progval8(target, 0x00);		/* status register data */
		spidf_unselect(target);	/* done sending data to target chip */
	}

	return timed_dataflash_read_status(target);
}
#if 0
static int writeEnableDATAFLASH(void)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	spidf_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spidf_progval8(target, 0x06);		/* write enable */
	spidf_unselect(target);	/* done sending data to target chip */

	return 0;
}

static int writeDisableDATAFLASH(void)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	spidf_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spidf_progval8(target, 0x04);		/* write disable */
	spidf_unselect(target);	/* done sending data to target chip */

	return 0;
}
#endif

static void sectoreraseDATAFLASH(unsigned long flashoffset)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	//debug_printf_P(PSTR(" Erase sector at address %08lX\n"), flashoffset);

	timed_dataflash_read_status(target);

	spidf_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spidf_progval8(target, 0x06);		/* write enable */
	spidf_unselect(target);	/* done sending data to target chip */

	// start byte programm
	spidf_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spidf_progval8_p1(target, 0xD8);				/* SECTOR ERASE */

	spidf_progval8_p2(target, flashoffset >> 16);
	spidf_progval8_p2(target, flashoffset >> 8);
	spidf_progval8_p2(target, flashoffset >> 0);

	spidf_complete(target);	/* done sending data to target chip */

	spidf_unselect(target);	/* done sending data to target chip */

	//timed_dataflash_read_status(target);
}

static void writesinglepageDATAFLASH(unsigned long flashoffset, const unsigned char * data, unsigned long len)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	timed_dataflash_read_status(target);
	//debug_printf_P(PSTR(" Prog to address %08lX %02X\n"), flashoffset, len);

	spidf_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spidf_progval8(target, 0x06);		/* write enable */
	spidf_unselect(target);	/* done sending data to target chip */

	// start byte programm
	spidf_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spidf_progval8_p1(target, 0x02);				/* Page Program */

	spidf_progval8_p2(target, flashoffset >> 16);
	spidf_progval8_p2(target, flashoffset >> 8);
	spidf_progval8_p2(target, flashoffset >> 0);

	while (len --)
		spidf_progval8_p2(target, (unsigned char) * data ++);	// data

	spidf_complete(target);	/* done sending data to target chip */

	spidf_unselect(target);	/* done sending data to target chip */

	//timed_dataflash_read_status(target);
}


static unsigned long ulmin(
	unsigned long a,
	unsigned long b)
{
	return a < b ? a : b;
}

static int writeDATAFLASH(unsigned long flashoffset, const unsigned char * data, unsigned long len)
{
	//debug_printf_P(PSTR("Write to address %08lX %02X\n"), flashoffset, len);
	while (len != 0)
	{
		unsigned long offset = flashoffset & 0xFF;
		unsigned long part = ulmin(len, ulmin(256, 256 - offset));

		writesinglepageDATAFLASH(flashoffset, data, part);

		len -= part;
		flashoffset += part;
		data += part;
	}
	return 0;
}

static int verifyDATAFLASH(unsigned long flashoffset, const unsigned char * data, unsigned long len)
{
	unsigned long count;
	unsigned long err = 0;
	unsigned char v;
	spitarget_t target = targetdataflash;	/* addressing to chip */

	//debug_printf_P(PSTR("Compare from address %08lX\n"), flashoffset);

	spidf_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spidf_progval8(target, 0x03);		/* sequential read block */

	spidf_progval8(target, flashoffset >> 16);
	spidf_progval8(target, flashoffset >> 8);
	spidf_progval8(target, flashoffset >> 0);

	spidf_to_read(target);

	for (count = 0; count < len; ++ count)
	{
		v = spidf_read_byte(target, 0xff);
		if (v != data [count])
		{
			debug_printf_P(PSTR("Data mismatch at %08lx: read=%02x, expected=%02x\n"), flashoffset + count, v, data [count]);
			err = 1;
			break;
		}
	}

	spidf_to_write(target);

	spidf_unselect(target);	/* done sending data to target chip */

	if (err)
		debug_printf_P(PSTR("Done compare, have errors\n"));

	return err;
}

static void readDATAFLASH(unsigned long flashoffset, unsigned char * data, unsigned long len)
{
	unsigned long count;
	unsigned long err = 0;
	unsigned char v;
	spitarget_t target = targetdataflash;	/* addressing to chip */

	//debug_printf_P(PSTR("Compare from address %08lX\n"), flashoffset);

	spidf_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spidf_progval8(target, 0x03);		/* sequential read block */

	spidf_progval8(target, flashoffset >> 16);
	spidf_progval8(target, flashoffset >> 8);
	spidf_progval8(target, flashoffset >> 0);

	spidf_to_read(target);

	for (count = 0; count < len; ++ count)
	{
		 data [count] = spidf_read_byte(target, 0xff);
	}

	spidf_to_write(target);

	spidf_unselect(target);	/* done sending data to target chip */
}



/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */

typedef struct
{
  const char* pStrDesc;
  uint16_t (* Init)     (void);
  uint16_t (* DeInit)   (void);
  uint16_t (* Erase)    (uint32_t Add);
  uint16_t (* Write)    (uint8_t *src, uint32_t dest, uint32_t Len);
  uint8_t* (* Read)     (uint32_t src, uint8_t *dest, uint32_t Len);
  uint16_t (* GetStatus)(uint32_t Add, uint8_t cmd, uint8_t *buff);
}
USBD_DFU_MediaTypeDef;
/**
  * @}
  */


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Memory initialization routine.
  * @retval USBD_OK if operation is successful, MAL_FAIL else.
  */
static uint16_t MEM_If_Init_HS(void)
{
	PRINTF(PSTR("MEM_If_Init_HS\n"));
	spidf_initialize();
	testchipDATAFLASH();
	prepareDATAFLASH();	// снятие защиты со страниц при первом програмимровании через SPI интерфейс
	return (USBD_OK);
}

/**
  * @brief  De-Initializes Memory.
  * @retval USBD_OK if operation is successful, MAL_FAIL else.
  */
static uint16_t MEM_If_DeInit_HS(void)
{
	PRINTF(PSTR("MEM_If_DeInit_HS\n"));
	spidf_uninitialize();
	PRINTF(PSTR("MEM_If_DeInit_HS 1\n"));
	return (USBD_OK);
}

/**
  * @brief  Erase sector.
  * @param  Add: Address of sector to be erased.
  * @retval USBD_OK if operation is successful, MAL_FAIL else.
  */
// called on each PAGESIZE region (see strFlashDesc)
static uint16_t MEM_If_Erase_HS(uint32_t Addr)
{
	//PRINTF(PSTR("MEM_If_Erase_HS: addr=%08lX\n"), Addr);
	if (Addr >= BOOTLOADER_SELFBASE && (Addr + BOOTLOADER_PAGESIZE) <= (BOOTLOADER_SELFBASE + BOOTLOADER_APPFULL))
		sectoreraseDATAFLASH(Addr);
	return (USBD_OK);
}

/**
  * @brief  Memory write routine.
  * @param  src: Pointer to the source buffer. Address to be written to.
  * @param  dest: Pointer to the destination buffer.
  * @param  Len: Number of data to be written (in bytes).
  * @retval USBD_OK if operation is successful, MAL_FAIL else.
  */
static uint16_t MEM_If_Write_HS(uint8_t *src, uint32_t dest, uint32_t Len)
{
	//PRINTF(PSTR("MEM_If_Write_HS: addr=%08lX, len=%03lX\n"), dest, Len);
	if (dest >= BOOTLOADER_SELFBASE && (dest + Len) <= (BOOTLOADER_SELFBASE + BOOTLOADER_APPFULL))
	{
#if WITHISBOOTLOADER
		// физическое выполненеие записи
		if (writeDATAFLASH(dest, src, Len))
			return USBD_FAIL;
#endif /* WITHISBOOTLOADER */
	}
#if WITHISBOOTLOADER
	if (dest >= BOOTLOADER_APPAREA && (dest + Len) <= (BOOTLOADER_APPAREA + BOOTLOADER_APPFULL))
		memcpy((void *) dest, src, Len);
	else if (dest >= BOOTLOADER_APPBASE)
		memcpy((void *) ((uintptr_t) dest - BOOTLOADER_APPBASE + BOOTLOADER_APPAREA), src, Len);
	else
	{
		/* программируем бутлоадер */
	}
#endif /* WITHISBOOTLOADER */
	return (USBD_OK);
}

/**
  * @brief  Memory read routine.
  * @param  src: Pointer to the source buffer. Address to be written to.
  * @param  dest: Pointer to the destination buffer.
  * @param  Len: Number of data to be read (in bytes).
  * @retval Pointer to the physical address where data should be read.
  */
static uint8_t *MEM_If_Read_HS(uint32_t src, uint8_t *dest, uint32_t Len)
{
	/* Return a valid address to avoid HardFault */
	/* USER CODE BEGIN 4 */
	spitarget_t target = targetdataflash;	/* addressing to chip */
	if (timed_dataflash_read_status(target))
		return dest;	// todo: error handling need
	readDATAFLASH(src, dest, Len);
	return dest;
	/* USER CODE END 4 */
}

/**
  * @brief  Get status routine.
  * @param  Add: Address to be read from.
  * @param  Cmd: Number of data to be read (in bytes).
  * @param  buffer: used for returning the time necessary for a program or an erase operation
  * @retval 0 if operation is successful
  */
static uint16_t MEM_If_GetStatus_HS(uint32_t Add, uint8_t Cmd, uint8_t *buffer)
{
	/* USER CODE BEGIN 11 */
	spitarget_t target = targetdataflash;	/* addressing to chip */
	uint_fast8_t st = dataflash_read_status(target);

	const unsigned FLASH_PROGRAM_TIME = (st & 0x01) ? 5 : 0;
	const unsigned FLASH_ERASE_TIME = (st & 0x01) ? 5 : 0;
	switch(Cmd)
	{
	case DFU_MEDIA_PROGRAM:
		USBD_poke_u24(& buffer[1], FLASH_PROGRAM_TIME);
		break;

	case DFU_MEDIA_ERASE:
	default:
		USBD_poke_u24(& buffer[1], FLASH_ERASE_TIME);
		break;
	}
	return  (USBD_OK);
	/* USER CODE END 11 */
}

static USBALIGN_BEGIN USBD_DFU_MediaTypeDef USBD_DFU_fops_HS USBALIGN_END =
{
    "dddd",
    MEM_If_Init_HS,
    MEM_If_DeInit_HS,
    MEM_If_Erase_HS,
    MEM_If_Write_HS,
    MEM_If_Read_HS,
    MEM_If_GetStatus_HS
};


/**
  * @brief  USBD_DFU_Init
  *         Initialize the DFU interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static USBD_StatusTypeDef  USBD_DFU_Init (USBD_HandleTypeDef *pdev,
                               uint_fast8_t cfgidx)
{
  USBD_DFU_HandleTypeDef   *hdfu;

 /* Allocate Audio structure */
  //pdev->pClassData = USBD_malloc(sizeof (USBD_DFU_HandleTypeDef));

  //if(pdev->pClassData == NULL)
  //{
  //  return USBD_FAIL;
  //}
  //else
  {
    //hdfu = (USBD_DFU_HandleTypeDef*) pdev->pClassData;
    hdfu = & gdfu;

    hdfu->alt_setting = 0;
    hdfu->data_ptr = 0;//USBD_DFU_APP_DEFAULT_ADD;
    hdfu->wblock_num = 0;
    hdfu->wlength = 0;

    hdfu->manif_state = DFU_MANIFEST_COMPLETE;
    hdfu->dev_state = DFU_STATE_IDLE;

    hdfu->dev_status[0] = DFU_ERROR_NONE;
    hdfu->dev_status[1] = 0;
    hdfu->dev_status[2] = 0;
    hdfu->dev_status[3] = 0;
    hdfu->dev_status[4] = DFU_STATE_IDLE;
    hdfu->dev_status[5] = 0;

    /* Initialize Hardware layer */
    if (USBD_DFU_fops_HS.Init() != USBD_OK)
    {
      return USBD_FAIL;
    }
  }
  return USBD_OK;
}


/**
  * @brief  USBD_DFU_DeInit
  *         De-Initialize the DFU layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static USBD_StatusTypeDef  USBD_DFU_DeInit (USBD_HandleTypeDef *pdev,
                                 uint_fast8_t cfgidx)
{
	//PRINTF(PSTR("USBD_DFU_DeInit\n"));
  USBD_DFU_HandleTypeDef   *hdfu;
    //hdfu = (USBD_DFU_HandleTypeDef*) pdev->pClassData;
    hdfu = & gdfu;

  hdfu->wblock_num = 0;
  hdfu->wlength = 0;

  hdfu->dev_state = DFU_STATE_IDLE;
  hdfu->dev_status[0] = DFU_ERROR_NONE;
  hdfu->dev_status[4] = DFU_STATE_IDLE;

  /* DeInit  physical Interface components */
  //if(pdev->pClassData != NULL)
  {
    /* De-Initialize Hardware layer */
    (USBD_DFU_fops_HS.DeInit());
    //USBD_free(pdev->pClassData);
    //pdev->pClassData = NULL;
  }

  return USBD_OK;
}

/**
  * @brief  USBD_DFU_EP0_TxSent
  *         handle EP0 TRx Ready event
  * @param  pdev: device instance
  * @retval status
  */
// call from USBD_LL_DataInStage after end of send data trough EP0
static USBD_StatusTypeDef  USBD_DFU_EP0_TxSent (USBD_HandleTypeDef *pdev)
{
 uint32_t addr;
 USBD_SetupReqTypedef     req;
 USBD_DFU_HandleTypeDef   *hdfu;

    //hdfu = (USBD_DFU_HandleTypeDef*) pdev->pClassData;
    hdfu = & gdfu;

	//PRINTF(PSTR("USBD_DFU_EP0_TxSent\n"));

  if (hdfu->dev_state == DFU_STATE_DNLOAD_BUSY)
  {
	 /* Decode the Special Command*/
    if (hdfu->wblock_num == 0)
    {
      if ((hdfu->buffer.d8[0] ==  DFU_CMD_GETCOMMANDS) && (hdfu->wlength == 1))
      {

      }
      else if  (( hdfu->buffer.d8[0] ==  DFU_CMD_SETADDRESSPOINTER ) && (hdfu->wlength == 5))
      {
        hdfu->data_ptr  = hdfu->buffer.d8[1];
        hdfu->data_ptr += hdfu->buffer.d8[2] << 8;
        hdfu->data_ptr += hdfu->buffer.d8[3] << 16;
        hdfu->data_ptr += hdfu->buffer.d8[4] << 24;
      }
      else if (( hdfu->buffer.d8[0] ==  DFU_CMD_ERASE ) && (hdfu->wlength == 5))
      {
        hdfu->data_ptr  = hdfu->buffer.d8[1];
        hdfu->data_ptr += hdfu->buffer.d8[2] << 8;
        hdfu->data_ptr += hdfu->buffer.d8[3] << 16;
        hdfu->data_ptr += hdfu->buffer.d8[4] << 24;

        if (USBD_DFU_fops_HS.Erase(hdfu->data_ptr) != USBD_OK)
        {
          return USBD_FAIL;
        }
      }
      else
      {
        /* Reset the global length and block number */
        hdfu->wlength = 0;
        hdfu->wblock_num = 0;
        /* Call the error management function (command will be nacked) */
        req.bmRequest = 0;
        req.wLength = 1;
        USBD_CtlError (pdev, &req);
      }
    }
    /* Regular Download Command */
    else if (hdfu->wblock_num > 1)
    {
      /* Decode the required address */
      addr = ((hdfu->wblock_num - 2) * USBD_DFU_XFER_SIZE) + hdfu->data_ptr;

      /* Preform the write operation */
      if (USBD_DFU_fops_HS.Write(hdfu->buffer.d8, addr, hdfu->wlength) != USBD_OK)
      {
        return USBD_FAIL;
      }
    }
    /* Reset the global length and block number */
    hdfu->wlength = 0;
    hdfu->wblock_num = 0;

    /* Update the state machine */
    hdfu->dev_state = DFU_STATE_DNLOAD_SYNC;

    hdfu->dev_status[1] = 0;
    hdfu->dev_status[2] = 0;
    hdfu->dev_status[3] = 0;
    hdfu->dev_status[4] = hdfu->dev_state;
    return USBD_OK;
  }
  else if (hdfu->dev_state == DFU_STATE_MANIFEST)/* Manifestation in progress*/
  {
    /* Start leaving DFU mode */
    DFU_Leave(pdev);
  }

  return USBD_OK;
}

/**
  * @brief  USBD_DFU_Setup
  *         Handle the DFU specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static USBD_StatusTypeDef  USBD_DFU_Setup (USBD_HandleTypeDef *pdev,
                                const USBD_SetupReqTypedef *req)
{
  uint8_t *pbuf = 0;
  uint16_t len = 0;
  uint8_t ret = USBD_OK;
  //USBD_DFU_HandleTypeDef   *hdfu;

    //hdfu = (USBD_DFU_HandleTypeDef*) pdev->pClassData;
    //hdfu = & gdfu;
	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);
	if (interfacev != INTERFACE_DFU_CONTROL)
		  return USBD_OK;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS :
    switch (req->bRequest)
    {
    case DFU_DNLOAD:
      DFU_Download(pdev, req);
      break;

    case DFU_UPLOAD:
				TP();
      DFU_Upload(pdev, req);
      break;

    case DFU_GETSTATUS:
				TP();
      DFU_GetStatus(pdev);
      break;

    case DFU_CLRSTATUS:
      DFU_ClearStatus(pdev);
      break;

    case DFU_GETSTATE:
      DFU_GetState(pdev);
      break;

    case DFU_ABORT:
				TP();
      DFU_Abort(pdev);
      break;

    case DFU_DETACH:
				TP();
      DFU_Detach(pdev, req);
      break;


    default:
      USBD_CtlError (pdev, req);
      ret = USBD_FAIL;
    }
    break;
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
#if 0
    case USB_REQ_GET_DESCRIPTOR:
      if( (req->wValue >> 8) == DFU_DESCRIPTOR_TYPE)
      {
        pbuf = USBD_DFU_CfgDesc + (9 * (USBD_DFU_MAX_ITF_NUM + 1));
        len = MIN(USB_DFU_DESC_SIZ , req->wLength);
      }

      USBD_CtlSendData (pdev,
                        pbuf,
                        len);
      break;

#endif
#if 0
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev,
                        (uint8_t *)&hdfu->alt_setting,
                        1);
      break;

    case USB_REQ_SET_INTERFACE :
      if (1) //((uint8_t)(req->wValue) < USBD_DFU_MAX_ITF_NUM)
      {
        hdfu->alt_setting = (uint8_t)(req->wValue);
      }
      else
      {
        /* Call the error management function (command will be nacked */
        USBD_CtlError (pdev, req);
        ret = USBD_FAIL;
      }
      break;
#endif
    default:
      //USBD_CtlError (pdev, req);
      //ret = USBD_FAIL;
       break;
   }
  }
  return ret;
}

/******************************************************************************
     DFU Class requests management
******************************************************************************/
/**
  * @brief  DFU_Detach
  *         Handles the DFU DETACH request.
  * @param  pdev: device instance
  * @param  req: pointer to the request structure.
  * @retval None.
  */
static void DFU_Detach(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	PRINTF(PSTR("DFU_Detach interfacev=%u\n"));
	USBD_DFU_HandleTypeDef   *hdfu;

	//hdfu = (USBD_DFU_HandleTypeDef*) pdev->pClassData;
	hdfu = & gdfu;

  if (hdfu->dev_state == DFU_STATE_IDLE || hdfu->dev_state == DFU_STATE_DNLOAD_SYNC
      || hdfu->dev_state == DFU_STATE_DNLOAD_IDLE || hdfu->dev_state == DFU_STATE_MANIFEST_SYNC
        || hdfu->dev_state == DFU_STATE_UPLOAD_IDLE )
  {
    /* Update the state machine */
    hdfu->dev_state = DFU_STATE_IDLE;
    hdfu->dev_status[0] = DFU_ERROR_NONE;
    hdfu->dev_status[1] = 0;
    hdfu->dev_status[2] = 0;
    hdfu->dev_status[3] = 0; /*bwPollTimeout=0ms*/
    hdfu->dev_status[4] = hdfu->dev_state;
    hdfu->dev_status[5] = 0; /*iString*/
    hdfu->wblock_num = 0;
    hdfu->wlength = 0;
  }

  /* Check the detach capability in the DFU functional descriptor */
  if (1) //((USBD_DFU_CfgDesc[12 + (9 * USBD_DFU_MAX_ITF_NUM)]) & DFU_DETACH_MASK)
  {
#if WITHISBOOTLOADER
    /* Perform an Attach-Detach operation on USB bus */
    USBD_Stop (pdev);
	bootloader_detach();
    USBD_Start (pdev);
#endif /* WITHISBOOTLOADER */
  }
  else
  {
    /* Wait for the period of time specified in Detach request */
    //USBD_Delay (req->wValue);
    //local_delay_ms(req->wValue);
  }
}

/**
  * @brief  DFU_Download
  *         Handles the DFU DNLOAD request.
  * @param  pdev: device instance
  * @param  req: pointer to the request structure
  * @retval None
  */
static void DFU_Download(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
	//PRINTF(PSTR("DFU_Download, req->wLength=%u\n"), req->wLength);
	USBD_DFU_HandleTypeDef   *hdfu;

    //hdfu = (USBD_DFU_HandleTypeDef*) pdev->pClassData;
    hdfu = & gdfu;

  /* Data setup request */
  if (req->wLength > 0)
  {
    if ((hdfu->dev_state == DFU_STATE_IDLE) || (hdfu->dev_state == DFU_STATE_DNLOAD_IDLE))
    {
      /* Update the global length and block number */
      hdfu->wblock_num = req->wValue;
      hdfu->wlength = req->wLength;

      /* Update the state machine */
      hdfu->dev_state = DFU_STATE_DNLOAD_SYNC;
      hdfu->dev_status[4] = hdfu->dev_state;

      /* Prepare the reception of the buffer over EP0 */
      USBD_CtlPrepareRx (pdev,
                         (uint8_t*)hdfu->buffer.d8,
                         hdfu->wlength);
    }
    /* Unsupported state */
    else
    {
      /* Call the error management function (command will be nacked */
      USBD_CtlError (pdev, req);
    }
  }
  /* 0 Data DNLOAD request */
  else
  {
    /* End of DNLOAD operation*/
    if (hdfu->dev_state == DFU_STATE_DNLOAD_IDLE || hdfu->dev_state == DFU_STATE_IDLE )
    {
      hdfu->manif_state = DFU_MANIFEST_IN_PROGRESS;
      hdfu->dev_state = DFU_STATE_MANIFEST_SYNC;
      hdfu->dev_status[1] = 0;
      hdfu->dev_status[2] = 0;
      hdfu->dev_status[3] = 0;
      hdfu->dev_status[4] = hdfu->dev_state;
    }
    else
    {
      /* Call the error management function (command will be nacked */
      USBD_CtlError (pdev, req);
    }
  }
}

/**
  * @brief  DFU_Upload - from device to host
  *         Handles the DFU UPLOAD request.
  * @param  pdev: instance
  * @param  req: pointer to the request structure
  * @retval status
  */
static void DFU_Upload(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
 USBD_DFU_HandleTypeDef   *hdfu;

    //hdfu = (USBD_DFU_HandleTypeDef*) pdev->pClassData;
    hdfu = & gdfu;

  uint8_t *phaddr = NULL;
  uint32_t addr = 0;

  /* Data setup request */
  if (req->wLength > 0)
  {
    if ((hdfu->dev_state == DFU_STATE_IDLE) || (hdfu->dev_state == DFU_STATE_UPLOAD_IDLE))
    {
      /* Update the global length and block number */
      hdfu->wblock_num = req->wValue;
      hdfu->wlength = req->wLength;

      /* DFU Get Command */
      if (hdfu->wblock_num == 0)
      {
        /* Update the state machine */
        hdfu->dev_state = (hdfu->wlength > 3)? DFU_STATE_IDLE : DFU_STATE_UPLOAD_IDLE;

        hdfu->dev_status[1] = 0;
        hdfu->dev_status[2] = 0;
        hdfu->dev_status[3] = 0;
        hdfu->dev_status[4] = hdfu->dev_state;

        /* Store the values of all supported commands */
        hdfu->buffer.d8[0] = DFU_CMD_GETCOMMANDS;
        hdfu->buffer.d8[1] = DFU_CMD_SETADDRESSPOINTER;
        hdfu->buffer.d8[2] = DFU_CMD_ERASE;

        /* Send the status data over EP0 */
        USBD_CtlSendData (pdev,
                          hdfu->buffer.d8,
                          3);
      }
      else if (hdfu->wblock_num > 1)
      {
        hdfu->dev_state = DFU_STATE_UPLOAD_IDLE ;

        hdfu->dev_status[1] = 0;
        hdfu->dev_status[2] = 0;
        hdfu->dev_status[3] = 0;
        hdfu->dev_status[4] = hdfu->dev_state;

        addr = ((hdfu->wblock_num - 2) * USBD_DFU_XFER_SIZE) + hdfu->data_ptr;  /* Change is Accelerated*/

        /* Return the physical address where data are stored */
        phaddr = USBD_DFU_fops_HS.Read(addr, hdfu->buffer.d8, hdfu->wlength);

        /* Send the status data over EP0 */
        USBD_CtlSendData (pdev,
                          phaddr,
                          hdfu->wlength);
      }
      else  /* unsupported hdfu->wblock_num */
      {
		TP();
        hdfu->dev_state = DFU_ERROR_STALLEDPKT;

        hdfu->dev_status[1] = 0;
        hdfu->dev_status[2] = 0;
        hdfu->dev_status[3] = 0;
        hdfu->dev_status[4] = hdfu->dev_state;

        /* Call the error management function (command will be nacked */
        USBD_CtlError (pdev, req);
      }
    }
    /* Unsupported state */
    else
    {
		TP();
      hdfu->wlength = 0;
      hdfu->wblock_num = 0;
      /* Call the error management function (command will be nacked */
      USBD_CtlError (pdev, req);
    }
  }
  /* No Data setup request */
  else
  {
    hdfu->dev_state = DFU_STATE_IDLE;

    hdfu->dev_status[1] = 0;
    hdfu->dev_status[2] = 0;
    hdfu->dev_status[3] = 0;
    hdfu->dev_status[4] = hdfu->dev_state;
  }
}

/**
  * @brief  DFU_GetStatus
  *         Handles the DFU GETSTATUS request.
  * @param  pdev: instance
  * @retval status
  */
static void DFU_GetStatus(USBD_HandleTypeDef *pdev)
{
	//PRINTF(PSTR("DFU_GetStatus\n"));
	USBD_DFU_HandleTypeDef   *hdfu;

    //hdfu = (USBD_DFU_HandleTypeDef*) pdev->pClassData;
    hdfu = & gdfu;

  switch (hdfu->dev_state)
  {
  case   DFU_STATE_DNLOAD_SYNC:
    if (hdfu->wlength != 0)
    {
      hdfu->dev_state = DFU_STATE_DNLOAD_BUSY;

      hdfu->dev_status[1] = 0;
      hdfu->dev_status[2] = 0;
      hdfu->dev_status[3] = 0;
      hdfu->dev_status[4] = hdfu->dev_state;

      if ((hdfu->wblock_num == 0) && (hdfu->buffer.d8[0] == DFU_CMD_ERASE))
      {
        USBD_DFU_fops_HS.GetStatus(hdfu->data_ptr, DFU_MEDIA_ERASE, hdfu->dev_status);
      }
      else
      {
        USBD_DFU_fops_HS.GetStatus(hdfu->data_ptr, DFU_MEDIA_PROGRAM, hdfu->dev_status);
      }
    }
    else  /* (hdfu->wlength==0)*/
    {
      hdfu->dev_state = DFU_STATE_DNLOAD_IDLE;

      hdfu->dev_status[1] = 0;
      hdfu->dev_status[2] = 0;
      hdfu->dev_status[3] = 0;
      hdfu->dev_status[4] = hdfu->dev_state;
		// здесь строка предотвращет зависания звука при программировании на стирании страницы
	  USBD_DFU_fops_HS.GetStatus(hdfu->data_ptr, DFU_MEDIA_ERASE, hdfu->dev_status);
    }
    break;

  case   DFU_STATE_MANIFEST_SYNC :
    if (hdfu->manif_state == DFU_MANIFEST_IN_PROGRESS)
    {
      hdfu->dev_state = DFU_STATE_MANIFEST;

	  USBD_poke_u24(& hdfu->dev_status [1], 1);	/*bwPollTimeout = 1ms*/
      hdfu->dev_status[4] = hdfu->dev_state;
    }
    else if ((hdfu->manif_state == DFU_MANIFEST_COMPLETE) &&
      (1) //((USBD_DFU_CfgDesc[(11 + (9 * USBD_DFU_MAX_ITF_NUM))]) & 0x04)
    )
    {
      hdfu->dev_state = DFU_STATE_IDLE;

      hdfu->dev_status[1] = 0;
      hdfu->dev_status[2] = 0;
      hdfu->dev_status[3] = 0;
      hdfu->dev_status[4] = hdfu->dev_state;
	  // не меняет протестиованного поведения
	//USBD_DFU_fops_HS.GetStatus(hdfu->data_ptr, DFU_MEDIA_ERASE, hdfu->dev_status);
    }
    break;

  default :
	  //TP();
	USBD_DFU_fops_HS.GetStatus(hdfu->data_ptr, DFU_MEDIA_ERASE, hdfu->dev_status);
    break;
  }

  /* Send the status data over EP0 */
  USBD_CtlSendData (pdev,
                    hdfu->dev_status,
                    DFU_STATUS_DEPTH);
}

/**
  * @brief  DFU_ClearStatus
  *         Handles the DFU CLRSTATUS request.
  * @param  pdev: device instance
  * @retval status
  */
static void DFU_ClearStatus(USBD_HandleTypeDef *pdev)
{
	PRINTF(PSTR("DFU_ClearStatus\n"));
USBD_DFU_HandleTypeDef   *hdfu;

    //hdfu = (USBD_DFU_HandleTypeDef*) pdev->pClassData;
    hdfu = & gdfu;

  if (hdfu->dev_state == DFU_STATE_ERROR)
  {
    hdfu->dev_state = DFU_STATE_IDLE;
    hdfu->dev_status[0] = DFU_ERROR_NONE;/*bStatus*/
    hdfu->dev_status[1] = 0;
    hdfu->dev_status[2] = 0;
    hdfu->dev_status[3] = 0; /*bwPollTimeout=0ms*/
    hdfu->dev_status[4] = hdfu->dev_state;/*bState*/
    hdfu->dev_status[5] = 0;/*iString*/
  }
  else
  {   /*State Error*/
    hdfu->dev_state = DFU_STATE_ERROR;
    hdfu->dev_status[0] = DFU_ERROR_UNKNOWN;/*bStatus*/
    hdfu->dev_status[1] = 0;
    hdfu->dev_status[2] = 0;
    hdfu->dev_status[3] = 0; /*bwPollTimeout=0ms*/
    hdfu->dev_status[4] = hdfu->dev_state;/*bState*/
    hdfu->dev_status[5] = 0;/*iString*/
  }
}

/**
  * @brief  DFU_GetState
  *         Handles the DFU GETSTATE request.
  * @param  pdev: device instance
  * @retval None
  */
static void DFU_GetState(USBD_HandleTypeDef *pdev)
{
	PRINTF(PSTR("DFU_GetState\n"));
 USBD_DFU_HandleTypeDef   *hdfu;

    //hdfu = (USBD_DFU_HandleTypeDef*) pdev->pClassData;
    hdfu = & gdfu;

  /* Return the current state of the DFU interface */
  USBD_CtlSendData (pdev,
	                    &hdfu->dev_state,
                    1);
}

/**
  * @brief  DFU_Abort
  *         Handles the DFU ABORT request.
  * @param  pdev: device instance
  * @retval None
  */
static void DFU_Abort(USBD_HandleTypeDef *pdev)
{
	//PRINTF(PSTR("DFU_Abort\n"));
 USBD_DFU_HandleTypeDef   *hdfu;

    //hdfu = (USBD_DFU_HandleTypeDef*) pdev->pClassData;
    hdfu = & gdfu;

  if (hdfu->dev_state == DFU_STATE_IDLE || hdfu->dev_state == DFU_STATE_DNLOAD_SYNC
      || hdfu->dev_state == DFU_STATE_DNLOAD_IDLE || hdfu->dev_state == DFU_STATE_MANIFEST_SYNC
        || hdfu->dev_state == DFU_STATE_UPLOAD_IDLE )
  {
    hdfu->dev_state = DFU_STATE_IDLE;
    hdfu->dev_status[0] = DFU_ERROR_NONE;
    hdfu->dev_status[1] = 0;
    hdfu->dev_status[2] = 0;
    hdfu->dev_status[3] = 0; /*bwPollTimeout=0ms*/
    hdfu->dev_status[4] = hdfu->dev_state;
    hdfu->dev_status[5] = 0; /*iString*/
    hdfu->wblock_num = 0;
    hdfu->wlength = 0;
  }
}

/**
  * @brief  DFU_Leave
  *         Handles the sub-protocol DFU leave DFU mode request (leaves DFU mode
  *         and resets device to jump to user loaded code).
  * @param  pdev: device instance
  * @retval None
  */
static void DFU_Leave(USBD_HandleTypeDef *pdev)
{
	PRINTF(PSTR("DFU_Leave\n"));
 USBD_DFU_HandleTypeDef   *hdfu;

    //hdfu = (USBD_DFU_HandleTypeDef*) pdev->pClassData;
    hdfu = & gdfu;

 hdfu->manif_state = DFU_MANIFEST_COMPLETE;

  if (1) //((USBD_DFU_CfgDesc[(11 + (9 * USBD_DFU_MAX_ITF_NUM))]) & 0x04)
  {
    hdfu->dev_state = DFU_STATE_MANIFEST_SYNC;

    hdfu->dev_status[1] = 0;
    hdfu->dev_status[2] = 0;
    hdfu->dev_status[3] = 0;
    hdfu->dev_status[4] = hdfu->dev_state;
    return;
  }
  else
  {

    hdfu->dev_state = DFU_STATE_MANIFEST_WAIT_RESET;

    hdfu->dev_status[1] = 0;
    hdfu->dev_status[2] = 0;
    hdfu->dev_status[3] = 0;
    hdfu->dev_status[4] = hdfu->dev_state;

    /* Disconnect the USB device */
    USBD_Stop (pdev);

    /* DeInitilialize the MAL(Media Access Layer) */
    USBD_DFU_fops_HS.DeInit();

    /* Generate system reset to allow jumping to the user code */
    //NVIC_SystemReset();

    /* This instruction will not be reached (system reset) */
    for(;;);
  }
}

static void
USBD_DFU_ColdInit(void)
{
    /* Initialize Hardware layer */
    if (USBD_DFU_fops_HS.Init() != USBD_OK)
    {
		PRINTF(PSTR("USBD_DFU_ColdInit: data flash initialization failure\n"));
    }
}


static USBD_StatusTypeDef USBD_DFU_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
	const USBD_SetupReqTypedef * const req = & pdev->request;

	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	//PRINTF(PSTR("1 USBD_XXX_EP0_RxReady: interfacev=%u: bRequest=%u, wLength=%u\n"), interfacev, req->bRequest, req->wLength);
	switch (interfacev)
	{

	case INTERFACE_DFU_CONTROL:
		{
			switch (req->bRequest)
			{
			case DFU_DNLOAD:
				//TP();
				DFU_Download(pdev, req);	// used then upload/download command issued
				break;

			case DFU_UPLOAD:
				TP();
				DFU_Upload(pdev, req);	// never called
				break;

			case DFU_GETSTATUS:
				TP();
				DFU_GetStatus(pdev);// never called
				break;

			case DFU_CLRSTATUS:
				DFU_ClearStatus(pdev);
				break;

			case DFU_GETSTATE:
				TP();
				DFU_GetState(pdev);
				break;

			case DFU_ABORT:
				TP();
				DFU_Abort(pdev);	// not called
				break;

			case DFU_DETACH:
				TP();
				DFU_Detach(pdev, req);
				break;
			default:
				USBD_CtlError (pdev, req);
				//ret = USBD_FAIL;
				break;
			}
		}
		break;

	}
	return USBD_OK;
}

USBD_ClassTypeDef  USBD_CLASS_DFU =
{
	USBD_DFU_ColdInit,
	USBD_DFU_Init,
	USBD_DFU_DeInit,
	USBD_DFU_Setup,
	USBD_DFU_EP0_TxSent,			// call from USBD_LL_DataInStage after end of send data trough EP0
	USBD_DFU_EP0_RxReady,	// call from USBD_LL_DataOutStage after end of receieve data trough EP0
	NULL, //USBD_DFU_DataIn,
	NULL, //USBD_DFU_DataOut,
	NULL, //USBD_DFU_SOF,
	NULL, //USBD_DFU_IsoINIncomplete,
	NULL, //USBD_DFU_IsoOutIncomplete,
};


#endif /* WITHUSBHW && WITHUSBDFU */
