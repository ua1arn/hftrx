#include "hardware.h"

#if CPUSTYLE_ALLWINNER && WITHSDHCHW

#include "formats.h"
#include "gpio.h"
#include "clocks.h"

/*
 * driver/sdhci-t113.c
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

#include "sdhci-t113.h"

//#include "PIO.h"

enum {
	SD_GCTL			= 0x00,
	SD_CLKDIV			= 0x04,
	SD_TMOR			= 0x08,
	SD_BWDR			= 0x0c,
	SD_BKSR			= 0x10,
	SD_BYCR			= 0x14,
	SD_CMDR			= 0x18,
	SD_CAGR			= 0x1c,
	SD_RESP0		= 0x20,
	SD_RESP1		= 0x24,
	SD_RESP2		= 0x28,
	SD_RESP3		= 0x2c,
	SD_IMKR			= 0x30,
	SD_MISR			= 0x34,
	SD_RISR			= 0x38,
	SD_STAR			= 0x3c,
	SD_FWLR			= 0x40,
	SD_FUNS			= 0x44,
	SD_A12A			= 0x58,
	SD_NTSR			= 0x5c,
	SD_SDBG			= 0x60,
	SD_HWRST		= 0x78,
	SD_DMAC			= 0x80,
	SD_DLBA			= 0x84,
	SD_IDST			= 0x88,
	SD_IDIE			= 0x8c,
	SD_THLDC		= 0x100,
	SD_DSBD			= 0x10c,
	SD_RES_CRC		= 0x110,
	SD_DATA7_CRC	= 0x114,
	SD_DATA6_CRC	= 0x118,
	SD_DATA5_CRC	= 0x11c,
	SD_DATA4_CRC	= 0x120,
	SD_DATA3_CRC	= 0x124,
	SD_DATA2_CRC	= 0x128,
	SD_DATA1_CRC	= 0x12c,
	SD_DATA0_CRC	= 0x130,
	SD_CRC_STA		= 0x134,
	SD_FIFO			= 0x200,
};

/*
 * Global control register bits
 */
#define SDXC_SOFT_RESET				(UINT32_C(1) << 0)
#define SDXC_FIFO_RESET				(UINT32_C(1) << 1)
#define SDXC_DMA_RESET				(UINT32_C(1) << 2)
#define SDXC_INTERRUPT_ENABLE_BIT	(UINT32_C(1) << 4)
#define SDXC_DMA_ENABLE_BIT			(UINT32_C(1) << 5)
#define SDXC_DEBOUNCE_ENABLE_BIT	(UINT32_C(1) << 8)
#define SDXC_POSEDGE_LATCH_DATA		(UINT32_C(1) << 9)
#define SDXC_DDR_MODE				(UINT32_C(1) << 10)
#define SDXC_MEMORY_ACCESS_DONE		(UINT32_C(1) << 29)
#define SDXC_ACCESS_DONE_DIRECT		(UINT32_C(1) << 30)
#define SDXC_ACCESS_BY_AHB			(UINT32_C(1) << 31)
#define SDXC_ACCESS_BY_DMA			(0 << 31)
#define SDXC_HARDWARE_RESET			(SDXC_SOFT_RESET | SDXC_FIFO_RESET | SDXC_DMA_RESET)

/*
 * Clock control bits
 */
#define SDXC_CARD_CLOCK_ON			(UINT32_C(1) << 16)
#define SDXC_LOW_POWER_ON			(UINT32_C(1) << 17)

/*
 * Bus width
 */
#define SDXC_WIDTH1					(0)
#define SDXC_WIDTH4					(1)
#define SDXC_WIDTH8					(2)

/*
 * Smc command bits
 */
