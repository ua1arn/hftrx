#include "hardware.h"

#if CPUSTYLE_ALLWINNER && WITHSDHCHW

#include "formats.h"

/*
 * driver/sd/sdcard.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "sdcard.h"

#define UNSTUFF_BITS(resp, start, size)								\
	({																\
		const int __size = size;									\
		const uint32_t __mask = (__size < 32 ? 1 << __size : 0) - 1;	\
		const int __off = 3 - ((start) / 32);						\
		const int __shft = (start) & 31;							\
		uint32_t __res;												\
																	\
		__res = resp[__off] >> __shft;								\
		if(__size + __shft > 32)									\
			__res |= resp[__off-1] << ((32 - __shft) % 32);			\
		__res & __mask;												\
	})

static const unsigned tran_speed_unit[] = {
	[0] = 10000,
	[1] = 100000,
	[2] = 1000000,
	[3] = 10000000,
};

static const unsigned char tran_speed_time[] = {
	 0, 10, 12, 13, 15, 20, 25, 30,
	35, 40, 45, 50, 55, 60, 70, 80,
};

static char * sdcard_version_string(struct sdcard_t * card)
{
	static char version[sizeof("xx.xxx")];
	unsigned int major, minor, micro;
	int n;

	major = (card->version >> 8) & 0xf;
	minor = (card->version >> 4) & 0xf;
	micro = card->version & 0xf;
	n = local_snprintf_P(version, ARRAY_SIZE(version), "%u.%u", major, minor);
	if(micro)
		local_snprintf_P(version + n, ARRAY_SIZE(version) - n, "%u", micro);
	return version;
}

static unsigned int extract_mid(struct sdcard_t * card)
{
	if((card->version & MMC_VERSION_MMC) && (card->version <= MMC_VERSION_1_4))
		return UNSTUFF_BITS(card->cid, 104, 24);
	else
		return UNSTUFF_BITS(card->cid, 120, 8);
}

static unsigned int extract_oid(struct sdcard_t * card)
{
	return (card->cid[0] >> 8) & 0xffff;
}

static unsigned int extract_prv(struct sdcard_t * card)
{
	return (card->cid[2] >> 24);
}

static unsigned int extract_psn(struct sdcard_t * card)
{
	if(card->version & SD_VERSION_SD)
	{
		return UNSTUFF_BITS(card->csd, 24, 32);
	}
	else
	{
		if(card->version > MMC_VERSION_1_4)
			return UNSTUFF_BITS(card->cid, 16, 32);
		else
			return UNSTUFF_BITS(card->cid, 16, 24);
	}
}

static unsigned int extract_month(struct sdcard_t * card)
{
	if(card->version & SD_VERSION_SD)
		return UNSTUFF_BITS(card->cid, 8, 4);
	else
		return UNSTUFF_BITS(card->cid, 12, 4);
}

static unsigned int extract_year(struct sdcard_t * card)
{
	unsigned int year;

	if(card->version & SD_VERSION_SD)
		year = UNSTUFF_BITS(card->cid, 12, 8) + 2000;
	else if(card->version < MMC_VERSION_4_41)
		return UNSTUFF_BITS(card->cid, 8, 4) + 1997;
	else
	{
		year = UNSTUFF_BITS(card->cid, 8, 4) + 1997;
		if(year < 2010)
			year += 16;
	}
	return year;
}

static int go_idle_state(struct sdhci_t * hci)
{
	struct sdhci_cmd_t cmd = { 0 }; //������������

	cmd.cmdidx = MMC_GO_IDLE_STATE;
	cmd.cmdarg = 0;
	cmd.resptype = hci->isspi ? MMC_RSP_R1 : MMC_RSP_NONE;

	if(sdhci_t113_transfer(hci, &cmd, NULL))
		return 1;
	return sdhci_t113_transfer(hci, &cmd, NULL);
}

static int sd_send_if_cond(struct sdhci_t * hci, struct sdcard_t * card)
{
	struct sdhci_cmd_t cmd = { 0 };

	cmd.cmdidx = SD_CMD_SEND_IF_COND;
	if(hci->voltage & MMC_VDD_27_36)
		cmd.cmdarg = (0x1 << 8);
	else if(hci->voltage & MMC_VDD_165_195)
		cmd.cmdarg = (0x2 << 8);
	else
		cmd.cmdarg = (0x0 << 8);
	cmd.cmdarg |= 0xaa;
	cmd.resptype = MMC_RSP_R7;
 	if(!sdhci_t113_transfer(hci, &cmd, NULL))
 		return 0;

	if((cmd.response[0] & 0xff) != 0xaa)
		return 0;
	card->version = SD_VERSION_2;
	return 1;
}

static int sd_send_op_cond(struct sdhci_t * hci, struct sdcard_t * card)
{
	struct sdhci_cmd_t cmd = { 0 };
	int retries = 100;

	do {
		cmd.cmdidx = MMC_APP_CMD;
		cmd.cmdarg = 0;
		cmd.resptype = MMC_RSP_R1;

		if(!sdhci_t113_transfer(hci, &cmd, NULL))
                {
			PRINTF("sd_send_op_cond: sdhci_t113_transfer failure\n");
	 		continue;
                }

		cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;
		if(hci->isspi)
		{
			cmd.cmdarg = 0;
			if(card->version == SD_VERSION_2)
				cmd.cmdarg |= OCR_HCS;
			cmd.resptype = MMC_RSP_R1;
			if(sdhci_t113_transfer(hci, &cmd, NULL))
				break;
		}
		else
		{
			if(hci->voltage & MMC_VDD_27_36)
				cmd.cmdarg = 0x00ff8000;
			else if(hci->voltage & MMC_VDD_165_195)
				cmd.cmdarg = 0x00000080;
			else
				cmd.cmdarg = 0;

			if(card->version == SD_VERSION_2)
				cmd.cmdarg |= OCR_HCS;
			cmd.resptype = MMC_RSP_R3;
			if(!sdhci_t113_transfer(hci, &cmd, NULL))
			{
				//PRINTF("1 wait not okay\n");
				return 0;
			}
			if((cmd.response[0] & OCR_BUSY))
			{
				break;
			}
		}
	} while(retries--);

	if(retries <= 0)
		return 0;

	if(card->version != SD_VERSION_2)
		card->version = SD_VERSION_1_0;
	if(hci->isspi)
	{
		cmd.cmdidx = MMC_SPI_READ_OCR;
		cmd.cmdarg = 0;
		cmd.resptype = MMC_RSP_R3;
		if(!sdhci_t113_transfer(hci, &cmd, NULL))
			return 0;
	}
	card->ocr = cmd.response[0];
	card->high_capacity = ((card->ocr & OCR_HCS) == OCR_HCS);
	card->rca = 0;

	return 1;
}

static int mmc_send_op_cond(struct sdhci_t * hci, struct sdcard_t * card)
{
	struct sdhci_cmd_t cmd = { 0 };
	int retries = 100;

	if (!go_idle_state(hci))
	{
		TP();
		return 0;
	}

	cmd.cmdidx = MMC_SEND_OP_COND;
	cmd.cmdarg = 0;
	cmd.resptype = MMC_RSP_R3;
 	if(!sdhci_t113_transfer(hci, &cmd, NULL))
	{
		TP();
		return 0;
	}

	do {
		cmd.cmdidx = MMC_SEND_OP_COND;
		cmd.cmdarg = hci->isspi ? 0 : (card->ocr & OCR_VOLTAGE_MASK) | (card->ocr & OCR_ACCESS_MODE);
		cmd.cmdarg |= OCR_HCS;
		cmd.resptype = MMC_RSP_R3;
	 	if(!sdhci_t113_transfer(hci, &cmd, NULL))
		{
			PRINTF("sdhci_t113_transfer error\n");
			return 0;
		}
	} while (!(cmd.response[0] & OCR_BUSY) && retries--);

	if(retries <= 0)
	{
		PRINTF("mmc_send_op_cond: no valid response\n");
		return 0;
	}

	if(hci->isspi)
	{
		cmd.cmdidx = MMC_SPI_READ_OCR;
		cmd.cmdarg = 0;
		cmd.resptype = MMC_RSP_R3;
		if(!sdhci_t113_transfer(hci, &cmd, NULL))
		{
			TP();
			return 0;
		}
	}
	card->version = MMC_VERSION_UNKNOWN;
	card->ocr = cmd.response[0];
	card->high_capacity = ((card->ocr & OCR_HCS) == OCR_HCS);
	card->rca = 0;
	return 1;
}

static int mmc_status(struct sdhci_t * hci, struct sdcard_t * card) //����������� �������� � ������� mmc_write_blocks, ����������� �������� (�������)
{
	struct sdhci_cmd_t cmd = { 0 };

	int retries = 100;

	cmd.cmdidx = MMC_SEND_STATUS;
	cmd.resptype = MMC_RSP_R1;
	cmd.cmdarg = card->rca << 16;

	do
	{
		if(!sdhci_t113_transfer(hci, &cmd, NULL))
                { 
			continue;
                }

		if(cmd.response[0] & (1 << 8))
                {
			break;
                }

		udelay(1);

	} while(retries-- > 0);

	if(retries > 0)
	{
		//PRINTF("1 status=%08X\n", cmd.response[0]);
		return ((cmd.response[0] >> 9) & 0xf);
	}

	return -1;
}

//------------------------------------------------------------------------------------

/*
https://github.com/Xilinx/u-boot-xlnx/blob/master/include/mmc.h
https://github.com/Xilinx/u-boot-xlnx/blob/master/drivers/mmc/mmc.c
https://github.com/Xilinx/u-boot-xlnx/blob/master/drivers/mmc/mmc_write.c
*/

