/*
 * i2c.c
 *
 *  Created on: 23 февр. 2023 г.
 *      Author: admin
 */
#include "hardware.h"	/* зависящие от процессора функции работы с портами */


#if (CPUSTYLE_T113 || CPUSTYLE_F133)

#include "clocks.h"
#include "i2c.h"
#include "gpio.h"
#include "formats.h"
#include <string.h>
#include <stdlib.h>


enum {
	I2C_M_TEN			= 0x0010,
	I2C_M_RD			= 0x0001,
	I2C_M_STOP			= 0x8000,
	I2C_M_NOSTART		= 0x4000,
	I2C_M_REV_DIR_ADDR	= 0x2000,
	I2C_M_IGNORE_NAK	= 0x1000,
	I2C_M_NO_RD_ACK		= 0x0800,
	I2C_M_RECV_LEN		= 0x0400,
};

enum {
	I2C_ADDR			= 0x000,
	I2C_XADDR			= 0x004,
	I2C_DATA 			= 0x008,
	I2C_CNTR			= 0x00c,
	I2C_STAT			= 0x010,
	I2C_CCR				= 0x014,
	I2C_SRST			= 0x018,
	I2C_EFR				= 0x01c,
	I2C_LCR				= 0x020,
};

enum {
	I2C_STAT_BUS_ERROR	= 0x00,
	I2C_STAT_TX_START	= 0x08,
	I2C_STAT_TX_RSTART	= 0x10,
	I2C_STAT_TX_AW_ACK	= 0x18,
	I2C_STAT_TX_AW_NAK	= 0x20,
	I2C_STAT_TXD_ACK	= 0x28,
	I2C_STAT_TXD_NAK	= 0x30,
	I2C_STAT_LOST_ARB	= 0x38,
	I2C_STAT_TX_AR_ACK	= 0x40,
	I2C_STAT_TX_AR_NAK	= 0x48,
	I2C_STAT_RXD_ACK	= 0x50,
	I2C_STAT_RXD_NAK	= 0x58,
	I2C_STAT_IDLE		= 0xf8,
};

typedef struct i2c_t113_pdata_t {
	uintptr_t virt;
	char * clk;
	int reset;
	int sda;
	int sdacfg;
	int scl;
	int sclcfg;
}i2c_t113_pdata_t;

struct i2c_msg_t {
	int addr;
	int len;
	void * buf;
};

static i2c_t113_pdata_t pdat_i2c;
static int I2C_ERROR_COUNT;

static uint32_t read32(uintptr_t a)
{
	return * (volatile uint32_t *) a;
}

static void write32(uintptr_t a, uint32_t v)
{
	* (volatile uint32_t *) a = v;
}
static void t113_i2c_set_rate(struct i2c_t113_pdata_t * pdat, uint64_t rate){
	uint64_t pclk = 100000000;	//clk_get_rate(pdat->clk);
	uint_fast64_t freq, delta, best = 0x7fffffffffffffffLL;
	int tm = 5, tn = 0;
	int m, n;

	for(n = 0; n <= 7; n++)
	{
		for(m = 0; m <= 15; m++)
		{
			freq = pclk / (10 * (m + 1) * (1 << n));
			delta = rate - freq;
			if(delta >= 0 && delta < best)
			{
				tm = m;
				tn = n;
				best = delta;
			}
			if(best == 0)
				break;
		}
	}
	write32(pdat->virt + I2C_CCR, ((tm & 0xf) << 3) | ((tn & 0x7) << 0));
}
extern __IO uint32_t uwTick;

volatile unsigned int timeout1;

static int t113_i2c_wait_status(struct i2c_t113_pdata_t * pdat){
	local_delay_ms(40);															//TODO: Допилить - убрать задержку!!!
	volatile unsigned int timeout = uwTick+5*10;	//uwTick+5мс
	do {
		if((read32(pdat->virt + I2C_CNTR) & (1 << 3))){
			unsigned int stat = read32(pdat->virt + I2C_STAT);
			//PRINTF("t113_i2c_wait_status = 0x%X \n", stat);
			return stat;
		}
	} while(uwTick>timeout);

	I2C_ERROR_COUNT++;
	 PRINTF("t113_i2c_wait_status = I2C_STAT_BUS_ERROR \n");
	return I2C_STAT_BUS_ERROR;
}