#define SDXC_RESP_EXPIRE			(UINT32_C(1) << 6)
#define SDXC_LONG_RESPONSE			(UINT32_C(1) << 7)
#define SDXC_CHECK_RESPONSE_CRC		(UINT32_C(1) << 8)
#define SDXC_DATA_EXPIRE			(UINT32_C(1) << 9)
#define SDXC_WRITE					(UINT32_C(1) << 10)
#define SDXC_SEQUENCE_MODE			(UINT32_C(1) << 11)
#define SDXC_SEND_AUTO_STOP			(UINT32_C(1) << 12)
#define SDXC_WAIT_PRE_OVER			(UINT32_C(1) << 13)
#define SDXC_STOP_ABORT_CMD			(UINT32_C(1) << 14)
#define SDXC_SEND_INIT_SEQUENCE		(UINT32_C(1) << 15)
#define SDXC_UPCLK_ONLY				(UINT32_C(1) << 21)
#define SDXC_READ_CEATA_DEV			(UINT32_C(1) << 22)
#define SDXC_CCS_EXPIRE				(UINT32_C(1) << 23)
#define SDXC_ENABLE_BIT_BOOT		(UINT32_C(1) << 24)
#define SDXC_ALT_BOOT_OPTIONS		(UINT32_C(1) << 25)
#define SDXC_BOOT_ACK_EXPIRE		(UINT32_C(1) << 26)
#define SDXC_BOOT_ABORT				(UINT32_C(1) << 27)
#define SDXC_VOLTAGE_SWITCH			(UINT32_C(1) << 28)
#define SDXC_USE_HOLD_REGISTER		(UINT32_C(1) << 29)
#define SDXC_START					(UINT32_C(1) << 31)

/*
 * Interrupt bits
 */
#define SDXC_RESP_ERROR				(UINT32_C(1) << 1)
#define SDXC_COMMAND_DONE			(UINT32_C(1) << 2)
#define SDXC_DATA_OVER				(UINT32_C(1) << 3)
#define SDXC_TX_DATA_REQUEST		(UINT32_C(1) << 4)
#define SDXC_RX_DATA_REQUEST		(UINT32_C(1) << 5)
#define SDXC_RESP_CRC_ERROR			(UINT32_C(1) << 6)
#define SDXC_DATA_CRC_ERROR			(UINT32_C(1) << 7)
#define SDXC_RESP_TIMEOUT			(UINT32_C(1) << 8)
#define SDXC_DATA_TIMEOUT			(UINT32_C(1) << 9)
#define SDXC_VOLTAGE_CHANGE_DONE	(UINT32_C(1) << 10)
#define SDXC_FIFO_RUN_ERROR			(UINT32_C(1) << 11)
#define SDXC_HARD_WARE_LOCKED		(UINT32_C(1) << 12)
#define SDXC_START_BIT_ERROR		(UINT32_C(1) << 13)
#define SDXC_AUTO_COMMAND_DONE		(UINT32_C(1) << 14)
#define SDXC_END_BIT_ERROR			(UINT32_C(1) << 15)
#define SDXC_SDIO_INTERRUPT			(UINT32_C(1) << 16)
#define SDXC_CARD_INSERT			(UINT32_C(1) << 30)
#define SDXC_CARD_REMOVE			(UINT32_C(1) << 31)

//#define SDXC_INTERRUPT_ERROR_BIT    (SDXC_RESP_ERROR | SDXC_RESP_CRC_ERROR | 0*SDXC_DATA_CRC_ERROR | SDXC_RESP_TIMEOUT | SDXC_DATA_TIMEOUT | SDXC_FIFO_RUN_ERROR | SDXC_HARD_WARE_LOCKED | SDXC_START_BIT_ERROR | 0*SDXC_END_BIT_ERROR)
#define SDXC_INTERRUPT_ERROR_BIT	(SDXC_RESP_ERROR | SDXC_RESP_CRC_ERROR | SDXC_DATA_CRC_ERROR | SDXC_RESP_TIMEOUT | SDXC_DATA_TIMEOUT | SDXC_FIFO_RUN_ERROR | SDXC_HARD_WARE_LOCKED | SDXC_START_BIT_ERROR | SDXC_END_BIT_ERROR)

#define SDXC_INTERRUPT_DONE_BIT		(SDXC_AUTO_COMMAND_DONE | SDXC_DATA_OVER | SDXC_COMMAND_DONE | SDXC_VOLTAGE_CHANGE_DONE)

/*
 * Status
 */