#define MMC_STATUS_RDY_FOR_DATA (1 << 8)
#define MMC_STATUS_CURR_STATE	(0xf << 9)
#define MMC_STATE_PRG	        (7 << 9)
#define MMC_STATUS_MASK	        (~0x0206BF7F)

static int mmc_status2(struct sdhci_t * hci, struct sdcard_t * card)
{
	struct sdhci_cmd_t cmd = { 0 };

	cmd.cmdidx = MMC_SEND_STATUS;
	cmd.resptype = MMC_RSP_R1;
	cmd.cmdarg = card->rca << 16;

        unsigned int retries=4;
        int ret;

	do {
		ret = sdhci_t113_transfer(hci, &cmd, NULL);
	} while (!ret && retries--);

	if (ret)
	{
		//PRINTF("2 status=%08X\n", cmd.response[0]);
		return  cmd.response[0];
	}

	return -1;
}

int mmc_poll_for_busy(struct sdhci_t * hci, struct sdcard_t * card, int timeout_ms)
{
	int status;

	while (1) {
                status = mmc_status2(hci,card);

		if (status < 0)
                {
			return -1;
                }

		if ((status & MMC_STATUS_RDY_FOR_DATA) &&
		    (status & MMC_STATUS_CURR_STATE) !=
		     MMC_STATE_PRG)
			break;

		if (status & MMC_STATUS_MASK) {
			return -2;
		}

		if (timeout_ms-- <= 0)
			break;

		udelay(1000);
	}

	if (timeout_ms <= 0) {
		return -3;
	}

	return 0;
}