static int t113_i2c_start(struct i2c_t113_pdata_t * pdat){
	uint32_t val;
	 PRINTF("I2C start \n");
	val = read32(pdat->virt + I2C_CNTR);
	val |= (1 << 5) | (1 << 3);
	write32(pdat->virt + I2C_CNTR, val);
	volatile unsigned int timeout = uwTick+5*100;	//uwTick+5мс
	do {
		if(!(read32(pdat->virt + I2C_CNTR) & (1 << 5)))
			break;
	} while(uwTick>timeout);
	//PRINTF("I2C start out\n");
	return t113_i2c_wait_status(pdat);
}

static int t113_i2c_stop(struct i2c_t113_pdata_t * pdat){
	uint32_t val;

	val = read32(pdat->virt + I2C_CNTR);
	val |= (1 << 4) | (1 << 3);
	write32(pdat->virt + I2C_CNTR, val);
	volatile unsigned int timeout = uwTick+5*10;
	do {
		if(!(read32(pdat->virt + I2C_CNTR) & (1 << 4)))
			break;
	} while(uwTick>timeout);
	return 1;
}

static int t113_i2c_send_data(struct i2c_t113_pdata_t * pdat, uint8_t dat)
{
	write32(pdat->virt + I2C_DATA, dat);
	write32(pdat->virt + I2C_CNTR, read32(pdat->virt + I2C_CNTR) | (1 << 3));
	return t113_i2c_wait_status(pdat);
}

static int t113_i2c_read(struct i2c_t113_pdata_t * pdat, struct i2c_msg_t * msg){
	uint8_t * p = msg->buf;
	int len = msg->len;

	if(t113_i2c_send_data(pdat, (uint8_t)(msg->addr << 1 | 1)) != I2C_STAT_TX_AR_ACK)
		return -1;

	write32(pdat->virt + I2C_CNTR, read32(pdat->virt + I2C_CNTR) | (1 << 2));
	while(len > 0){
		if(len == 1){
			write32(pdat->virt + I2C_CNTR, (read32(pdat->virt + I2C_CNTR) & ~(1 << 2)) | (1 << 3));
			if(t113_i2c_wait_status(pdat) != I2C_STAT_RXD_NAK)
				return -1;
		}else{
			write32(pdat->virt + I2C_CNTR, read32(pdat->virt + I2C_CNTR) | (1 << 3));
			if(t113_i2c_wait_status(pdat) != I2C_STAT_RXD_ACK)
				return -1;
		}
		*p++ = read32(pdat->virt + I2C_DATA);
		len--;
	}
	return 0;
}

static int t113_i2c_write(struct i2c_t113_pdata_t * pdat, struct i2c_msg_t * msg)
{
	uint8_t * p = msg->buf;
	int len = msg->len;

	if(t113_i2c_send_data(pdat, (uint8_t)(msg->addr << 1)) != I2C_STAT_TX_AW_ACK){
		return -1;
	}
	while(len > 0)
	{
		if(t113_i2c_send_data(pdat, *p++) != I2C_STAT_TXD_ACK)
			return -1;
		len--;
	}
	return 0;
}

// TWI0...n
//TWI2	0x02502800
void i2c_init(uint8_t TWIx){
	switch (TWIx){
	case 1:
		TWIHARD_INITIALIZE();
		break;
//	case 2:
//		TWI2HARD_INITIALIZE();
//		break;

	}

	// Open the clock gate
	volatile uintptr_t addr;
	unsigned int val;

	// Deassert reset
	addr = 0x0200191c;
	val = read32(addr);
	val |= 1 << (16 + TWIx);
	write32(addr, val);

	addr = 0x0200191c;
	val = read32(addr);
	val |= 1 << (0 + TWIx);
	write32(addr, val);


	    switch (TWIx){
		case 0:
			pdat_i2c.virt = TWI0_BASE;
		break;
		case 1:
			pdat_i2c.virt = TWI1_BASE;
		break;
		case 2:
			pdat_i2c.virt = TWI2_BASE;
		break;
		case 3:
			pdat_i2c.virt = TWI3_BASE;
		break;
		}

	t113_i2c_set_rate(&pdat_i2c, 400000);
	write32(pdat_i2c.virt + I2C_CNTR, 1 << 6);
	write32(pdat_i2c.virt + I2C_SRST, 1 << 0);


	 PRINTF("I2C init ok \n");
//Инициализация устройства на шине
	/*if(Dash_main_str.ACC==ACC_LSM6DS3HTR){
		unsigned char am_i_reg;
		PRINTF("Read am_i_reg\n");
		if(I2C_ReadBuffer(AccAdr, &am_i_reg, LSM6DS3_ACC_GYRO_WHO_AM_I_REG, 1)){
			//PRINTF("Read am_i_reg= %x\n", am_i_reg);
			//PRINTF("\n");
			if(am_i_reg==0x69){
				PRINTF("am_i_reg OK\n");
				AccInit();
				PRINTF("AccInit() OK\n");
				ACC_Ok=1;
			}
		}else{
			//PRINTF("Read am_i_reg= %x\n", am_i_reg);
			//PRINTF("ACC no init\n");
		}
	}*/
	t113_i2c_stop(&pdat_i2c);

}