#define SDXC_RXWL_FLAG				(UINT32_C(1) << 0) //
#define SDXC_TXWL_FLAG				(UINT32_C(1) << 1) //
#define SDXC_FIFO_EMPTY				(UINT32_C(1) << 2) //при чтении
#define SDXC_FIFO_FULL				(UINT32_C(1) << 3) //при записи
#define SDXC_CARD_PRESENT			(UINT32_C(1) << 8) //
#define SDXC_CARD_DATA_BUSY			(UINT32_C(1) << 9)
#define SDXC_DATA_FSM_BUSY			(UINT32_C(1) << 10) //не используется
#define SDXC_DMA_REQUEST			(UINT32_C(1) << 31) //не используется
#define SDXC_FIFO_SIZE				(16)

/*
 * Function select
 */
#define SDXC_CEATA_ON				(0xceaa << 16)
#define SDXC_SEND_IRQ_RESPONSE		(UINT32_C(1) << 0)
#define SDXC_SDIO_READ_WAIT		(UINT32_C(1) << 1)
#define SDXC_ABORT_READ_DATA		(UINT32_C(1) << 2)
#define SDXC_SEND_CCSD			(UINT32_C(1) << 8)
#define SDXC_SEND_AUTO_STOPCCSD		(UINT32_C(1) << 9)
#define SDXC_CEATA_DEV_IRQ_ENABLE	(UINT32_C(1) << 10)

static void WaitAfterReset(struct sdhci_t * sdhci)
{
/*
 uint32_t s;
 do
 {
  s=read32(sdhci->base + SD_GCTL);
 }
 while((s&SDXC_FIFO_RESET)||(s&SDXC_SOFT_RESET)); //ждём пока оба бита FIFO_RST и SOFT_RST не сбросятся
*/
	while (read32(sdhci->base + SD_GCTL) & (SDXC_FIFO_RESET | SDXC_SOFT_RESET))
		;

}

static int t113_transfer_command(struct sdhci_t * sdhci, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
	uint32_t cmdval = 0;
	uint32_t status = 0;

//	ktime_t timeout;

	if(cmd->cmdidx == MMC_STOP_TRANSMISSION)
	{
//		timeout = ktime_add_ms(ktime_get(), 1);

		do {
			status = read32(sdhci->base + SD_STAR);

/*			if(ktime_after(ktime_get(), timeout))
			{
				write32(sdhci->base + SD_GCTL, SDXC_HARDWARE_RESET);
				write32(sdhci->base + SD_RISR, 0xffffffff);
                                WaitAfterReset(sdhci);

				return 0;
			}*/

		} while( (status & SDXC_CARD_DATA_BUSY)  /* || (status & SDXC_DATA_FSM_BUSY) */ );
		return 1;
	}

	if(cmd->cmdidx == MMC_GO_IDLE_STATE)
		cmdval |= SDXC_SEND_INIT_SEQUENCE;
	if(cmd->resptype & MMC_RSP_PRESENT)
	{
		cmdval |= SDXC_RESP_EXPIRE;
		if(cmd->resptype & MMC_RSP_136)
			cmdval |= SDXC_LONG_RESPONSE;
		if(cmd->resptype & MMC_RSP_CRC)
			cmdval |= SDXC_CHECK_RESPONSE_CRC;
	}

	if(dat)
		cmdval |= SDXC_DATA_EXPIRE | SDXC_WAIT_PRE_OVER;

	if(dat && (dat->flag & MMC_DATA_WRITE))
		cmdval |= SDXC_WRITE;

	if(cmd->cmdidx == MMC_WRITE_MULTIPLE_BLOCK || cmd->cmdidx == MMC_READ_MULTIPLE_BLOCK)
		cmdval |= SDXC_SEND_AUTO_STOP;

	write32(sdhci->base + SD_CAGR, cmd->cmdarg);

//	if(dat)
//		sdhci->instance->SMHC_CTRL |= (UINT32_C(1) << 31);
//	else
//		sdhci->instance->SMHC_CTRL &= ~ (UINT32_C(1) << 31);

	sdhci->instance->SMHC_CTRL |= (UINT32_C(1) << 31);	// NO DMA, всегда чтение процессором

	sdhci->instance->SMHC_CMD = SDXC_START | cmdval | cmd->cmdidx;
	while (sdhci->instance->SMHC_CMD & SDXC_START)
		 ;

//	timeout = ktime_add_ms(ktime_get(),1);
	do {
		status = read32(sdhci->base + SD_RISR);

/*		if(ktime_after(ktime_get(), timeout) || (status & SDXC_INTERRUPT_ERROR_BIT))
		{
			write32(sdhci->base + SD_GCTL, SDXC_HARDWARE_RESET);
			write32(sdhci->base + SD_RISR, 0xffffffff);
                        WaitAfterReset(sdhci);

			return 0;
		}*/

	} while(!(status & SDXC_COMMAND_DONE));

	if(cmd->resptype & MMC_RSP_BUSY)
	{
//		timeout = ktime_add_ms(ktime_get(), 1);

		do {
			status = read32(sdhci->base + SD_STAR);

/*			if(ktime_after(ktime_get(), timeout))
			{
				write32(sdhci->base + SD_GCTL, SDXC_HARDWARE_RESET);
				write32(sdhci->base + SD_RISR, 0xffffffff);
                                WaitAfterReset(sdhci);

				return 0;
			}*/

		} while( (status & SDXC_CARD_DATA_BUSY) /* || (status & SDXC_DATA_FSM_BUSY) */ );
	}

	if(cmd->resptype & MMC_RSP_136)
	{
		cmd->response[0] = read32(sdhci->base + SD_RESP3);
		cmd->response[1] = read32(sdhci->base + SD_RESP2);
		cmd->response[2] = read32(sdhci->base + SD_RESP1);
		cmd->response[3] = read32(sdhci->base + SD_RESP0);
	}
	else
	{
		cmd->response[0] = read32(sdhci->base + SD_RESP0);
	}
	write32(sdhci->base + SD_RISR, 0xffffffff);
	return 1;
}