//------------------------------------------------------------------------------------

uint64_t mmc_read_blocks(uint8_t * buf, uint64_t start, uint64_t blkcnt)
{
	struct sdhci_t *hci=&HCI;
	struct sdcard_t *card=&CARD;

	struct sdhci_cmd_t cmd = { 0 };
	struct sdhci_data_t dat = { 0 };

	int status;

	if(blkcnt > 1)
		cmd.cmdidx = MMC_READ_MULTIPLE_BLOCK;
	else
		cmd.cmdidx = MMC_READ_SINGLE_BLOCK;

	if(card->high_capacity)
        {
		cmd.cmdarg = start;
        }
	else
        {
		cmd.cmdarg = start * card->read_bl_len;
        }

	cmd.resptype = MMC_RSP_R1;
	dat.buf = buf;
	dat.flag = MMC_DATA_READ;
	dat.blksz = card->read_bl_len;
	dat.blkcnt = blkcnt;

	if(!sdhci_t113_transfer(hci, &cmd, &dat))
        {
		return 0;
        }
/*
	if(!hci->isspi)
	{
		do {
			status = mmc_status(hci, card);

			if(status < 0)
                        {
				return 0;
                        }

		} while((status != MMC_STATUS_TRAN) && (status != MMC_STATUS_DATA));
	}
*/
	if(blkcnt > 1)
	{
		cmd.cmdidx = MMC_STOP_TRANSMISSION;
		cmd.cmdarg = 0;
		cmd.resptype = MMC_RSP_R1B;

		if(!sdhci_t113_transfer(hci, &cmd, NULL))
                {
			return 0;
                }
	}

	return blkcnt;
}