unsigned char I2C_WriteByte(unsigned char slaveAddr, const unsigned char* pBuffer, unsigned char WriteAddr){
	//записываем адрес который хотим прочитать
	unsigned char wr_buf[2];
	int res;

	wr_buf[0] = WriteAddr;
	wr_buf[1] = pBuffer[0];

	struct i2c_msg_t  msgs;
	msgs.addr = slaveAddr;
	msgs.len = 2;
	msgs.buf = wr_buf;
	//генереруем старт
	if(t113_i2c_start(&pdat_i2c) != I2C_STAT_TX_START){
		 PRINTF("I2C start error\n");
		return 0;
	}
	 PRINTF("I2C start ok\n");

	res = t113_i2c_write(&pdat_i2c, &msgs);
	if(res!=0){
		 PRINTF("I2C write err \n");
		return 0;
	}else{
		 PRINTF("I2C write ok \n");
	}
	 PRINTF("I2C t113_i2c_stop in\n");
	t113_i2c_stop(&pdat_i2c);
	 PRINTF("I2C t113_i2c_stop ok\n");

	return 1;
}


// возвращает 1 если все хорошо и 0 если что-то не так
unsigned char I2C_ReadBuffer(unsigned char slaveAddr, unsigned char* pBuffer, unsigned char ReadAddr, unsigned short NumByteToRead){
	int res;
	PRINTF("!!!!!=====I2C_ReadBuffer=====!!!!!\n");
	//записываем адрес который хотим прочитать
	struct i2c_msg_t  msgs;
	msgs.addr = slaveAddr;
	msgs.len = 1;
	msgs.buf = &ReadAddr;

	//генереруем старт

	res = t113_i2c_start(&pdat_i2c);
	if(res != I2C_STAT_TX_START){
		PRINTF("I2C start error\n");
		t113_i2c_stop(&pdat_i2c);
		return 0;
	}
	PRINTF("I2C start ok\n");

	res = t113_i2c_write(&pdat_i2c, &msgs);
	if(res!=0){
		PRINTF("I2C write err \n");
		return 0;
	}
	PRINTF("I2C write ok \n");

	if(t113_i2c_start(&pdat_i2c) != I2C_STAT_TX_RSTART){	//генерируем рестарт
		PRINTF("I2C restart error\n");
		return 0;
	}
	PRINTF("I2C restart Ok\n");
	//читаем регистр
	msgs.addr = slaveAddr;
	msgs.len = NumByteToRead;
	msgs.buf = pBuffer;

	res = t113_i2c_read(&pdat_i2c, &msgs);
	if(res!=0){
		PRINTF("I2C read err \n");
		//I2C_ERROR = 0x04;
		return 0;
	}
	PRINTF("I2C read ok \n");
/**/
	PRINTF("I2C t113_i2c_stop in\n");
	t113_i2c_stop(&pdat_i2c);
	PRINTF("I2C t113_i2c_stop ok\n");

	return 1;
}

uint16_t i2chw_read(uint16_t slave_address, uint8_t * buf, uint32_t size)
{
	return 0;
}

uint16_t i2chw_write(uint16_t slave_address, const uint8_t * buf, uint32_t size)
{
	return 0;
}

uint16_t i2chw_read2(uint16_t slave_address, uint16_t reg_address, uint8_t * buf, uint32_t size)
{
	I2C_ReadBuffer(slave_address, buf, reg_address, size);
	return 0;
}

uint16_t i2chw_write2(uint16_t slave_address, uint16_t reg_address, const uint8_t * buf, uint32_t size)
{
	return 0;
}

#endif /* (CPUSTYLE_T113 || CPUSTYLE_F133) */