static int read_bytes(struct sdhci_t * sdhci, uint32_t * buf, uint32_t blkcount, uint32_t blksize)
{
	uint64_t count = (uint64_t) blkcount * blksize;
	uint32_t * tmp = buf;
	uint32_t status, err, done;

	while (count >= sizeof(uint32_t))
	{
		if ((read32(sdhci->base + SD_STAR) & SDXC_FIFO_EMPTY) == 0)
		{
			*(tmp) = read32(sdhci->base + SD_FIFO);
			tmp++;
			count -= sizeof(uint32_t);
		}
		else
		{
			//TP();

		}
	}

	do {
		status = read32(sdhci->base + SD_RISR);
		err = status & SDXC_INTERRUPT_ERROR_BIT;
		if(blkcount > 1)
			done = status & SDXC_AUTO_COMMAND_DONE;
		else
			done = status & SDXC_DATA_OVER;
	} while(!done && !err);

	if(err & SDXC_INTERRUPT_ERROR_BIT)
        {
		PRINTF("2 rd err=%08X, done=%08X, count=%u (%08X)\n", (unsigned) err, (unsigned) done, (unsigned) count, (unsigned) read32(sdhci->base + SD_RISR));
		return 0;
        }

	if(count)
        {
		PRINTF("3 rd err=%08X, done=%08X, count=%u (%08X)\n", (unsigned) err, (unsigned) done, (unsigned) count, (unsigned) read32(sdhci->base + SD_RISR));
		return 0;
        }
	return 1;
}

static int write_bytes(struct sdhci_t * sdhci, uint32_t * buf, uint32_t blkcount, uint32_t blksize)
{
	uint64_t count = (uint64_t) blkcount * blksize;
	uint32_t * tmp = buf;
	uint32_t status, err, done;

	while (count >= sizeof(uint32_t))
	{
		if(!(read32(sdhci->base + SD_STAR) & SDXC_FIFO_FULL))
		{
			write32(sdhci->base + SD_FIFO, *tmp);
			tmp++;
			count -= sizeof(uint32_t);
		}
		else
		{
			//TP();

		}
	}

	do {
		status = read32(sdhci->base + SD_RISR);
		err = status & SDXC_INTERRUPT_ERROR_BIT;
		if(blkcount > 1)
			done = status & SDXC_AUTO_COMMAND_DONE;
		else
			done = status & SDXC_DATA_OVER;
		//PRINTF("4 wr err=%08X, done=%u (%08X), blksize=%u\n", (unsigned) err, (unsigned) done, (unsigned) read32(sdhci->base + SD_RISR), (unsigned) blksize);
	} while(!done && !err);

	if(err & SDXC_INTERRUPT_ERROR_BIT)
	{
		PRINTF("2 wr err=%08X, done=%u (%08X)\n", (unsigned) err, (unsigned) done, (unsigned) read32(sdhci->base + SD_RISR));
		return 0;
	}

	if(count)
	{
		PRINTF("3 wr err=%08X, count=%u (%08X)\n", (unsigned) err, (unsigned) count, (unsigned) read32(sdhci->base + SD_RISR));
		return 0;
	}
	return 1;
}