uint64_t mmc_write_blocks(uint8_t * buf, uint64_t start, uint64_t blkcnt)
{
	struct sdhci_t *hci=&HCI;
	struct sdcard_t *card=&CARD;

	struct sdhci_cmd_t cmd = { 0 };
	struct sdhci_data_t dat = { 0 };
	int status;

	if(blkcnt > 1)
		cmd.cmdidx = MMC_WRITE_MULTIPLE_BLOCK;
	else
		cmd.cmdidx = MMC_WRITE_SINGLE_BLOCK;

	if(card->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * card->write_bl_len;

	cmd.resptype = MMC_RSP_R1;
	dat.buf = buf;
	dat.flag = MMC_DATA_WRITE;
	dat.blksz = card->write_bl_len;
	dat.blkcnt = blkcnt;

	if(!sdhci_t113_transfer(hci, &cmd, &dat))
        {
		return 0;
        }
/*
	if(!hci->isspi)
	{
		do {
			status = mmc_status(hci, card);


			if(status < 0)
                        {
				return 0;
                        }

		} while((status != MMC_STATUS_TRAN) && (status != MMC_STATUS_RCV));
	}
*/
	if(blkcnt > 1)
	{
		cmd.cmdidx = MMC_STOP_TRANSMISSION;
		cmd.cmdarg = 0;
		cmd.resptype = MMC_RSP_R1B;

		if(!sdhci_t113_transfer(hci, &cmd, NULL))
                {
			return 0;
                }

	}

	/* Waiting for the ready status */
	if(mmc_poll_for_busy(hci,card,1000))return 0; //����-��� 1 �.

	return blkcnt;
}

int sdcard_init(void)
{
        struct sdhci_t  *hci =&HCI;
        struct sdcard_t *card=&CARD;

        if(!sdhci_t113_init(hci))
        {
        	PRINTF("No SD card\n");
        	return 0;
        }
      	PRINTF("SD card present:\n");

	struct sdhci_cmd_t cmd = { 0 };
	struct sdhci_data_t dat = { 0 };
	char scap[32];
	uint64_t csize, cmult;
	uint32_t unit, time;
	int width;
	int status;

	sdhci_t113_reset(hci);

	sdhci_t113_setclock(hci, 400 * 1000);

	sdhci_t113_setwidth(hci, MMC_BUS_WIDTH_1);

	if(!go_idle_state(hci))
	{
		PRINTF("bad go_idle_state\n");
		return 0;
	}

	if (!sd_send_if_cond(hci, card))
		PRINTF("bad sd_send_if_cond\n");

	if(!sd_send_op_cond(hci, card))
	{
		PRINTF("bad sd_send_op_cond\n");
		if(!mmc_send_op_cond(hci, card))
		{
			TP();
			return 0;
		}
	}

	if(hci->isspi)
	{
		cmd.cmdidx = MMC_SEND_CID;
		cmd.cmdarg = 0;
		cmd.resptype = MMC_RSP_R1;
		if(!sdhci_t113_transfer(hci, &cmd, NULL))
			return 0;
		card->cid[0] = cmd.response[0];
		card->cid[1] = cmd.response[1];
		card->cid[2] = cmd.response[2];
		card->cid[3] = cmd.response[3];

		cmd.cmdidx = MMC_SEND_CSD;
		cmd.cmdarg = 0;
		cmd.resptype = MMC_RSP_R1;
		if(!sdhci_t113_transfer(hci, &cmd, NULL))
			return 0;
		card->csd[0] = cmd.response[0];
		card->csd[1] = cmd.response[1];
		card->csd[2] = cmd.response[2];
		card->csd[3] = cmd.response[3];
	}
	else
	{
		cmd.cmdidx = MMC_ALL_SEND_CID;
		cmd.cmdarg = 0;
		cmd.resptype = MMC_RSP_R2;
		if(!sdhci_t113_transfer(hci, &cmd, NULL))
			return 0;
		card->cid[0] = cmd.response[0];
		card->cid[1] = cmd.response[1];
		card->cid[2] = cmd.response[2];
		card->cid[3] = cmd.response[3];

		cmd.cmdidx = SD_CMD_SEND_RELATIVE_ADDR;
		cmd.cmdarg = card->rca << 16;
		cmd.resptype = MMC_RSP_R6;
		if(!sdhci_t113_transfer(hci, &cmd, NULL))
			return 0;
		if(card->version & SD_VERSION_SD)
			card->rca = (cmd.response[0] >> 16) & 0xffff;

		cmd.cmdidx = MMC_SEND_CSD;
		cmd.cmdarg = card->rca << 16;
		cmd.resptype = MMC_RSP_R2;
		if(!sdhci_t113_transfer(hci, &cmd, NULL))
			return 0;
		card->csd[0] = cmd.response[0];
		card->csd[1] = cmd.response[1];
		card->csd[2] = cmd.response[2];
		card->csd[3] = cmd.response[3];

		cmd.cmdidx = MMC_SELECT_CARD;
		cmd.cmdarg = card->rca << 16;
		cmd.resptype = MMC_RSP_R1;
		if(!sdhci_t113_transfer(hci, &cmd, NULL))
			return 0;
		do {
			status = mmc_status(hci, card);
			if(status < 0)
				return 0;
		} while(status != MMC_STATUS_TRAN);
	}

	if(card->version == MMC_VERSION_UNKNOWN)
	{
		switch((card->csd[0] >> 26) & 0xf)
		{
		case 0:
			card->version = MMC_VERSION_1_2;
			break;
		case 1:
			card->version = MMC_VERSION_1_4;
			break;
		case 2:
			card->version = MMC_VERSION_2_2;
			break;
		case 3:
			card->version = MMC_VERSION_3;
			break;
		case 4:
			card->version = MMC_VERSION_4;
			break;
		default:
			card->version = MMC_VERSION_1_2;
			break;
		};
	}

	unit = tran_speed_unit[(card->csd[0] & 0x7)];
	time = tran_speed_time[((card->csd[0] >> 3) & 0xf)];
	card->tran_speed = time * unit;
	card->dsr_imp = UNSTUFF_BITS(card->csd, 76, 1);

	card->read_bl_len = 1 << UNSTUFF_BITS(card->csd, 80, 4);
	if(card->version & SD_VERSION_SD)
		card->write_bl_len = card->read_bl_len;
	else
		card->write_bl_len = 1 << ((card->csd[3] >> 22) & 0xf);
	if(card->read_bl_len > 512)
		card->read_bl_len = 512;
	if(card->write_bl_len > 512)
		card->write_bl_len = 512;

	if((card->version & MMC_VERSION_MMC) && (card->version >= MMC_VERSION_4))
	{
		cmd.cmdidx = MMC_SEND_EXT_CSD;
		cmd.cmdarg = 0;
		cmd.resptype = MMC_RSP_R1;
		dat.buf = card->extcsd;
		dat.flag = MMC_DATA_READ;
		dat.blksz = 512;
		dat.blkcnt = 1;
		if(!sdhci_t113_transfer(hci, &cmd, &dat))
			return 0;
		if(!hci->isspi)
		{
			do {
				status = mmc_status(hci, card);
				if(status < 0)
					return 0;
			} while(status != MMC_STATUS_TRAN);
		}
		switch(card->extcsd[192])
		{
		case 1:
			card->version = MMC_VERSION_4_1;
			break;
		case 2:
			card->version = MMC_VERSION_4_2;
			break;
		case 3:
			card->version = MMC_VERSION_4_3;
			break;
		case 5:
			card->version = MMC_VERSION_4_41;
			break;
		case 6:
			card->version = MMC_VERSION_4_5;
			break;
		case 7:
			card->version = MMC_VERSION_5_0;
			break;
		case 8:
			card->version = MMC_VERSION_5_1;
			break;
		default:
			break;
		}
	}

	if(card->high_capacity)
	{
		if(card->version & SD_VERSION_SD)
		{
			csize = UNSTUFF_BITS(card->csd, 48, 22);
			card->capacity = (1 + csize) << 10;
		}
		else
		{
			card->capacity = card->extcsd[212] << 0 | card->extcsd[212 + 1] << 8 | card->extcsd[212 + 2] << 16 | card->extcsd[212 + 3] << 24;
		}
	}
	else
	{
		cmult = UNSTUFF_BITS(card->csd, 47, 3);
		csize = UNSTUFF_BITS(card->csd, 62, 12);
		card->capacity = (csize + 1) << (cmult + 2);
	}
	card->capacity *= 1 << UNSTUFF_BITS(card->csd, 80, 4);

	if(hci->isspi)
	{
		sdhci_t113_setclock(hci, min(card->tran_speed, hci->clock));
		sdhci_t113_setwidth(hci, MMC_BUS_WIDTH_1);
	}
	else
	{
		if(card->version & SD_VERSION_SD)
		{
			if((hci->width & MMC_BUS_WIDTH_8) || (hci->width & MMC_BUS_WIDTH_4))
				width = 2;
			else
				width = 0;

			cmd.cmdidx = MMC_APP_CMD;
			cmd.cmdarg = card->rca << 16;
			cmd.resptype = MMC_RSP_R5;
			if(!sdhci_t113_transfer(hci, &cmd, NULL))
				return 0;

			cmd.cmdidx = SD_CMD_SWITCH_FUNC;
			cmd.cmdarg = width;
			cmd.resptype = MMC_RSP_R1;
			if(!sdhci_t113_transfer(hci, &cmd, NULL))
				return 0;

			sdhci_t113_setclock(hci, min(card->tran_speed, hci->clock));

			if((hci->width & MMC_BUS_WIDTH_8) || (hci->width & MMC_BUS_WIDTH_4))
				sdhci_t113_setwidth(hci, MMC_BUS_WIDTH_4);
			else
				sdhci_t113_setwidth(hci, MMC_BUS_WIDTH_1);
		}
		else if(card->version & MMC_VERSION_MMC)
		{
			if(hci->width & MMC_BUS_WIDTH_8)
				width = 2;
			else if(hci->width & MMC_BUS_WIDTH_4)
				width = 1;
			else
				width = 0;

			cmd.cmdidx = MMC_APP_CMD;
			cmd.cmdarg = card->rca << 16;
			cmd.resptype = MMC_RSP_R5;
			if(!sdhci_t113_transfer(hci, &cmd, NULL))
				return 0;

			cmd.cmdidx = SD_CMD_SWITCH_FUNC;
			cmd.cmdarg = width;
			cmd.resptype = MMC_RSP_R1;
			if(!sdhci_t113_transfer(hci, &cmd, NULL))
				return 0;

			sdhci_t113_setclock(hci, min(card->tran_speed, hci->clock));

			if(hci->width & MMC_BUS_WIDTH_8)
				sdhci_t113_setwidth(hci, MMC_BUS_WIDTH_8);
			else if(hci->width & MMC_BUS_WIDTH_4)
				sdhci_t113_setwidth(hci, MMC_BUS_WIDTH_4);
			else if(hci->width & MMC_BUS_WIDTH_1)
				sdhci_t113_setwidth(hci, MMC_BUS_WIDTH_1);
		}
	}

	cmd.cmdidx = MMC_SET_BLOCKLEN;
	cmd.cmdarg = card->read_bl_len;
	cmd.resptype = MMC_RSP_R1;
	if(!sdhci_t113_transfer(hci, &cmd, NULL))
		return 0;

	//PRINTF("SD/MMC card at the '%s' host controller:\n", hci->name);
	PRINTF("  Attached is a %s card\n", card->version & SD_VERSION_SD ? "SD" : "MMC");
	PRINTF("  Version: %s\n", sdcard_version_string(card));
	PRINTF("  Capacity: %d MB\n", (int) (card->capacity / 1024 / 1024));
	if(card->high_capacity)
		PRINTF("  High capacity card\n");
	PRINTF("  CID: %08X-%08X-%08X-%08X\n", (unsigned) card->cid[0], (unsigned) card->cid[1], (unsigned) card->cid[2], (unsigned) card->cid[3]);
	PRINTF("  CSD: %08X-%08X-%08X-%08X\n", (unsigned) card->csd[0], (unsigned) card->csd[1], (unsigned) card->csd[2], (unsigned) card->csd[3]);
	PRINTF("  Max transfer speed: %u HZ\n", (unsigned) card->tran_speed);
	PRINTF("  Manufacturer ID: %02X\n", extract_mid(card));
	PRINTF("  OEM/Application ID: %04X\n", extract_oid(card));
	PRINTF("  Product name: '%c%c%c%c%c'\n", (int) (card->cid[0] & 0xff), (int) (card->cid[1] >> 24), (int) ((card->cid[1] >> 16) & 0xff), (int) (card->cid[1] >> 8) & 0xff, (int) (card->cid[1] & 0xff));
	PRINTF("  Product revision: %u.%u\n", extract_prv(card) >> 4, extract_prv(card) & 0xf);
	PRINTF("  Serial no: %0u\n", extract_psn(card));
	PRINTF("  Manufacturing date: %u.%u\n", extract_year(card), extract_month(card));
	return 1;
}

#endif /* CPUSTYLE_ALLWINNER && WITHSDHCHW */