static int t113_transfer_data(struct sdhci_t * sdhci, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
	write32(sdhci->base + SD_GCTL, read32(sdhci->base + SD_GCTL) | SDXC_FIFO_RESET);
	WaitAfterReset(sdhci);

	write32(sdhci->base + SD_RISR, 0xffffffff);

	uint32_t dlen = (uint32_t)(dat->blkcnt * dat->blksz);
	int ret = 0;

	write32(sdhci->base + SD_BKSR, dat->blksz);
	write32(sdhci->base + SD_BYCR, dlen);
	if(dat->flag & MMC_DATA_READ)
	{
		if(!t113_transfer_command(sdhci, cmd, dat))
		{
			return 0;
		}
		ret = read_bytes(sdhci, (uint32_t *)dat->buf, dat->blkcnt, dat->blksz);
	}
	else if(dat->flag & MMC_DATA_WRITE)
	{
		if(!t113_transfer_command(sdhci, cmd, dat))
		{
			return 0;
		}
		ret = write_bytes(sdhci, (uint32_t *)dat->buf, dat->blkcnt, dat->blksz);
	}
	return ret;
}

// retuen 0: no disk
int sdhci_t113_detect(struct sdhci_t * sdhci)
{
	return HARDWARE_SDIOSENSE_CD() != 0;
}

int sdhci_t113_reset(struct sdhci_t * sdhci)
{
	write32(sdhci->base + SD_GCTL, SDXC_HARDWARE_RESET);
	WaitAfterReset(sdhci);

	return 1;
}

int sdhci_t113_setvoltage(struct sdhci_t * sdhci, uint32_t voltage)
{
	return 1;
}

int sdhci_t113_setwidth(struct sdhci_t * sdhci, uint32_t width)
{
	switch(width)
	{
	case MMC_BUS_WIDTH_1:
		write32(sdhci->base + SD_BWDR, SDXC_WIDTH1);
		break;
	case MMC_BUS_WIDTH_4:
		write32(sdhci->base + SD_BWDR, SDXC_WIDTH4);
		break;
	case MMC_BUS_WIDTH_8:
		write32(sdhci->base + SD_BWDR, SDXC_WIDTH8);
		break;
	default:
		ASSERT(0);
		return 0;
	}
	return 1;
}

int sdhci_t113_update_clk(struct sdhci_t * sdhci)
{
	uint32_t cmd = (UINT32_C(1) << 21) | (UINT32_C(1) << 13);	// PRG_CLK WAIT_PRE_OVER

	//TP();
	write32(sdhci->base + SD_CMDR, cmd | SDXC_START);
	while(read32(sdhci->base + SD_CMDR) & SDXC_START)
		;
	//TP();

//	ktime_t timeout = ktime_add_ms(ktime_get(), 1);

	do {

/*		if(ktime_after(ktime_get(), timeout))
                {
			return 0;
                }*/

	} while(read32(sdhci->base + SD_CMDR) & SDXC_START);
	//TP();

	write32(sdhci->base + SD_RISR, read32(sdhci->base + SD_RISR));
	//TP();
	return 1;
}

int sdhci_t113_setclock(struct sdhci_t * sdhci, uint32_t clock)
{
//	uint32_t ratio = udiv32( clk_get_rate(pdat->pclk) + 2 * clock - 1, (2 * clock));
	uint32_t ratio;

	if (sdhci->instance == SMHC2)
	{
		if (clock > 1000000)
			clock = 1000000;
	}

    ratio = SMHCHARD_FREQ / (4 * clock);	// Измерения частоты сигнала CLK показали, что деление на 4 а не на 2


    if (ratio == 0)
		ratio = 1;
	else if (ratio > 255)
		ratio = 255;

	//PRINTF("***************** sdhci_t113_setclock: sdhci_t113_update_clk: ratio=%u, SMHCHARD_FREQ=%u MHz SMHCHARD_CCU_CLK_REG=0x%08X\n", (unsigned) ratio, (unsigned) (SMHCHARD_FREQ / 1000 / 1000), (unsigned) SMHCHARD_CCU_CLK_REG);
	if((ratio & 0xff) != ratio)
        {
		PRINTF("sdhci_t113_setclock: unreacheable ratio=%u (%u)\n", (unsigned) ratio, (unsigned) clock);
		//return 0;
		ratio = 255;
        }

	write32(sdhci->base + SD_CLKDIV, read32(sdhci->base + SD_CLKDIV) & ~ (UINT32_C(1) << 16));	// card clock off CCLK_ENB=0
	write32(sdhci->base + SD_CLKDIV, ratio);

	if(!sdhci_t113_update_clk(sdhci))
        {
		PRINTF("sdhci_t113_setclock: sdhci_t113_update_clk failure\n");
		return 0;
        }

	write32(sdhci->base + SD_CLKDIV, read32(sdhci->base + SD_CLKDIV) | (UINT32_C(1) << 17) | (UINT32_C(1) << 16)); // CCLK_CTRL-1, CCLK_ENB=1

	if(!sdhci_t113_update_clk(sdhci))
        {
		PRINTF("sdhci_t113_setclock: sdhci_t113_update_clk failure\n");
		return 0;
        }

	return 1;
}

int sdhci_t113_transfer(struct sdhci_t * sdhci, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
	if (dat)
	{
		//PRINTF("sdhci_t113_transfer: blkcnt=%u, blksz=%u\n", (unsigned) dat->blkcnt, (unsigned) dat->blksz);
	}
	if(!dat)
		return t113_transfer_command(sdhci, cmd, dat);

	return t113_transfer_data(sdhci, cmd, dat);
}

//-----------------------------------------------------------------------------------

void sdhci_t113_clock(void)
{
// SMHC_BGR_REG&=~ (1<<16);                       //SMHC0 assert reset
//
//// SMHC0_CLK_REG|=(1UL<<31);                   //clock enable SMHC0, HOSC         selected, N=1, M=1 =>  24 MHz ( Quartz  24 MHz /1 /1 ) = HOSC_CLOCK
// SMHC0_CLK_REG=(1UL<<31)|(1<<24)|(1<<8)|(3-1); //clock enable SMHC0, PLL_PERI(1x) selected, N=2, M=3 => 100 MHz ( PLL    600 MHz /2 /3 ) = PLLPERI1X_CLOCK_DIV
//
// SMHC_BGR_REG|=1;                              //gating on clock SMHC0
//
// SMHC_BGR_REG|=(1<<16);                        //SMHC0 de-assert reset
#if ! defined CPUSTYLE_A64
	if (SMHCHARD_PTR == SMHC2)
	{

		SMHCHARD_PTR->SMHC_FIFOTH = 0x300F00F0;
		SMHCHARD_PTR->SMHC_THLD = 0x02000004;
		SMHCHARD_PTR->SMHC_SFC =
				(3u << 1) | // STOP_CLK_CTRL
				(1u << 0) |	// BYPASS_EN When set, sample FIFO will be
				0;
		SMHCHARD_PTR->SMHC_CSDC = 0x00000006;
		SMHCHARD_PTR->SMHC_NTSR = 0;
		//SMHCHARD_PTR->EMMC_DDR_SBIT_DET |= (UINT32_C(1) << 31);

		//	The steps to calibrate delay chain are as follows:
		//	Step1: Enable SMHC. In order to calibrate delay chain by operation registers in SMHC, SMHC must be enabled through SMHC Bus Gating Reset Register and SMHC0/1/2 Clock Register.
		//	Step2: Configure a proper clock for SMHC. Calibration delay chain is based on the clock for SMHC from Clock Control Unit(CCU). Calibration delay chain is an internal function in SMHC and does not need device. So, it is unnecessary to open clock signal for device. The recommended clock frequency is 200 MHz.
		//	Step3: Set proper initial delay value. Writing 0xA0 to delay control register enables Delay Software Enable (bit[ and sets initial delay value 0x20 to Delay chain(bit[5:0]). Then write 0x0 to delay control register to clear the value.
		//	Step4: Write 0x8000 to delay control register to start calibrate delay chain.
		//	Step5: Wait until the flag(bit14 in delay control register) of calibration done is set. The number of delay cells is shown at bit[13:8] in delay control register. The delay time generated by these delay cells is equal to the cycle of SMHC’s clock nearly. This value is the result of calibration.
		//	Step6: Calculate the delay time of one delay cell according to the cycle of SMHC’s clock and the result of calibration.

		/* Delay calibration */
		SMHCHARD_PTR->SMHC_SAMP_DL = 0xA0;
		SMHCHARD_PTR->SMHC_SAMP_DL = 0;
		SMHCHARD_PTR->SMHC_SAMP_DL |= (UINT32_C(1) << 15);
		while ((SMHCHARD_PTR->SMHC_SAMP_DL & (UINT32_C(1) << 14)) == 0)
			;
		SMHCHARD_PTR->SMHC_SAMP_DL = (SMHCHARD_PTR->SMHC_SAMP_DL & ~ (UINT32_C(0x3F) << 0)) |
			((SMHCHARD_PTR->SMHC_SAMP_DL >> 8) & 0x3F) |
			0;
		SMHCHARD_PTR->SMHC_SAMP_DL |= (UINT32_C(1) << 7);	// Sample Delay Software Enable
		//PRINTF("SMHC_SAMP_DL calibration result=0x%02X\n", (unsigned) (SMHCHARD_PTR->SMHC_SAMP_DL >> 8) & 0x3F);

		/* Delay calibration */
		SMHCHARD_PTR->SMHC_DS_DL = 0xA0;
		SMHCHARD_PTR->SMHC_DS_DL = 0;
		SMHCHARD_PTR->SMHC_DS_DL |= (UINT32_C(1) << 15);
		while ((SMHCHARD_PTR->SMHC_DS_DL & (UINT32_C(1) << 14)) == 0)
			;
		SMHCHARD_PTR->SMHC_DS_DL = (SMHCHARD_PTR->SMHC_DS_DL & ~ (UINT32_C(0x3F) << 0)) |
			((SMHCHARD_PTR->SMHC_DS_DL >> 8) & 0x3F) |
			0;
		SMHCHARD_PTR->SMHC_DS_DL |= (UINT32_C(1) << 7);	// Sample Delay Software Enable
		//PRINTF("SMHC_DS_DL calibration result=0x%02X\n", (unsigned) (SMHCHARD_PTR->SMHC_DS_DL >> 8) & 0x3F);

	}
#endif
}

struct sdhci_t HCI;
struct sdcard_t CARD;

int sdhci_t113_init(struct sdhci_t * sdhci)
{
//	SMHCHARD_CCU_CLK_REG |= (UINT32_C(1) << 31);
//	SMHCHARD_CCU_CLK_REG |= (UINT32_C(1) << 31);
//	hardware_sdhost_initialize();
 //Порты настроены в PIO.c (без настройки портов - НЕ работает)

//	SMHCHARD_PTR->SMHC_HWRST &= ~ UINT32_C(1) << 0;
//	local_delay_ms(10);
//	SMHCHARD_PTR->SMHC_HWRST |= UINT32_C(1) << 0;
//	local_delay_ms(10);

 memset(&HCI,0,sizeof(HCI));
 HCI.base = SMHCHARD_BASE;
 HCI.instance = SMHCHARD_PTR;
 memset(&CARD,0,sizeof(CARD));

 if(!sdhci_t113_detect(sdhci)) //если слот без карты - неудачно!
	 return 0;

 sdhci_t113_clock();                        //клок, гейт, ресет

#if WITHSDHCHW1P8V
 sdhci->voltage   = MMC_VDD_165_195;
#else /* WITHSDHCHW1P8V */
 sdhci->voltage   = MMC_VDD_27_36;
#endif /* WITHSDHCHW1P8V */

#if WITHSDHCHW8BIT
 sdhci->width     = MMC_BUS_WIDTH_8;
#elif WITHSDHCHW4BIT
 sdhci->width     = MMC_BUS_WIDTH_4;
#else
 sdhci->width     = MMC_BUS_WIDTH_1;
#endif
 sdhci->clock     = SMHCHARD_FREQ; //HOSC_CLOCK
 sdhci->isspi     = 0;

 return 1;
}
#endif /* CPUSTYLE_ALLWINNER && WITHSDHCHW */
