/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN

/* TWI (I2C) interface */
//
#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	// for debug prints
#include "gpio.h"

#if WITHTWIHW || WITHTWISW

void i2c_writeX(const i2cp_t * p, uint_fast8_t d);

// Обслуживание I2C без использования аппаратных контроллеров процессора
// программное "ногодрыгание" выводами.

// Парметры скорости для функций семейства i2c_xxx
static i2cp_t i2cp_1 =
{
		.ch = I2CP_I2C1,
		.usdelayv = (1000000 / 2) / 400000
};

// Парметры скорости для функций семейства i2c2_xxx
static i2cp_t i2cp_2 =
{
		.ch = I2CP_I2C2,
		.usdelayv = (1000000 / 2) / 400000
};

//We use a small delay routine between SDA and SCL changes to give a clear
// sequence on the I2C bus. This is nothing more than a subroutine call and return.
/* задержка обепечивает скорость обмена по I2C при программной реализации протокола - 1/2 периода частоты 400 кГц */
static void i2c_delay(const i2cp_t * p)
{
	local_delay_us(p->usdelayv);
#if LCDMODEX_SII9022A
	local_delay_us(25);
#endif /* LCDMODEX_SII9022A */
#if PCF8576C
	local_delay_us(15);
#endif /* PCF8576C */
}

#if CPUSTYLE_ATMEGA
	#define SET_TWCK() do { TARGET_TWI_TWCK_DDR &= ~ TARGET_TWI_TWCK; } while (0)	// SCL = 1
	#define CLR_TWCK() do { TARGET_TWI_TWCK_DDR |= TARGET_TWI_TWCK; } while (0)	// SCL = 0
	#define SET_TWD() do { TARGET_TWI_TWD_DDR &= ~ TARGET_TWI_TWD;  } while (0)	// SDA = 1
	#define CLR_TWD() do { TARGET_TWI_TWD_DDR |= TARGET_TWI_TWD; } while (0)	// SDA = 0

	#define GET_TWCK() ((TARGET_TWI_TWCK_PIN & TARGET_TWI_TWCK) != 0)
	#define GET_TWD() ((TARGET_TWI_TWD_PIN & TARGET_TWI_TWD) != 0)

	#define SET2_TWCK() do { TARGET_TWI2_TWCK_DDR &= ~ TARGET_TWI2_TWCK; } while (0)	// SCL = 1
	#define CLR2_TWCK() do { TARGET_TWI2_TWCK_DDR |= TARGET_TWI2_TWCK; } while (0)	// SCL = 0
	#define SET2_TWD() do { TARGET_TWI2_TWD_DDR &= ~ TARGET_TWI2_TWD; } while (0)	// SDA = 1
	#define CLR2_TWD() do { TARGET_TWI2_TWD_DDR |= TARGET_TWI2_TWD; } while (0)	// SDA = 0

	#define GET2_TWCK() ((TARGET_TWI2_TWCK_PIN & TARGET_TWI2_TWCK) != 0)
	#define GET2_TWD() ((TARGET_TWI2_TWD_PIN & TARGET_TWI2_TWD) != 0)

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	#if WITHTWISW && ! defined (TWISOFT_INITIALIZE)

	#define TWISOFT_INITIALIZE() do { } while (0)

	#define SET_TWCK() do { xc7z_gpio_output(TARGET_TWI_TWCK_MIO); xc7z_writepin(TARGET_TWI_TWCK_MIO, 1); } while (0)

	#define CLR_TWCK() do { xc7z_gpio_output(TARGET_TWI_TWCK_MIO); xc7z_writepin(TARGET_TWI_TWCK_MIO, 0); } while (0)

	#define SET_TWD() do { xc7z_gpio_output(TARGET_TWI_TWD_MIO); xc7z_writepin(TARGET_TWI_TWD_MIO, 1); } while (0)

	#define CLR_TWD() do { xc7z_gpio_output(TARGET_TWI_TWD_MIO); xc7z_writepin(TARGET_TWI_TWD_MIO, 0); } while (0)

	uint8_t GET_TWCK(void)
	{
		xc7z_gpio_input(TARGET_TWI_TWCK_MIO);
		return xc7z_readpin(TARGET_TWI_TWCK_MIO);
	}

	uint8_t GET_TWD(void)
	{
		xc7z_gpio_input(TARGET_TWI_TWD_MIO);
		return xc7z_readpin(TARGET_TWI_TWD_MIO);
	}

#endif /* WITHTWISW */

#elif CPUSTYLE_ARM || CPUSTYLE_ATXMEGA || CPUSTYLE_RISCV

	#define SET_TWCK() do { TARGET_TWI_TWCK_PORT_S(TARGET_TWI_TWCK); } while (0)	// SCL = 1
	#define CLR_TWCK() do { TARGET_TWI_TWCK_PORT_C(TARGET_TWI_TWCK); } while (0)	// SCL = 0
	#define SET_TWD() do { TARGET_TWI_TWD_PORT_S(TARGET_TWI_TWD);  } while (0)	// SDA = 1
	#define CLR_TWD() do { TARGET_TWI_TWD_PORT_C(TARGET_TWI_TWD);  } while (0)	// SDA = 0

	#define GET_TWCK() ((TARGET_TWI_TWCK_PIN & TARGET_TWI_TWCK) != 0)
	#define GET_TWD() ((TARGET_TWI_TWD_PIN & TARGET_TWI_TWD) != 0)

	/* второй канал I2C */
	#define SET2_TWCK() do { TARGET_TWI2_TWCK_PORT_S(TARGET_TWI2_TWCK); } while (0)	// SCL = 1
	#define CLR2_TWCK() do { TARGET_TWI2_TWCK_PORT_C(TARGET_TWI2_TWCK); } while (0)	// SCL = 0
	#define SET2_TWD() do { TARGET_TWI2_TWD_PORT_S(TARGET_TWI2_TWD); } while (0)	// SDA = 1
	#define CLR2_TWD() do { TARGET_TWI2_TWD_PORT_C(TARGET_TWI2_TWD); } while (0)	// SDA = 0

	#define GET2_TWCK() ((TARGET_TWI2_TWCK_PIN & TARGET_TWI2_TWCK) != 0)
	#define GET2_TWD() ((TARGET_TWI2_TWD_PIN & TARGET_TWI2_TWD) != 0)

#else
	#error Undefined CPUSTYLE_xxx
#endif

#if defined (TWISOFT_INITIALIZE)

static void i2c_softbusrelease(void)
{
	uint_fast8_t i;
	// release I2C bus
	CLR_TWD();
	for (i = 0; i < 24; ++ i)
	{
		CLR_TWCK();
		SET_TWCK();
	}
	SET_TWD();
	for (i = 0; i < 24; ++ i)
	{
		CLR_TWCK();
		SET_TWCK();
	}
}

#endif /* defined (TWISOFT_INITIALIZE) */

#if defined (TWISOFT2_INITIALIZE)

static void i2c2_softbusrelease(void)
{
	uint_fast8_t i;
	// release I2C bus
	CLR2_TWD();
	for (i = 0; i < 24; ++ i)
	{
		CLR2_TWCK();
		SET2_TWCK();
	}
	SET2_TWD();
	for (i = 0; i < 24; ++ i)
	{
		CLR2_TWCK();
		SET2_TWCK();
	}
}

#endif /* defined (TWISOFT2_INITIALIZE) */

#if WITHTWIHW

#if CPUSTYLE_ATMEGA

#include <util/twi.h>

void i2c_initialize(void)
{
	const i2cp_t * const p1 = & i2cp_1;
	const i2cp_t * const p2 = & i2cp_2;

	TWISOFT_INITIALIZE();
	i2c_softbusrelease();
	hardware_twi_master_configure();

#if WITHTWIHW
	TWIHARD_INITIALIZE();
#endif
}


enum i2c_read_mode_t
{
	I2C_START,
	I2C_DATA,
	I2C_DATA_ACK,
	I2C_STOP
};

#define MAX_TRIES 1 // 50 

/* START I2C Routine */ 
static uint_fast8_t 
i2c_transmit(uint_fast8_t type) 
{ 
	uint_fast8_t w;

	switch (type) 
	{ 
	default:
	case I2C_START: // Send Start Condition 
		TWCR = (1U << TWINT) | (1U << TWSTA) | (1U << TWEN); 
		break; 
	case I2C_DATA: // Send Data with No-Acknowledge 
		TWCR = (1U << TWINT) | (1U << TWEN); 
		break; 
	case I2C_DATA_ACK: // Send Data with Acknowledge 
		TWCR = (1U << TWEA) | (1U << TWINT) | (1U << TWEN); 
		break; 
	case I2C_STOP: // Send Stop Condition 
		TWCR = (1U << TWINT) | (1U << TWEN) | (1U << TWSTO); 
		return 0; 
	} 

	// Wait for TWINT flag set on Register TWCR 
	w = 255;
	while (w -- && !(TWCR & (1U << TWINT)))
		local_delay_us(1);

	// Return TWI Status Register, mask the prescaller bits (TWPS1,TWPS0) 
	return (TWSR & TW_STATUS_MASK); 
	//return TW_STATUS;
} 

/* char */ void i2c_start(
	//uint_fast8_t dev_id, uint_fast8_t dev_addr, uint_fast8_t rw_type
	uint_fast8_t addr
	) 
{ 
	uint_fast8_t n = 0; 
	uint_fast8_t twi_status; 
	//char r_val = -1; 

i2c_retry: 
	if (n++ >= MAX_TRIES) 
	{
		return ; //r_val; 
	}

	// Transmit Start Condition 
	twi_status = i2c_transmit(I2C_START); 

	// Check the TWI Status 
	if (twi_status == TW_MT_ARB_LOST) 
		goto i2c_retry; 
	if ((twi_status != TW_START) && (twi_status != TW_REP_START)) 
		goto i2c_quit; 

	// Send slave address (SLA_W) 
	TWDR = addr; // (dev_id & 0xF0) | (dev_addr & 0x07) | rw_type; 

	// Transmit I2C Data 
	twi_status = i2c_transmit(I2C_DATA); 

	// Check the TWSR status 
	if ((twi_status == TW_MT_SLA_NACK) || (twi_status == TW_MT_ARB_LOST)) 
		goto i2c_retry; 
	if (twi_status != TW_MT_SLA_ACK) 
		goto i2c_quit; 

	//r_val=0; 

i2c_quit: 
	return; // r_val; 
} 

// Вызвать после последнего i2c_write()
void i2c_waitsend(void)
{

}

void i2c_stop(void) 
{ 
	//uint_fast8_t twi_status; 

	// Transmit I2C Data 
	/*twi_status = */ i2c_transmit(I2C_STOP); 
} 

/* char */ void i2c_write(uint_fast8_t data)
{ 
	uint_fast8_t twi_status; 
	//char r_val = -1; 

	// Send the Data to I2C Bus 
	TWDR = data; 

	// Transmit I2C Data 
	twi_status = i2c_transmit(I2C_DATA); 

	// Check the TWSR status 
	if (twi_status != TW_MT_DATA_ACK) goto i2c_quit; 

	//r_val=0; 

i2c_quit: 
	return; // r_val; 
} 

// запись, после чего restart
/* char */ void i2c_write_withrestart(uint_fast8_t data)
{
	i2c_write(data);
	i2c_waitsend();

}

// запись, после чего restart
/* char */ void i2c_write_withrestartX(const i2cp_t * p, uint_fast8_t data)
{
	i2c_write(data);
	i2c_waitsend();
	
}

//#define I2C_READ_ACK 1  // i2c_read parameter
//#define I2C_READ_NACK 0		// ack_type - last parameterr in read block

// tested
void i2c_read(uint8_t *data, uint_fast8_t ack_type)
{ 
	uint_fast8_t twi_status; 
	//char r_val = -1; 

	if (ack_type == I2C_READ_ACK || ack_type == I2C_READ_ACK_1) 
	{ 
		// Read I2C Data and Send Acknowledge 
		twi_status = i2c_transmit(I2C_DATA_ACK); 

		if (twi_status != TW_MR_DATA_ACK) 
			goto i2c_quit; 
		// Get the Data 
		* data = TWDR; 
		//r_val=0; 
	} else if (ack_type == I2C_READ_ACK_NACK) { 
		// чтение первого и единственного байта по I2C
		// Read I2C Data and Send No Acknowledge 
		twi_status = i2c_transmit(I2C_DATA); 
		// Get the Data 
		* data = TWDR; 
		//r_val=0; 

		//if (twi_status != TW_MR_DATA_NACK) 
		//	goto i2c_quit; 
		////twi_status = i2c_transmit(I2C_STOP); 

		//if (twi_status != TW_MR_DATA_NACK) 
		//	goto i2c_quit; 
	} else { 
		// Read I2C Data and Send No Acknowledge 
		twi_status = i2c_transmit(I2C_DATA); 

		if (twi_status != TW_MR_DATA_NACK) 
			goto i2c_quit; 
		// Get the Data 
		* data = TWDR; 
		//r_val=0; 
	} 




i2c_quit: 
	return ;//r_val; 
} 


#elif CPUSTYLE_AT91SAM7S

// On ARM machines

/* char */ void i2c_start(
	uint_fast8_t address
	) 
{
	if (address & 0x01)
	{	
		AT91C_BASE_TWI->TWI_MMR = AT91C_TWI_IADRSZ_NO | (AT91C_TWI_DADR & (address << 15)) | AT91C_TWI_MREAD;
	}
	else
	{
		AT91C_BASE_TWI->TWI_MMR = AT91C_TWI_IADRSZ_NO | (AT91C_TWI_DADR & (address << 15));
	}
}

// Вызвать после последнего i2c_write()
void i2c_waitsend(void)
{

}

void i2c_stop(void)
{
	
    AT91C_BASE_TWI->TWI_CR = AT91C_TWI_STOP;
	unsigned w = 255;
	while (w -- && (AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXCOMP) == 0)
		local_delay_us(1);

}

/* char */ void i2c_write(
	uint_fast8_t byte
	)
{
	unsigned w = 255;
	while (w -- && (AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXRDY) == 0)
		local_delay_us(1);
    AT91C_BASE_TWI->TWI_THR = byte;

	//return 0;
}
	
// запись, после чего restart
/* char */ void i2c_write_withrestartX(const i2cp_t * p, uint_fast8_t data)
{
	i2c_writeX(p, data);
	i2c_waitsendX(p);
	
}

void i2c_readX(const i2cp_t * p, uint8_t *data, uint_fast8_t ack_type)
{
	switch (ack_type)
	{
	case I2C_READ_ACK_NACK:	/* чтение первого и единственного байта ответа */
		{
			// Чтение единственного (первого и последнего) байта.
			AT91C_BASE_TWI->TWI_CR = AT91C_TWI_START | AT91C_TWI_STOP;
			// Дождаться окончания перехода в состояние STOP
			unsigned w = 255;
			while (w -- && (AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXCOMP) == 0)	
				local_delay_us(1);
			* data = AT91C_BASE_TWI->TWI_RHR;
		}
		break;

	case I2C_READ_NACK:
		{
			// Чтение последнего байта.
			// Дождаться окончания перехода в состояние STOP
			AT91C_BASE_TWI->TWI_CR = AT91C_TWI_STOP;
			unsigned w = 255;
			while ( w -- && (AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXCOMP) == 0)
				local_delay_us(1);
			* data = AT91C_BASE_TWI->TWI_RHR;
		}
		break;

	case I2C_READ_ACK_1:
			AT91C_BASE_TWI->TWI_CR = AT91C_TWI_START;
	case I2C_READ_ACK:
		{
			// первый и последующие байты в последовательном чтении.
			unsigned w = 255;
			while (w -- && (AT91C_BASE_TWI->TWI_SR & AT91C_TWI_RXRDY) == 0)
				local_delay_us(1);
			* data = AT91C_BASE_TWI->TWI_RHR;
		}
		break;

	}
	return ;//0;
}


void i2c_initialize(void)
{
	const i2cp_t * const p1 = & i2cp_1;
	const i2cp_t * const p2 = & i2cp_2;
	TWISOFT_INITIALIZE();
	i2c_softbusrelease();

    // Configure clock
	hardware_twi_master_configure();
#if WITHTWIHW
	TWIHARD_INITIALIZE();
#endif
}

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX


/** 
  * @brief  Communication start
  * 
  * After sending the START condition (I2C_GenerateSTART() function) the master 
  * has to wait for this event. It means that the Start condition has been correctly 
  * released on the I2C bus (the bus is free, no other devices is communicating).
  * 
  */
/* --EV5 */
//#define  I2C_EVENT_MASTER_MODE_SELECT                      ((uint32_t)0x00030001)  /* BUSY, MSL and SB flag */
#define	I2C_EVENT_MASTER_MODE_SELECT	(((I2C_SR2_MSL | I2C_SR2_BUSY) << 16) | (I2C_SR1_SB))
/** 
  * @brief  Address Acknowledge
  * 
  * After checking on EV5 (start condition correctly released on the bus), the 
  * master sends the address of the slave(s) with which it will communicate 
  * (I2C_Send7bitAddress() function, it also determines the direction of the communication: 
  * Master transmitter or Receiver). Then the master has to wait that a slave acknowledges 
  * his address. If an acknowledge is sent on the bus, one of the following events will 
  * be set:
  * 
  *  1) In case of Master Receiver (7-bit addressing): the I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED 
  *     event is set.
  *  
  *  2) In case of Master Transmitter (7-bit addressing): the I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED 
  *     is set
  *  
  *  3) In case of 10-Bit addressing mode, the master (just after generating the START 
  *  and checking on EV5) has to send the header of 10-bit addressing mode (I2C_SendData() 
  *  function). Then master should wait on EV9. It means that the 10-bit addressing 
  *  header has been correctly sent on the bus. Then master should send the second part of 
  *  the 10-bit address (LSB) using the function I2C_Send7bitAddress(). Then master 
  *  should wait for event EV6. 
  *     
  */

/* --EV6 */
//#define  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED        ((uint32_t)0x00070082)  /* BUSY, MSL, ADDR, TXE and TRA flags */
#define	I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED		(((I2C_SR2_MSL | I2C_SR2_BUSY | I2C_SR2_TRA) << 16) | (I2C_SR1_TXE | I2C_SR1_ADDR))
//#define  I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED           ((uint32_t)0x00030002)  /* BUSY, MSL and ADDR flags */
#define	I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED			(((I2C_SR2_MSL | I2C_SR2_BUSY) << 16) | (I2C_SR1_ADDR))
/* --EV9 */
//#define  I2C_EVENT_MASTER_MODE_ADDRESS10                   ((uint32_t)0x00030008)  /* BUSY, MSL and ADD10 flags */
#define	I2C_EVENT_MASTER_MODE_ADDRESS10					(((I2C_SR2_MSL | I2C_SR2_BUSY) << 16) | (I2C_SR1_ADD10))

/** 
  * @brief Communication events
  * 
  * If a communication is established (START condition generated and slave address 
  * acknowledged) then the master has to check on one of the following events for 
  * communication procedures:
  *  
  * 1) Master Receiver mode: The master has to wait on the event EV7 then to read 
  *    the data received from the slave (I2C_ReceiveData() function).
  * 
  * 2) Master Transmitter mode: The master has to send data (I2C_SendData() 
  *    function) then to wait on event EV8 or EV8_2.
  *    These two events are similar: 
  *     - EV8 means that the data has been written in the data register and is 
  *       being shifted out.
  *     - EV8_2 means that the data has been physically shifted out and output 
  *       on the bus.
  *     In most cases, using EV8 is sufficient for the application.
  *     Using EV8_2 leads to a slower communication but ensure more reliable test.
  *     EV8_2 is also more suitable than EV8 for testing on the last data transmission 
  *     (before Stop condition generation).
  *     
  *  @note In case the  user software does not guarantee that this event EV7 is 
  *        managed before the current byte end of transfer, then user may check on EV7 
  *        and BTF flag at the same time (ie. (I2C_EVENT_MASTER_BYTE_RECEIVED | I2C_FLAG_BTF)).
  *        In this case the communication may be slower.
  * 
  */

/* Master RECEIVER mode -----------------------------*/ 
/* --EV7 */
//#define  I2C_EVENT_MASTER_BYTE_RECEIVED                    ((uint32_t)0x00030040)  /* BUSY, MSL and RXNE flags */
#define	I2C_EVENT_MASTER_BYTE_RECEIVED					(((I2C_SR2_MSL | I2C_SR2_BUSY) << 16) | (I2C_SR1_RXNE))

/* Master TRANSMITTER mode --------------------------*/
/* --EV8 */
//#define I2C_EVENT_MASTER_BYTE_TRANSMITTING                 ((uint32_t)0x00070080) /* TRA, BUSY, MSL, TXE flags */
#define	I2C_EVENT_MASTER_BYTE_TRANSMITTING		(((I2C_SR2_MSL | I2C_SR2_BUSY | I2C_SR2_TRA) << 16) | (I2C_SR1_TXE))
/* --EV8_2 */
//#define  I2C_EVENT_MASTER_BYTE_TRANSMITTED                 ((uint32_t)0x00070084)  /* TRA, BUSY, MSL, TXE and BTF flags */
#define	I2C_EVENT_MASTER_BYTE_TRANSMITTED		(((I2C_SR2_MSL | I2C_SR2_BUSY | I2C_SR2_TRA) << 16) | (I2C_SR1_TXE | I2C_SR1_BTF))


/*
 ===============================================================================
                          End of Events Description
 ===============================================================================
 */
#define WDELAYCONST	2550

static uint_fast8_t i2c_waitforevent(uint_fast32_t event)
{
	unsigned w;
	w = WDELAYCONST;
	while (w --)
	{
		// порядок чтения регистров важен.
		const uint_fast32_t sr1 = I2C1->SR1;
		const uint_fast32_t sr2 = I2C1->SR2;

		if ((((sr2 << 16) | sr1) & event) == event)	// Все биты должны появиться
			return 0;
		if ((sr2 & (I2C_SR1_BERR | I2C_SR1_ARLO | I2C_SR1_AF | I2C_SR1_OVR | I2C_SR1_PECERR | I2C_SR1_TIMEOUT)) != 0)
			break;
	}
	//PRINTF(PSTR("i2c_waitforevent timeout (event=%" PRIxFAST32 ")\n"), event);
	//for (;;)
	//	;
	return 1;
}

// Ideas taken from https://github.com/elliottt/stm32f4/blob/master/stm32f4/src/i2c.c

/**
 * Generate an I2C START condition.  Per the reference manual, we wait
 * for the hardware to clear the start bit after setting it before
 * allowing any further writes to CR1.  This prevents random lockups.
 */
static void i2c_set_start(void) 
{
	unsigned w;
	w = WDELAYCONST;
	I2C1->CR1 |= I2C_CR1_START;
	while (w -- && (I2C1->CR1 & I2C_CR1_START) != 0)
		;
}

/**
 * Generate an I2C STOP condition.  Per the reference manual, we wait
 * for the hardware to clear the stop bit after setting it before
 * allowing any further writes to CR1.  This prevents random lockups.
 */
static void i2c_set_stop(void) 
{
	unsigned w;
	w = WDELAYCONST;
	I2C1->CR1 |= I2C_CR1_STOP;
	while (w -- && (I2C1->CR1 & I2C_CR1_STOP) != 0)
		;
}

static void i2c_send7bitaddress(uint_fast8_t addr)
{
	I2C1->DR = addr;
}

static void i2c_acknowledgeconfig(uint_fast8_t state)
{
	if (state != 0)
	{
		I2C1->CR1 |= I2C_CR1_ACK;
	}
	else
	{
		I2C1->CR1 &= ~ I2C_CR1_ACK;
	}
}

// start write
/* char */ void i2c_start(
	uint_fast8_t address
	) 
{
b:
		//TP();
	i2c_set_start();
	if (i2c_waitforevent(I2C_EVENT_MASTER_MODE_SELECT))	// I2C_SR1_SB
		goto b;
	i2c_send7bitaddress(address);
	if ((address & 0x01) != 0)
	{
		i2c_waitforevent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);	// I2C_SR1_ADDR
		i2c_acknowledgeconfig(1);
	}
	else
	{
		i2c_waitforevent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	// I2C_SR1_ADDR
	}
	//TP();
}

void i2c_read(uint8_t *data, uint_fast8_t ack_type)
{
	//PRINTF(PSTR("i2c_read trapped\n"));
	//for (;;)
	//	;
	//TP();
	switch (ack_type)
	{
	case I2C_READ_ACK_NACK:	/* чтение первого и единственного байта ответа */
		{
			i2c_acknowledgeconfig(0);

			//TP();
			// Чтение единственного (первого и последнего) байта.
			i2c_waitforevent(I2C_EVENT_MASTER_BYTE_RECEIVED);	// I2C_SR1_RXNE
			* data = I2C1->DR & 0xFF; // cчитываем
			//TP();
			i2c_set_stop();
		}
		break;

	case I2C_READ_NACK:
		{
			// Чтение последнего байта.
			// Дождаться окончания перехода в состояние STOP

			i2c_acknowledgeconfig(0);
			//TP();
			i2c_waitforevent(I2C_EVENT_MASTER_BYTE_RECEIVED);	// I2C_SR1_RXNE
			* data = I2C1->DR & 0xFF; // cчитываем
			//TP();
			i2c_set_stop();
		}
		break;

	case I2C_READ_ACK_1:	// первый байт в последующем чтении
			I2C1->CR1 |= I2C_CR1_ACK; // после приема байта, ведомый посылает подтверждение приема, и устанавливает бит ACK. Очищаем бит ACK и ,если нужно, ждем приема следующего символа
	case I2C_READ_ACK:
		{
			// первый и последующие байты в последовательном чтении.
			//TP();
			i2c_waitforevent(I2C_EVENT_MASTER_BYTE_RECEIVED);	// I2C_SR1_RXNE
			//TP();
			* data = I2C1->DR & 0xFF; // cчитываем
			//I2C1->CR1 |= I2C_CR1_ACK; // после приема байта, ведомый посылает подтверждение приема, и устанавливает бит ACK. Очищаем бит ACK и ,если нужно, ждем приема следующего символа
		}
		break;

	}
	//TP();
	return; // 0;
}

/* char */ void i2c_write(
	uint_fast8_t databyte
	)
{
	i2c_waitforevent(I2C_EVENT_MASTER_BYTE_TRANSMITTING);
	I2C1->DR = databyte;
}

// запись, после чего restart
/* char */ void i2c_write_withrestart(const i2cp_t * p, uint_fast8_t data)
{
	i2c_writeX(p, data);
	i2c_waitsendX(p);
}


// Вызвать после последнего i2c_write()
void i2c_waitsendX(const i2cp_t * p)
{

}

void i2c_stopX(const i2cp_t * p)
{
	i2c_waitforevent(I2C_EVENT_MASTER_BYTE_TRANSMITTED);	// I2C_SR1_BTF
	i2c_set_stop();
}

void i2c_initialize(void)
{
	const i2cp_t * const p1 = & i2cp_1;
	const i2cp_t * const p2 = & i2cp_2;
	hardware_twi_master_configure();
#if WITHTWIHW
	TWIHARD_INITIALIZE();
#endif
}

#elif CPUSTYLE_STM32F7XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F0XX

/*
 * Taken from http://seriouslyembedded.com/stm32f7-i2c-hal/
 * By Joel Bodenmann July 22, 2015 
 */		

/*
 * The CR2 register needs atomic access. Hence always use this function to setup a transfer configuration.
 */
static void stm32f7xxx_i2cConfigTransfer(I2C_TypeDef* i2c, uint16_t slaveAddr, uint8_t numBytes, uint32_t mode, uint32_t request)
{
	uint32_t tmpreg = 0;

	// Get the current CR2 register value
	tmpreg = i2c->CR2;

	// Clear tmpreg specific bits
	tmpreg &= (uint32_t) ~((uint32_t) (I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP));

	// update tmpreg
	tmpreg |= 
		(uint32_t) (
			((uint32_t) slaveAddr & I2C_CR2_SADD) | 
			(((uint32_t) numBytes << 16) & I2C_CR2_NBYTES) | 
			(uint32_t) mode | 
			(uint32_t) request |
			0
		);

	// Update the actual CR2 contents
	i2c->CR2 = tmpreg;
}

/*
 * According to the STM32Cube HAL the CR2 register needs to be reset after each transaction.
 */
static void stm32f7xxx_i2cResetCr2(I2C_TypeDef* i2c)
{
	i2c->CR2 &= (uint32_t) ~((uint32_t) (I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN));
}


void i2cSend(I2C_TypeDef* i2c, uint8_t slaveAddr, uint8_t* data, uint16_t length)
{
	// We are currently not able to send more than 255 bytes at once
	if (length > 255) {
		return;
	}

	// Setup the configuration
	stm32f7xxx_i2cConfigTransfer(i2c, slaveAddr, length, (!I2C_CR2_RD_WRN) | I2C_CR2_AUTOEND, I2C_CR2_START);

	// Transmit the whole buffer
	while (length > 0) {
		while (!(i2c->ISR & I2C_ISR_TXIS));
		i2c->TXDR = *data++;
		length--;
	}

	// Wait until the transfer is complete
	while (!(i2c->ISR & I2C_ISR_TXE))
		;

	// Wait until the stop condition was automagically sent
	while (!(i2c->ISR & I2C_ISR_STOPF))
		;

	// Reset the STOP bit
	i2c->ISR &= ~I2C_ISR_STOPF;

	// Reset the CR2 register
	stm32f7xxx_i2cResetCr2(i2c);
}

void i2cSendByte(I2C_TypeDef* i2c, uint8_t slaveAddr, uint8_t data)
{
	i2cSend(i2c, slaveAddr, &data, 1);
}

void i2cWriteReg(I2C_TypeDef* i2c, uint8_t slaveAddr, uint8_t regAddr, uint8_t value)
{
	uint8_t txbuf[2];
	txbuf[0] = regAddr;
	txbuf[1] = value;

	i2cSend(i2c, slaveAddr, txbuf, 2);
}

void i2cRead(I2C_TypeDef* i2c, uint8_t slaveAddr, uint8_t* data, uint16_t length)
{
	// We are currently not able to read more than 255 bytes at once
	if (length > 255) {
		return;
	}

	// Setup the configuration
	stm32f7xxx_i2cConfigTransfer(i2c, slaveAddr, length, I2C_CR2_RD_WRN | I2C_CR2_AUTOEND, I2C_CR2_START);
	int i;
	// Transmit the whole buffer
	for (i = 0; i < length; i++) {
		while (!(i2c->ISR & I2C_ISR_RXNE));
		data[i] = i2c->RXDR;
	}

	// Wait until the stop condition was automagically sent
	while (!(i2c->ISR & I2C_ISR_STOPF));

	// Reset the STOP bit
	i2c->ISR &= ~I2C_ISR_STOPF;

	// Reset the CR2 register
	stm32f7xxx_i2cResetCr2(i2c);
}

uint8_t i2cReadByte(I2C_TypeDef* i2c, uint8_t slaveAddr, uint8_t regAddr)
{
	uint8_t ret = 0xAA;

	i2cSend(i2c, slaveAddr, &regAddr, 1);
	i2cRead(i2c, slaveAddr, &ret, 1);

	return ret;
}

uint16_t i2cReadWord(I2C_TypeDef* i2c, uint8_t slaveAddr, uint8_t regAddr)
{
	uint8_t ret[2] = { 0xAA, 0xAA };

	i2cSend(i2c, slaveAddr, &regAddr, 1);
	i2cRead(i2c, slaveAddr, ret, 2);

	return (uint16_t)((ret[0] << 8) | (ret[1] & 0x00FF));
}

void i2c_initialize(void)
{
	const i2cp_t * const p1 = & i2cp_1;
	const i2cp_t * const p2 = & i2cp_2;
	TWISOFT_INITIALIZE();
	i2c_softbusrelease();
	
	hardware_twi_master_configure();
#if WITHTWIHW
	TWIHARD_INITIALIZE();
#endif
}




static uint_fast8_t i2c_waittxready(void)
{
	for (;;)
	{
		const portholder_t isr = I2C1->ISR;
		if ((isr & I2C_ISR_TCR) != 0)
			break;
		if ((isr & I2C_ISR_TIMEOUT) != 0)
		{
			//PRINTF(PSTR("i2c_waittxready: bus error ISR: I2C_ISR_TIMEOUT\n"));
			I2C1->ICR = I2C_ICR_TIMOUTCF;
			return 1;
		}
		if ((isr & I2C_ISR_NACKF) != 0)
		{
			//PRINTF(PSTR("i2c_waittxready: bus error ISR: I2C_ISR_NACKF\n"));
			I2C1->ICR = I2C_ICR_NACKCF;
			return 1;
		}
		//PRINTF(PSTR("i2c_waittxready: waiting: ISR=%08lX\n"), isr);
	}
	return 0;
}

static uint_fast8_t i2c_waitrxready(void)
{
	for (;;)
	{
		const portholder_t isr = I2C1->ISR;
		if ((isr & I2C_ISR_RXNE) != 0)
			break;
		if ((isr & I2C_ISR_TIMEOUT) != 0)
		{
			//PRINTF(PSTR("i2c_waitrxready: bus error ISR: I2C_ISR_TIMEOUT\n"));
			I2C1->ICR = I2C_ICR_TIMOUTCF;
			return 1;
		}
		if ((isr & I2C_ISR_NACKF) != 0)
		{
			//PRINTF(PSTR("i2c_waitrxready: bus error ISR: I2C_ISR_NACKF\n"));
			I2C1->ICR = I2C_ICR_NACKCF;
			return 1;
		}
		//PRINTF(PSTR("i2c_waitrxready: waiting: ISR=%08lX\n"), isr);
	}
	return 0;
}

static uint_fast8_t i2c_sended = 0;	// Признак того, что предавали данные
// start write
void i2c_start(
	uint_fast8_t address
	) 
{
	//if (i2c_sended != 0)
	//{
	//	if (i2c_waittxready() != 0)
	//		return;
	//}

	// Setup the configuration
	//stm32f7xxx_i2cConfigTransfer(I2C1, address, 1, I2C_CR2_RD_WRN * ((address & 0x01) != 0) | I2C_CR2_RELOAD, I2C_CR2_START);
	{
		uint32_t tmpreg = 0;

		// Get the current CR2 register value
		tmpreg = I2C1->CR2;

		// Clear tmpreg specific bits
		tmpreg &= (uint32_t) ~((uint32_t) (I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP));

		// update tmpreg
		tmpreg |= 
			(uint32_t) (
				((uint32_t) address & I2C_CR2_SADD) | 
				(((uint32_t) 1 << 16) & I2C_CR2_NBYTES) | 
				(uint32_t) I2C_CR2_RD_WRN * ((address & 0x01) != 0) | 
				(uint32_t) I2C_CR2_RELOAD | 
				(uint32_t) I2C_CR2_START |
				0
			);

		// Update the actual CR2 contents
		I2C1->CR2 = tmpreg;
	}

	// Ожидание окончания формирования START CONDITION и передачи адреса
	while ((I2C1->CR2 & I2C_CR2_START) != 0)
		; //PRINTF(PSTR("i2c_start: waiting, address=%02x\n"), address);

	if ((I2C1->ISR & (I2C_ISR_NACKF | I2C_ISR_TIMEOUT)) != 0)
	{
		//PRINTF(PSTR("i2c_start: bus error ISR: %08lx\n"), I2C1->ISR);
		I2C1->ICR = (I2C_ICR_NACKCF | I2C_ICR_TIMOUTCF);
	}
	i2c_sended = 0;
}

static void i2cmakestop(void)
{
	// Формируем STOP
	I2C1->CR2 = (I2C1->CR2 & ~ (I2C_CR2_NBYTES | I2C_CR2_RELOAD)) |
		//(((uint32_t) 1 << 16) & I2C_CR2_NBYTES) | 
		I2C_CR2_STOP |
		0;
	// Wait until the stop condition was automagically sent
	while ((I2C1->ISR & I2C_ISR_STOPF) == 0)
		; //PRINTF(PSTR("i2cmakestop: waiting\n"));
	// Reset the STOPF bit
	I2C1->ICR = I2C_ICR_STOPCF;
}

void i2c_read(uint8_t *data, uint_fast8_t ack_type)
{
	//PRINTF(PSTR("i2c_read: ack_type=%08lX\n"), ack_type);
	switch (ack_type)
	{
	case I2C_READ_ACK_NACK:	/* чтение первого и единственного байта ответа */
		{
			if (i2c_waitrxready() != 0)
				return;
			* data = I2C1->RXDR & I2C_RXDR_RXDATA;

			i2cmakestop();

			// Reset the CR2 register
			//stm32f7xxx_i2cResetCr2(I2C1);
			I2C1->CR2 &= (uint32_t) ~((uint32_t) (I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN));
		}
		break;

	case I2C_READ_NACK:	// Чтение последнего байта.
		{
			if (i2c_waitrxready() != 0)
				return;
			* data = I2C1->RXDR & I2C_RXDR_RXDATA;

			i2cmakestop();

			// Reset the CR2 register
			//stm32f7xxx_i2cResetCr2(I2C1);
			I2C1->CR2 &= (uint32_t) ~((uint32_t) (I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN));
		}
		break;

	case I2C_READ_ACK_1:	// первый байт в последующем чтении
		{
			if (i2c_waitrxready() != 0)
				return;
			* data = I2C1->RXDR & I2C_RXDR_RXDATA;
			I2C1->CR2 |= I2C_CR2_RELOAD;
		}
		break;

			
	case I2C_READ_ACK:		// последующие байты в последовательном чтении.
		{
			if (i2c_waitrxready() != 0)
				return;
			* data = I2C1->RXDR & I2C_RXDR_RXDATA;
			I2C1->CR2 |= I2C_CR2_RELOAD;
		}
		break;

	}
	//PRINTF(PSTR("i2c_read done: ack_type=%08lX\n"), ack_type);
}

void i2c_write(
	uint_fast8_t databyte
	)
{
	if (i2c_sended != 0)
	{
		if (i2c_waittxready() != 0)
			return;
		I2C1->CR2 = (I2C1->CR2 & ~ (I2C_CR2_NBYTES)) |
			(((uint32_t) 1 << 16) & I2C_CR2_NBYTES) | 
			//I2C_CR2_RELOAD |
			0;
	}

	I2C1->TXDR = databyte;

	i2c_sended = 1;
}

// запись, после чего restart
/* char */ void i2c_write_withrestart(uint_fast8_t data)	
{
	i2c_write(data);
	i2c_waitsend();
	
}

// Вызвать после последнего i2c_write()
void i2c_waitsend(void)
{
	if (i2c_sended != 0)
	{
		if (i2c_waittxready() != 0)
			return;
	}

}

// Вызовом этой функции завершается цепочка записей. При чтении используется другой способ.
void i2c_stop(void)
{
	i2cmakestop();

	// Reset the CR2 register
	//stm32f7xxx_i2cResetCr2(I2C1);
	I2C1->CR2 &= (uint32_t) ~((uint32_t) (I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN));
}

#elif CPUSTYLE_STM32F30X

#define WDELAYCONST	25500

// start write
/* char */ void i2c_start(
	uint_fast8_t address
	) 
{
	I2C1->CR1 = I2C_CR1_PE; // генерирую старт
	I2C1->CR2 = I2C_CR2_START;
	while ((I2C1->ISR & I2C_ISR_SB) == 0) // жду окончания генерации старт И ТУТ ИНОГДА ЗАВИСОН !!!!!
		local_delay_us(1);
	I2C1->TXDR = (I2C1->TXDR & ~ I2C_TXDR_TXDATA) | (address & I2C_TXDR_TXDATA); // передаю адрес ведомого
	unsigned w;
	w = WDELAYCONST;
	while (w -- && (I2C1->SR1 & I2C_SR1_ADDR) == 0)	 // жду окончания передачи адреса
		local_delay_us(1);
	(void) I2C1->SR2;	// reset I2C_SR1_ADDR flag

	w = WDELAYCONST;
	while (w -- && (I2C1->CR2 & I2C_CR2_START) != 0)
		local_delay_us(1);
}

void i2c_read(uint8_t *data, uint_fast8_t ack_type)
{
	switch (ack_type)
	{
	case I2C_READ_ACK_NACK:	/* чтение первого и единственного байта ответа */
		{
			I2C1->CR1 = I2C_CR1_PE; // после приема байта, ведомый посылает подтверждение приема, и устанавливает бит ACK. Очищаем бит ACK и ,если нужно, ждем приема следующего символа
			I2C1->CR2 = I2C_CR2_STOP;
			// Чтение единственного (первого и последнего) байта.
			unsigned w;
			w = WDELAYCONST;
			while (w -- && (I2C1->ISR & I2C_ISR_RXNE) == 0) // ожидаем окончания приема данных
				local_delay_us(1);
			* data = I2C1->RXDR & I2C_RXDR_RXDATA; // cчитываем
			w = WDELAYCONST;
			while (w -- && (I2C1->CR2 & I2C_CR2_STOP) != 0)
				local_delay_us(1);
		}
		break;

	case I2C_READ_NACK:
		{
			// Чтение последнего байта.
			// Дождаться окончания перехода в состояние STOP
			I2C1->CR1 = I2C_CR1_PE; // после приема байта, ведомый посылает подтверждение приема, и устанавливает бит ACK. Очищаем бит ACK и ,если нужно, ждем приема следующего символа
			I2C1->CR2 = I2C_CR2_STOP;
			unsigned w;
			w = WDELAYCONST;
			while (w -- && (I2C1->ISR & I2C_ISR_RXNE) == 0) // ожидаем окончания приема данных
				local_delay_us(1);
			* data = I2C1->RXDR & I2C_RXDR_RXDATA; // cчитываем
			w = WDELAYCONST;
			while (w -- && (I2C1->CR2 & I2C_CR2_STOP) != 0)
				local_delay_us(1);
		}
		break;

	case I2C_READ_ACK_1:	// первый байт в последующем чтении
			I2C1->CR1 |= I2C_CR1_ACK; // после приема байта, ведомый посылает подтверждение приема, и устанавливает бит ACK. Очищаем бит ACK и ,если нужно, ждем приема следующего символа
	case I2C_READ_ACK:
		{
			// первый и последующие байты в последовательном чтении.
			unsigned w;
			w = WDELAYCONST;
			while (w -- && (I2C1->ISR & I2C_ISR_RXNE) == 0) // ожидаем окончания приема данных
				local_delay_us(1);
			* data = I2C1->RXDR & I2C_RXDR_RXDATA; // cчитываем
			//I2C1->CR1 |= I2C_CR1_ACK; // после приема байта, ведомый посылает подтверждение приема, и устанавливает бит ACK. Очищаем бит ACK и ,если нужно, ждем приема следующего символа
		}
		break;

	}
	return; // 0;
}

/* char */ void i2c_write(
	uint_fast8_t databyte
	)
{
	unsigned w;
	w = WDELAYCONST;
	while (w -- && (I2C1->ISR & I2C_ISR_TXE) == 0) // жду окончания передачи байта
		local_delay_us(1);
	I2C1->TXDR = (I2C1->TXDR & ~ I2C_TXDR_TXDATA) | (databyte & I2C_TXDR_TXDATA); // передаю данные
}

// запись, после чего restart
/* char */ void i2c_write_withrestart(uint_fast8_t data)
{
	i2c_write(data);
	i2c_waitsend();
}


void i2c_stop(void)
{
	unsigned w;
	w = WDELAYCONST;
	while (w -- && (I2C1->SR1 & I2C_SR1_BTF) == 0) // жду окончания передачи байта
		local_delay_us(1);
	I2C1->CR1 = I2C_CR1_PE; // формирование сигнала "Стоп", заканчиваем прием
	I2C1->CR2 = I2C_CR2_STOP;
}


//TARGET_TWI_PORT_C	GPIOB->BRR
//TARGET_TWI_PORT_S	GPIOB->BSRR
//TARGET_TWI_PIN		GPIOB->IDR
//TARGET_TWI_TWCK		GPIO_ODR_ODR6
//TARGET_TWI_TWD		GPIO_ODR_ODR7

void i2c_initialize(void)
{
	const i2cp_t * const p1 = & i2cp_1;
	const i2cp_t * const p2 = & i2cp_2;
	
	hardware_twi_master_configure();
#if WITHTWIHW
	TWIHARD_INITIALIZE();
#endif
}

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

// On ARM CORTEX M-3 machines

// start write
/* char */ void i2c_start(
	uint_fast8_t address
	) 
{
	if (address & 0x01)
	{	
		TWI0->TWI_MMR = TWI_MMR_IADRSZ_NONE | TWI_MMR_DADR(address / 2) | TWI_MMR_MREAD;
	}
	else
	{
		TWI0->TWI_MMR = TWI_MMR_IADRSZ_NONE | TWI_MMR_DADR(address / 2);
	}
}


// Вызвать после последнего i2c_write()
void i2c_waitsend(void)
{

}

void i2c_stop(void)
{
	
    TWI0->TWI_CR = TWI_CR_STOP;
	unsigned w = 255;
	while (w -- && (TWI0->TWI_SR & TWI_SR_TXCOMP) == 0)
		local_delay_us(1);
}

/* char */ void i2c_write(
	uint_fast8_t byte
	)
{
	unsigned w = 255;
	while (w -- && (TWI0->TWI_SR & TWI_SR_TXRDY) == 0)
		local_delay_us(1);
    TWI0->TWI_THR = byte;
}

// запись, после чего restart
/* char */ void i2c_write_withrestart(uint_fast8_t data)
{
	i2c_write(data);
	i2c_waitsend();
}
	
void i2c_read(uint8_t *data, uint_fast8_t ack_type)
{
	switch (ack_type)
	{
	case I2C_READ_ACK_NACK:	/* чтение первого и единственного байта ответа */
		{
			// Чтение единственного (первого и последнего) байта.
			TWI0->TWI_CR = TWI_CR_START | TWI_CR_STOP;
			// Дождаться окончания перехода в состояние STOP
			unsigned w = 255;
			while (w -- && (TWI0->TWI_SR & TWI_SR_TXCOMP) == 0)	
				local_delay_us(1);
			* data = TWI0->TWI_RHR;
		}
		break;

	case I2C_READ_NACK:
		{
			// Чтение последнего байта.
			// Дождаться окончания перехода в состояние STOP
			TWI0->TWI_CR = TWI_CR_STOP;
			unsigned w = 255;
			while ( w -- && (TWI0->TWI_SR & TWI_SR_TXCOMP) == 0)
				local_delay_us(1);
			* data = TWI0->TWI_RHR;
		}
		break;

	case I2C_READ_ACK_1:
			TWI0->TWI_CR = TWI_CR_START;
	case I2C_READ_ACK:
		{
			// первый и последующие байты в последовательном чтении.
			unsigned w = 255;
			while (w -- && (TWI0->TWI_SR & TWI_SR_RXRDY) == 0)
				local_delay_us(1);
			* data = TWI0->TWI_RHR;
		}
		break;

	}
	return; // 0;
}

void i2c_initialize(void)
{
	const i2cp_t * const p1 = & i2cp_1;
	const i2cp_t * const p2 = & i2cp_2;
	TWISOFT_INITIALIZE();
	i2c_softbusrelease();
    // Configure clock
	hardware_twi_master_configure();
	// 
#if WITHTWIHW
	TWIHARD_INITIALIZE();
#endif
}

#elif CPUSTYLE_ATXMEGA

void i2c_initialize(void)
{
	const i2cp_t * const p1 = & i2cp_1;
	const i2cp_t * const p2 = & i2cp_2;
	TWISOFT_INITIALIZE();
	i2c_softbusrelease();

	hardware_twi_master_configure();
#if WITHTWIHW
	TWIHARD_INITIALIZE();
#endif
}
// start write
/* char */ void i2c_start(
	uint_fast8_t address
	) 
{
	TARGET_TWI.MASTER.ADDR = address;
}

// Вызвать после последнего i2c_write()
void i2c_waitsend(void)
{

}

void i2c_stop(void)
{
	unsigned w = 255;
	while (w -- && (TARGET_TWI.MASTER.STATUS & TWI_MASTER_WIF_bm) == 0)
		local_delay_us(1);
	TARGET_TWI.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc; 
}
/* char */ void i2c_write(
	uint_fast8_t data
	)
{
	unsigned w = 255;
	while (w -- && (TARGET_TWI.MASTER.STATUS & TWI_MASTER_WIF_bm) == 0)
		local_delay_us(1);
	TARGET_TWI.MASTER.DATA = data;
}

// запись, после чего restart
/* char */ void i2c_write_withrestart(uint_fast8_t data)
{
	unsigned w = 255;
	while (w -- && (TARGET_TWI.MASTER.STATUS & TWI_MASTER_WIF_bm) == 0)
		local_delay_us(1);
	TARGET_TWI.MASTER.DATA = data;
	TARGET_TWI.MASTER.CTRLC = TWI_MASTER_CMD_REPSTART_gc; 	// next command - i2c_start
}
	
void i2c_read(uint8_t *data, uint_fast8_t ack_type)
{
	unsigned w = 255;
	while (w -- && (TARGET_TWI.MASTER.STATUS & TWI_MASTER_RIF_bm) == 0)
		local_delay_us(1);
	* data = TARGET_TWI.MASTER.DATA;

	switch (ack_type)
	{
	case I2C_READ_ACK_1:
		TARGET_TWI.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_RECVTRANS_gc;
		break;
	case I2C_READ_ACK:
		TARGET_TWI.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_RECVTRANS_gc; 
		break;
	case I2C_READ_NACK:
		TARGET_TWI.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc; 
		break;
	case I2C_READ_ACK_NACK:	/* чтение первого и единственного байта ответа */
		TARGET_TWI.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc; 
		* data = TARGET_TWI.MASTER.DATA;
		break;
	}
}

#elif LINUX_SUBSYSTEM

#elif (CPUSTYLE_XC7Z) && WITHTWIHW

#include "xc7z_inc.h"
static XIicPs xc7z_iicps;

void hardware_iicps_configure(void)
{
	XIicPs_Config *Config = XIicPs_LookupConfig(XPAR_XIICPS_0_DEVICE_ID);
	XIicPs_CfgInitialize(& xc7z_iicps, Config, Config->BaseAddress);

	int Status = XIicPs_SelfTest(& xc7z_iicps);
	if (Status != XST_SUCCESS)
	{
		//PRINTF("iicps init error %d\n", Status);
		ASSERT(0);
	}
	XIicPs_SetSClk(& xc7z_iicps, 100000);
	XIicPs_SetOptions(& xc7z_iicps, XIICPS_7_BIT_ADDR_OPTION);
	XIicPs_ClearOptions(& xc7z_iicps, XIICPS_10_BIT_ADDR_OPTION | XIICPS_SLAVE_MON_OPTION | XIICPS_REP_START_OPTION);
}

/* return non-zero then error */
int i2chw_read(uint16_t slave_address, uint8_t * buf, uint32_t size)
{
	while (XIicPs_BusIsBusy(& xc7z_iicps)) { }

	int Status = XIicPs_MasterRecvPolled(& xc7z_iicps, buf, size, slave_address >> 1);
	if (Status != XST_SUCCESS)
		PRINTF("iicps receive error %d from address %x\n", Status, slave_address);

	return Status != XST_SUCCESS;
}

/* return non-zero then error */
int i2chw_write(uint16_t slave_address, const uint8_t * buf, uint32_t size)
{
	while (XIicPs_BusIsBusy(& xc7z_iicps)) { }

	int Status = XIicPs_MasterSendPolled(& xc7z_iicps, (uint8_t *) buf, size, slave_address >> 1);
	if (Status != XST_SUCCESS)
		PRINTF("iicps write error %d to address %x\n", Status, slave_address);

	return Status != XST_SUCCESS;
}

void i2c_initialize(void)
{
	const i2cp_t * const p1 = & i2cp_1;
	const i2cp_t * const p2 = & i2cp_2;
#if 0
	// программирование выводов, управляющих I2C
	TWISOFT_INITIALIZE();
	i2c_softbusrelease();

#ifdef TWISOFT2_INITIALIZE

	TWISOFT2_INITIALIZE();
	i2c2_softbusrelease();

#endif

#endif

#if defined (TWIHARD_INITIALIZE)
	TWIHARD_INITIALIZE();
#endif /* defined (TWIHARD_INITIALIZE) */

	hardware_twi_master_configure();	// clocks - pass XPAR_XIICPS_0_DEVICE_ID
	hardware_iicps_configure();			// Peripheral
}

#elif (CPUSTYLE_ALLWINNER)



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
//	uintptr_t virt;
	TWI_TypeDef * io;
//	char * clk;
//	int reset;
//	int sda;
//	int sdacfg;
//	int scl;
//	int sclcfg;
}i2c_t113_pdata_t;

struct i2c_msg_t {
	int addr;
	int len;
	void * buf;
};

static void t113_i2c_set_rate(struct i2c_t113_pdata_t * pdat, uint64_t rate){
	uint64_t pclk = TWIHARD_FREQ;	//clk_get_rate(pdat->clk);
	uint_fast64_t freq, delta, best = 0x7fffffffffffffffLL;
	int tm = 5, tn = 0;
	int m, n;

	for(n = 0; n <= 7; n++)
	{
		for(m = 0; m <= 15; m++)
		{
			freq = pclk / (10 * (m + 1) * (1 << n));
			delta = rate - freq;
			if (delta >= 0 && delta < best)
			{
				tm = m;
				tn = n;
				best = delta;
			}
			if (best == 0)
				break;
		}
	}
	pdat->io->TWI_CCR = ((tm & 0xf) << 3) | ((tn & 0x7) << 0);
}

//const uint32_t timeout1;

static int t113_i2c_wait_status(struct i2c_t113_pdata_t * pdat){
	const uint32_t timeout = sys_now()+5*10;	//sys_now()+5мс
	do {
		if ((pdat->io->TWI_CNTR & (1 << 3))){
			unsigned int stat = pdat->io->TWI_STAT;
			//PRINTF("t113_i2c_wait_status1 = 0x%02X\n", stat);
			return stat;
		}
	} while (sys_now()<timeout);

	//I2C_ERROR_COUNT++;
	// PRINTF("t113_i2c_wait_status = I2C_STAT_BUS_ERROR\n");
	return I2C_STAT_BUS_ERROR;
}

static int t113_i2c_start(struct i2c_t113_pdata_t * pdat){
	//PRINTF("I2C start\n");
	pdat->io->TWI_CNTR |= (1u << 5) | (1u << 3);
	const uint32_t timeout = sys_now()+5*100;	//sys_now()+5мс
	do {
		if (!(pdat->io->TWI_CNTR & (1 << 5)))	// M_STA
		{
			//PRINTF("I2C start ok\n");
			return t113_i2c_wait_status(pdat);
		}
	} while (sys_now()<timeout);
	//PRINTF("I2C start out\n");
	return t113_i2c_wait_status(pdat);
}

static int t113_i2c_stop(struct i2c_t113_pdata_t * pdat){
	uint32_t val;

	val = pdat->io->TWI_CNTR;
	val |= (1 << 4) | (1 << 3);
	pdat->io->TWI_CNTR = val;
	const uint32_t timeout = sys_now()+5*10;
	do {
		if (!(pdat->io->TWI_CNTR & (1 << 4)))
		{
			return 0;
		}
	} while (sys_now()<timeout);
	return 1;
}

static int t113_i2c_send_data(struct i2c_t113_pdata_t * pdat, uint8_t dat)
{
	pdat->io->TWI_DATA = dat;
	pdat->io->TWI_CNTR |= (1 << 3);

	return t113_i2c_wait_status(pdat);
}

static int t113_i2c_read(struct i2c_t113_pdata_t * pdat, struct i2c_msg_t * msg){
	uint8_t * p = msg->buf;
	int len = msg->len;

	if (t113_i2c_send_data(pdat, (uint8_t)((msg->addr << 1) | 1)) != I2C_STAT_TX_AR_ACK)
		return -1;
	if (len == 0)	/* Handle zero count for probes */
		return 0;
	pdat->io->TWI_CNTR = pdat->io->TWI_CNTR | (1 << 2);
	while (len > 0){
		if (len == 1){
			pdat->io->TWI_CNTR = (pdat->io->TWI_CNTR & ~(1 << 2)) | (1 << 3);
			if (t113_i2c_wait_status(pdat) != I2C_STAT_RXD_NAK)
				return -1;
		}else{
			pdat->io->TWI_CNTR = pdat->io->TWI_CNTR | (1 << 3);
			if (t113_i2c_wait_status(pdat) != I2C_STAT_RXD_ACK)
				return -1;
		}
		*p++ = pdat->io->TWI_DATA;
		len--;
	}
	return 0;
}

static int t113_i2c_write(struct i2c_t113_pdata_t * pdat, struct i2c_msg_t * msg)
{
	uint8_t * p = msg->buf;
	int len = msg->len;

	if (t113_i2c_send_data(pdat, (uint8_t)(msg->addr << 1)) != I2C_STAT_TX_AW_ACK){
		return -1;
	}
	if (len == 0)	/* Handle zero count for probes */
		return 0;
	while (len > 0)
	{
		if (t113_i2c_send_data(pdat, *p++) != I2C_STAT_TXD_ACK)
			return -1;
		len--;
	}
	return 0;
}
//
//unsigned char I2C_WriteByte(unsigned char slaveAddr, const unsigned char* pBuffer, unsigned char WriteAddr){
//	//записываем адрес который хотим прочитать
//	unsigned char wr_buf[2];
//	int res;
//
//	wr_buf[0] = WriteAddr;
//	wr_buf[1] = pBuffer[0];
//
//	struct i2c_msg_t  msgs;
//	msgs.addr = slaveAddr;
//	msgs.len = 2;
//	msgs.buf = wr_buf;
//	//генереруем старт
//	if (t113_i2c_start(&pdat_i2c) != I2C_STAT_TX_START){
//		 //PRINTF("I2C start error\n");
//		return 0;
//	}
//	 //PRINTF("I2C start ok\n");
//
//	res = t113_i2c_write(&pdat_i2c, &msgs);
//	if (res!=0){
//		 //PRINTF("I2C write err\n");
//		return 0;
//	}else{
//		 //PRINTF("I2C write ok\n");
//	}
//	 //PRINTF("I2C t113_i2c_stop in\n");
//	t113_i2c_stop(&pdat_i2c);
//	 //PRINTF	("I2C t113_i2c_stop ok\n");
//
//	return 1;
//}


//// возвращает 1 если все хорошо и 0 если что-то не так
//unsigned char I2C_ReadBuffer(unsigned char slaveAddr, unsigned char* pBuffer, unsigned char ReadAddr, unsigned short NumByteToRead){
//	int res;
//	//PRINTF("!!!!!=====I2C_ReadBuffer=====!!!!!\n");
//	//записываем адрес который хотим прочитать
//	struct i2c_msg_t  msgs;
//	msgs.addr = slaveAddr;
//	msgs.len = 1;
//	msgs.buf = &ReadAddr;
//
//	//генереруем старт
//
//	res = t113_i2c_start(&pdat_i2c);
//	if (res != I2C_STAT_TX_START){
//		//PRINTF("I2C start error\n");
//		t113_i2c_stop(&pdat_i2c);
//		return 0;
//	}
//	//PRINTF("I2C start ok\n");
//
//	res = t113_i2c_write(&pdat_i2c, &msgs);
//	if (res!=0){
//		//PRINTF("I2C write err\n");
//		return 0;
//	}
//	//PRINTF("I2C write ok\n");
//
//	if (t113_i2c_start(&pdat_i2c) != I2C_STAT_TX_RSTART){	//генерируем рестарт
//		//PRINTF("I2C restart error\n");
//		return 0;
//	}
//	//PRINTF("I2C restart Ok\n");
//	//читаем регистр
//	msgs.addr = slaveAddr;
//	msgs.len = NumByteToRead;
//	msgs.buf = pBuffer;
//
//	res = t113_i2c_read(&pdat_i2c, &msgs);
//	if (res!=0){
//		//PRINTF("I2C read err\n");
//		//I2C_ERROR = 0x04;
//		return 0;
//	}
//	//PRINTF("I2C read ok\n");
///**/
//	//PRINTF("I2C t113_i2c_stop in\n");
//	t113_i2c_stop(&pdat_i2c);
//	//PRINTF("I2C t113_i2c_stop ok\n");
//
//	return 1;
//}
//
//// возвращает 1 если все хорошо и 0 если что-то не так
//unsigned char I2C_WriteBuffer(unsigned char slaveAddr, const unsigned char* pBuffer, unsigned char ReadAddr, unsigned short NumByteToWrite){
//	int res;
//	//PRINTF("!!!!!=====I2C_WriteBuffer=====!!!!!\n");
//	//записываем адрес который хотим прочитать
//	struct i2c_msg_t  msgs;
//	msgs.addr = slaveAddr;
//	msgs.len = 1;
//	msgs.buf = &ReadAddr;
//
//	//генереруем старт
//
//	res = t113_i2c_start(&pdat_i2c);
//	if (res != I2C_STAT_TX_START){
//		//PRINTF("I2C start error\n");
//		t113_i2c_stop(&pdat_i2c);
//		return 0;
//	}
//	//PRINTF("I2C start ok\n");
//
//	res = t113_i2c_write(&pdat_i2c, &msgs);
//	if (res!=0){
//		//PRINTF("I2C write err\n");
//		return 0;
//	}
//	//PRINTF("I2C write ok\n");
//
//	if (t113_i2c_start(&pdat_i2c) != I2C_STAT_TX_RSTART){	//генерируем рестарт
//		//PRINTF("I2C restart error\n");
//		return 0;
//	}
//	//PRINTF("I2C restart Ok\n");
//	//читаем регистр
//	msgs.addr = slaveAddr;
//	msgs.len = NumByteToWrite;
//	msgs.buf = (void *) pBuffer;
//
//	res = t113_i2c_write(&pdat_i2c, &msgs);
//	if (res!=0){
//		//PRINTF("I2C write err\n");
//		//I2C_ERROR = 0x04;
//		return 0;
//	}
//	//PRINTF("I2C write ok\n");
///**/
//	//PRINTF("I2C t113_i2c_stop in\n");
//	t113_i2c_stop(&pdat_i2c);
//	//PRINTF("I2C t113_i2c_stop ok\n");
//
//	return 1;
//}

static i2c_t113_pdata_t pdat_i2c;

/* return non-zero then error */
// LSB of slave_address8b ignored */
int i2chw_read(uint16_t slave_address8b, uint8_t * buf, uint32_t size)
{
	struct i2c_t113_pdata_t * const pdat = & pdat_i2c;
	int res;
	struct i2c_msg_t  msgs;
	msgs.addr = slave_address8b >> 1;
	msgs.len = size;
	msgs.buf = (void *) buf;

	res = t113_i2c_start(pdat);
	if (res != I2C_STAT_TX_START) {
		//PRINTF("i2chw_read start error\n");
		t113_i2c_stop(&pdat_i2c);
		return 1;
	}
	res = t113_i2c_read(pdat, &msgs);
	if (res!=0) {
		//PRINTF("i2chw_read read err\n");
		//I2C_ERROR = 0x04;
//		pdat->io->TWI_CNTR &= ~ (1u << 4);
		pdat->io->TWI_SRST |= 1u << 0;
		while ((pdat->io->TWI_SRST & (1u << 0)) != 0)
			;
		return 1;
	}
	t113_i2c_stop(pdat);
	return 0;
}

/* return non-zero then error */
// LSB of slave_address8b ignored */
int i2chw_write(uint16_t slave_address8b, const uint8_t * buf, uint32_t size)
{
	struct i2c_t113_pdata_t *const  pdat = & pdat_i2c;
	int res;
	struct i2c_msg_t  msgs;
	msgs.addr = slave_address8b >> 1;
	msgs.len = size;
	msgs.buf = (void *) buf;

	res = t113_i2c_start(pdat);
	if (res != I2C_STAT_TX_START) {
		//PRINTF("i2chw_write start error\n");
		t113_i2c_stop(pdat);
		return 1;
	}
	res = t113_i2c_write(pdat, &msgs);
	if (res!=0) {
		//PRINTF("i2chw_write write err\n");
		//I2C_ERROR = 0x04;
//		pdat->io->TWI_CNTR &= ~ (1u << 4);
		pdat->io->TWI_SRST |= 1u << 0;
		while ((pdat->io->TWI_SRST & (1u << 0)) != 0)
			;
		return 1;
	}
	t113_i2c_stop(pdat);
	return 0;
}

void i2c_initialize(void)
{
	TWISOFT_INITIALIZE();
	i2c_softbusrelease();

	hardware_twi_master_configure();
	TWIHARD_INITIALIZE();

	t113_i2c_stop(&pdat_i2c);
}

#else
	#error I2C hardware implementation for CPUSTYLE_xxx is not avaliable

#endif // CPUSTYLE_ATMEGA

#endif

#if WITHTWISW

// программно-реализованный I2C интерфейс

//you'll need to change for a different processor.

//#define SCL     TRISB4 // I2C bus
//#define SDA     TRISB1 //
//#define SCL_IN  RB4    //
//#define SDA_IN  RB1    //

//To initialize the ports set the output resisters to 0 and the tristate registers to 1 
// which disables the outputs and allows them to be pulled high by the resistors.
//SDA = SCL = 1;
//SCL_IN = SDA_IN = 0;
void i2c_initialize(void)
{
	const i2cp_t * const p1 = & i2cp_1;
	const i2cp_t * const p2 = & i2cp_2;
	// программирование выводов, управляющих I2C
	TWISOFT_INITIALIZE();

#if 0
	i2c_softbusrelease();

#endif

	SET_TWD();
	i2c_delay(p2);
	SET_TWCK();
	i2c_delay(p2);

#ifdef TWISOFT2_INITIALIZE

	TWISOFT2_INITIALIZE();
	i2c2_softbusrelease();

#endif

#if WITHTWIHW
	TWIHARD_INITIALIZE();
	i2chw_initialize();
#endif /* WITHTWIHW */
}


//The following 4 functions provide the primitive start, stop, read and write sequences. 
// All I2C transactions can be built up from these.

// i2c start bit sequence
// negative edge on SD while SCL is HIGH
void i2c_startX(const i2cp_t * p, uint_fast8_t address)
{
	             
	SET_TWD();	//SDA = 1;
	i2c_delay(p);
	SET_TWCK();	//SCL = 1;
	i2c_delay(p);
	CLR_TWD();	//SDA = 0;// negative edge on SCL
	i2c_delay(p);
	CLR_TWCK();	//SCL = 0;
	i2c_delay(p);

	i2c_writeX(p, address);
	//return 0;
}

// Вызвать после последнего i2c_write()
void i2c_waitsendX(const i2cp_t * p)
{

}

// i2c stop bit sequence
void i2c_stopX(const i2cp_t * p)
{
	CLR_TWD();//SDA = 0;  
	i2c_delay(p);
	SET_TWCK();//SCL = 1;
	i2c_delay(p);
	SET_TWD();//SDA = 1;
	i2c_delay(p);

}

void i2c_readX(const i2cp_t * p, uint8_t *data, uint_fast8_t ack_type)
{ 
	char x, d=0;
	SET_TWD(); 
	i2c_delay(p);
	for (x = 0; x < 8; x ++)
	{
		int n;
		n = 10000;
		do {
			SET_TWCK();
			//i2c_delay(p);
		} while (n -- && GET_TWCK() == 0);    // wait for any SCL clock stretching
		i2c_delay(p);
		d = d * 2 + (GET_TWD() != 0);
		CLR_TWCK();
		i2c_delay(p);
	} 

	switch (ack_type)
	{
	case I2C_READ_ACK_1:
	case I2C_READ_ACK:
		CLR_TWD();
		i2c_delay(p);
		break;
	case I2C_READ_NACK:
	case I2C_READ_ACK_NACK:	/* чтение первого и единственного байта ответа */
		SET_TWD();
		i2c_delay(p);
		break;
	}

	SET_TWCK();
	i2c_delay(p);             // send (N)ACK bit
	CLR_TWCK();
	SET_TWD();
	i2c_delay(p);
	* data = d;
	//return 0;
}

void i2c_writeX(const i2cp_t * p, uint_fast8_t d)
{
	uint_fast8_t x;
	//char b;
	for (x = 8; x != 0; x --, d <<= 1)
	{
		if (d & 0x80) 
		{
			//SDA = 1;
			SET_TWD();
		}
		else 
		{
			//SDA = 0;
			CLR_TWD();
		}
		/* Формирование импульса на SCL */
		i2c_delay(p);
		//SCL = 1;
		SET_TWCK();
		//SCL = 0;
		i2c_delay(p);
		CLR_TWCK();
		i2c_delay(p);
	}
	
	//SDA = 1;
	SET_TWD();
    i2c_delay(p);
	//SCL = 1;
	SET_TWCK();
	i2c_delay(p);
	//b = SDA_IN;          // possible ACK bit
	//b = 0;
	//SCL = 0;
	CLR_TWCK();
	i2c_delay(p);
	//return b;
}

// запись, после чего restart
/* char */ void i2c_write_withrestartX(const i2cp_t * p, uint_fast8_t data)
{
	i2c_writeX(p, data);
	i2c_waitsendX(p);
}

#ifdef TWISOFT2_INITIALIZE


//The following 4 functions provide the primitive start, stop, read and write sequences.
// All I2C transactions can be built up from these.

// i2c start bit sequence
// negative edge on SD while SCL is HIGH
void i2c2_start(uint_fast8_t address)
{

	SET2_TWD();	//SDA = 1;
	i2c_delay(p);
	SET2_TWCK();	//SCL = 1;
	i2c_delay(p);
	CLR2_TWD();	//SDA = 0;// negative edge on SCL
	i2c_delay(p);
	CLR2_TWCK();	//SCL = 0;
	i2c_delay(p);

	i2c2_write(address);
	//return 0;
}

// Вызвать после последнего i2c_write()
void i2c2_waitsend(void)
{

}

// i2c stop bit sequence
void i2c2_stop(void)
{
	CLR2_TWD();//SDA = 0;
	i2c_delay(p);
	SET2_TWCK();//SCL = 1;
	i2c_delay(p);
	SET2_TWD();//SDA = 1;
	i2c_delay(p);

}

void i2c2_read(uint8_t *data, uint_fast8_t ack_type)
{
	char x, d=0;
	SET2_TWD();
	i2c_delay(p);
	for (x = 0; x < 8; x ++)
	{
		int n;
		n = 1000;
		do {
			SET2_TWCK();
			i2c_delay(p);
		} while (n -- && GET2_TWCK() == 0);    // wait for any SCL clock stretching
		i2c_delay(p);
		d = d * 2 + (GET2_TWD() != 0);
		CLR2_TWCK();
		i2c_delay(p);
	}

	switch (ack_type)
	{
	case I2C_READ_ACK_1:
	case I2C_READ_ACK:
		CLR2_TWD();
		i2c_delay(p);
		break;
	case I2C_READ_NACK:
	case I2C_READ_ACK_NACK:	/* чтение первого и единственного байта ответа */
		SET2_TWD();
		i2c_delay(p);
		break;
	}

	SET2_TWCK();
	i2c_delay(p);             // send (N)ACK bit
	CLR2_TWCK();
	SET2_TWD();
	i2c_delay(p);
	* data = d;
	//return 0;
}

void i2c2_write(uint_fast8_t d)
{
	uint_fast8_t x;
	//char b;
	for (x = 8; x != 0; x --, d <<= 1)
	{
		if (d & 0x80)
		{
			//SDA = 1;
			SET2_TWD();
		}
		else
		{
			//SDA = 0;
			CLR2_TWD();
		}
		/* Формирование импульса на SCL */
		i2c_delay(p);
		//SCL = 1;
		SET2_TWCK();
		//SCL = 0;
		i2c_delay(p);
		CLR2_TWCK();
		i2c_delay(p);
	}

	//SDA = 1;
	SET2_TWD();
    i2c_delay(p);
	//SCL = 1;
	SET2_TWCK();
	i2c_delay(p);
	//b = SDA_IN;          // possible ACK bit
	//b = 0;
	//SCL = 0;
	CLR2_TWCK();
	i2c_delay(p);
	//return b;
}

// запись, после чего restart
/* char */ void i2c2_write_withrestart(uint_fast8_t data)
{
	i2c2_write(data);
	i2c2_waitsend();
}
#endif

#endif /* WITHTWISW */

#endif /* WITHTWIHW || WITHTWISW */

#if WITHTWIHW

void hardware_twi_master_configure(void)
{
#if CPUSTYLE_ATMEGA

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, SCL_CLOCK * 2) - 8, ATMEGA_TWBR_WIDTH, ATMEGA_TWBR_TAPS, & value, 0);

	TWSR = prei; 	/* prescaler */
	TWBR = value;
	TWCR = (1U << TWEN);

#elif CPUSTYLE_AT91SAM7S

	AT91C_BASE_PMC->PMC_PCER = (1UL << AT91C_ID_TWI) | (1UL << AT91C_ID_PIOA);
	//
    // Reset the TWI
    AT91C_BASE_TWI->TWI_CR = AT91C_TWI_SWRST;
    (void) AT91C_BASE_TWI->TWI_RHR;

    // Set master mode
    AT91C_BASE_TWI->TWI_CR = AT91C_TWI_MSEN;
	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, SCL_CLOCK * 2) - 3, AT91SAM7_TWI_WIDTH, AT91SAM7_TWI_TAPS, & value, 0);

    AT91C_BASE_TWI->TWI_CWGR = (prei << 16) | (value << 8) | value;

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1UL << ID_TWI0);	 // разрешить тактированние этого блока
	//
    // Reset the TWI
    TWI0->TWI_CR = TWI_CR_SWRST;
    (void) TWI0->TWI_RHR;

    // Set master mode
    TWI0->TWI_CR = TWI_CR_MSEN;
	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, SCL_CLOCK * 2) - 4, ATSAM3S_TWI_WIDTH, ATSAM3S_TWI_TAPS, & value, 1);
	//prei = 0;
	//value = 70;
    TWI0->TWI_CWGR = TWI_CWGR_CKDIV(prei) | TWI_CWGR_CHDIV(value) | TWI_CWGR_CLDIV(value);

#elif CPUSTYLE_ATXMEGA

	TARGET_TWI.MASTER.BAUD = ((CPU_FREQ / (2 * SCL_CLOCK)) - 5);
	TARGET_TWI.MASTER.CTRLA = TWI_MASTER_ENABLE_bm;
  	//TARGET_TWI.MASTER.CTRLB = TWI_MASTER_SMEN_bm;
	TARGET_TWI.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	//конфигурирую непосредствено I2С
	RCC->APB1ENR |= (RCC_APB1ENR_I2C1EN); //вкл тактирование контроллера I2C
	(void) RCC->APB1ENR;

	I2C1->CR1 |= I2C_CR1_SWRST;
	I2C1->CR1 &= ~ I2C_CR1_SWRST;
	I2C1->CR1 &= ~ I2C_CR1_PE; // все конфигурации необходимо проводить только со сброшеным битом PE

/*
	The FREQ bits must be configured with the APB clock frequency value (I2C peripheral
	connected to APB). The FREQ field is used by the peripheral to generate data setup and
	hold times compliant with the I2C specifications. The minimum allowed frequency is 2 MHz,
	the maximum frequency is limited by the maximum APB frequency (42 MHz) and an intrinsic
	limitation of 46 MHz.

*/

	I2C1->CR2 = (I2C1->CR2 & ~ (I2C_CR2_FREQ)) |
		((I2C_CR2_FREQ_0 * 42) & I2C_CR2_FREQ) |
		0;
	// (1000 ns / 125 ns = 8 + 1)
	// (1000 ns / 22 ns = 45 + 1)
	I2C1->TRISE = 46; //время установления логического уровня в количестве цыклах тактового генератора I2C

	I2C1->CCR = (I2C1->CCR & ~ (I2C_CCR_CCR | I2C_CCR_FS | I2C_CCR_DUTY)) |
		(calcdivround2(BOARD_I2C_FREQ, SCL_CLOCK * 25) & I2C_CCR_CCR) |	// Делитель для получения 10 МГц (400 кHz * 25)
	#if SCL_CLOCK == 400000UL
		I2C_CCR_FS |
		I2C_CCR_DUTY | // T high = 9 * CCR * TPCLK1, T low = 16 * CCR * TPCLK1: full cycle = 25 * CCR * TPCLK1
	#endif /* SCL_CLOCK == 400000UL */
		0;

	I2C1->CR1 |= I2C_CR1_ACK | I2C_CR1_PE; // включаю тактирование переферии I2С

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	//конфигурирую непосредствено I2С
	RCC->APB1ENR |= (RCC_APB1ENR_I2C1EN); //вкл тактирование контроллера I2C
	(void) RCC->APB1ENR;
    // set I2C1 clock to PCLCK (72/64/36 MHz)
    RCC->CFGR3 |= RCC_CFGR3_I2C1SW;		// PCLK1_FREQ or PCLK2_FREQ (PCLK of this BUS, PCLK1) selected as I2C spi clock source


	I2C1->CR1 &= ~ I2C_CR1_PE; // все конфигурации необходимо проводить только со сброшеным битом PE

	//I2C1->CR2 = (I2C1->CR2 & ~I2C_CR2_FREQ) | I2C_CR2_FREQ_0 * ( PCLK2_FREQ / SCL_CLOCK); // частота тактирования модуля I2C1 до делителя равна FREQ_IN
	//I2C1->CR2 = I2C_CR2_FREQ_0 * 4; //255; // |= I2C_CR2_FREQ;	// debug

	I2C1->TIMINGR = (I2C1->TIMINGR & ~ I2C_TIMINGR_PRESC) | (4UL << 28);

	//I2C1->CCR &= ~I2C_CCR_CCR;
	//I2C1->CCR |= (1000/(2*40000)) * ((I2C1->CR2&I2C_CR2_FREQ) / I2C_CR2_FREQ_0); // конечный коэффциент деления
	////I2C1->CCR = 40; //36; // / 4;	//|= I2C_CCR_CCR;	// debug
	//I2C1->CCR |= I2C_CCR_FS;

	//I2C1->TRISE = 9; //время установления логического уровня в количестве циклов тактового генератора I2C

    // disable analog filter
    I2C1->CR1 |= I2C_CR1_ANFOFF;
    // from stm32f3_i2c_calc.py (400KHz, 125ns rise/fall time, no AF/DFN)
    const uint_fast8_t sdadel = 7;
    const uint_fast8_t scldel = 5;
    I2C1->TIMINGR = 0x30000C19 | ((scldel & 0x0F) << 20) | ((sdadel & 0x0F) << 16);

	I2C1->CR1 |= I2C_CR1_PE; // включаю тактирование периферии I2С

#elif CPUSTYLE_STM32F7XX
	//конфигурирую непосредствено I2С
	RCC->APB1ENR |= (RCC_APB1ENR_I2C1EN); //вкл тактирование контроллера I2C
	(void) RCC->APB1ENR;

	// Disable the I2Cx peripheral
	I2C1->CR1 &= ~ I2C_CR1_PE;
	while ((I2C1->CR1 & I2C_CR1_PE) != 0)
		;

	// Set timings. Asuming I2CCLK is 50 MHz (APB1 clock source)
	I2C1->TIMINGR =
		//0x00912732 |		// Discovery BSP code from ST examples
		0x00913742 |		// подобрано для 400 кГц
		4 * (1uL << I2C_TIMINGR_PRESC_Pos) |			// prescaler, was: 0
		0;



	// Use 7-bit addresses
	I2C1->CR2 &= ~ I2C_CR2_ADD10;

	// Enable auto-end mode
	//I2C1->CR2 |= I2C_CR2_AUTOEND;

	// Disable the analog filter
	I2C1->CR1 |= I2C_CR1_ANFOFF;

	// Disable NOSTRETCH
	I2C1->CR1 |= I2C_CR1_NOSTRETCH;

	// Enable the I2Cx peripheral
	I2C1->CR1 |= I2C_CR1_PE;

#elif CPUSTYLE_STM32H7XX
	//конфигурирую непосредствено I2С
	RCC->APB1LENR |= (RCC_APB1LENR_I2C1EN); //вкл тактирование контроллера I2C
	(void) RCC->APB1LENR;

	// Disable the I2Cx peripheral
	I2C1->CR1 &= ~ I2C_CR1_PE;
	while ((I2C1->CR1 & I2C_CR1_PE) != 0)
		;

	// Set timings. Asuming I2CCLK is 50 MHz (APB1 clock source)
	I2C1->TIMINGR =
		//0x00912732 |		// Discovery BSP code from ST examples
		0x00913742 |		// подобрано для 400 кГц
		4 * (1uL << I2C_TIMINGR_PRESC_Pos) |			// prescaler, was: 0
		0;



	// Use 7-bit addresses
	I2C1->CR2 &= ~ I2C_CR2_ADD10;

	// Enable auto-end mode
	//I2C1->CR2 |= I2C_CR2_AUTOEND;

	// Disable the analog filter
	I2C1->CR1 |= I2C_CR1_ANFOFF;

	// Disable NOSTRETCH
	I2C1->CR1 |= I2C_CR1_NOSTRETCH;

	// Enable the I2Cx peripheral
	I2C1->CR1 |= I2C_CR1_PE;

#elif LINUX_SUBSYSTEM

#elif CPUSTYLE_XC7Z

	unsigned iicix = XPAR_XIICPS_0_DEVICE_ID;
	SCLR->SLCR_UNLOCK = 0x0000DF0DU;
	SCLR->APER_CLK_CTRL |= (0x01uL << (18 + iicix));	// APER_CLK_CTRL.I2C0_CPU_1XCLKACT

#elif CPUSTYLE_A64

	#warning Should be implemented for CPUSTYLE_A64

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_T507 || CPUSTYLE_H616)

	if (0)
	{

	}
#if defined (S_TWI0)
	else if (TWIHARD_PTR == S_TWI0)
	{
		PRCM->R_TWI_BGR_REG |= (UINT32_C(1) << 0);	// Open the clock gate
		PRCM->R_TWI_BGR_REG &= ~ (UINT32_C(1) << 16);	// Assert reset
		PRCM->R_TWI_BGR_REG |= (UINT32_C(1) << 16);	// De-assert reset
	}
#endif /* defined (S_TWI0) */
	else
	{
		const unsigned TWIx = TWIHARD_IX;
		CCU->TWI_BGR_REG |= UINT32_C(1) << (0 + TWIx);	// Open the clock gate
		CCU->TWI_BGR_REG &= ~ (UINT32_C(1) << (16 + TWIx));	// Assert reset
		CCU->TWI_BGR_REG |= UINT32_C(1) << (16 + TWIx);	// De-assert reset
	}

	//pdat_i2c.virt = (uintptr_t) TWIHARD_PTR;
	pdat_i2c.io = TWIHARD_PTR;

	t113_i2c_set_rate(&pdat_i2c, 400000);

	pdat_i2c.io->TWI_CNTR =  1u << 6;	// BUS_EN

	pdat_i2c.io->TWI_SRST |= 1u << 0;
	while ((pdat_i2c.io->TWI_SRST & (1u << 0)) != 0)
		;

#else
	#warning Undefined CPUSTYLE_XXX
#endif
}

#endif /* WITHTWIHW */

#if (WITHTWISW) && ! LINUX_SUBSYSTEM

/* скорость обмена */
void i2cp_intiialize(i2cp_t * p, unsigned ch, unsigned freq)
{
	p->ch = ch;
	p->usdelayv = (1000000 / 2) / freq;

}

/* Версии функций с указанием скорости и порта I2C */
void i2cp_start(const i2cp_t * p, uint_fast8_t address)
{
	switch (p->ch)
	{
	case I2CP_I2C1:
		i2c_startX(p, address);
		break;
#if defined (TWISOFT2_INITIALIZE)
	case I2CP_I2C2:
		i2c2_startX(p, address);
		break;
#endif /* defined (TWISOFT2_INITIALIZE) */
	default:
		ASSERT(0);
		return;
	}
}

/* Версии функций с указанием скорости и порта I2C */
void i2cp_read(const i2cp_t * p, uint8_t * pdata, uint_fast8_t acknak)
{
	switch (p->ch)
	{
	case I2CP_I2C1:
		i2c_readX(p, pdata, acknak);
		break;
#if defined (TWISOFT2_INITIALIZE)
	case I2CP_I2C2:
		i2c2_readX(p, pdata, acknak);
		break;
#endif /* defined (TWISOFT2_INITIALIZE) */
	default:
		ASSERT(0);
		return;
	}
}

/* Версии функций с указанием скорости и порта I2C */
void i2cp_write(const i2cp_t * p, uint_fast8_t data)
{
	switch (p->ch)
	{
	case I2CP_I2C1:
		i2c_writeX(p, data);
		break;
#if defined (TWISOFT2_INITIALIZE)
	case I2CP_I2C2:
		i2c2_writeX(p, data);
		break;
#endif /* defined (TWISOFT2_INITIALIZE) */
	default:
		ASSERT(0);
		return;
	}
}

/* Версии функций с указанием скорости и порта I2C */
void i2cp_write_withrestart(const i2cp_t * p, uint_fast8_t data)	// запись, после чего restart
{
	switch (p->ch)
	{
	case I2CP_I2C1:
		i2c_write_withrestartX(p, data);
		break;
#if defined (TWISOFT2_INITIALIZE)
	case I2CP_I2C2:
		i2c2_write_withrestartX(p, data);
		break;
#endif /* defined (TWISOFT2_INITIALIZE) */
	default:
		ASSERT(0);
		return;
	}
}

/* Версии функций с указанием скорости и порта I2C */
void i2cp_waitsend(const i2cp_t * p)	// Вызвать после последнего i2c_write()
{
	switch (p->ch)
	{
	case I2CP_I2C1:
		i2c_waitsendX(p);
		break;
#if defined (TWISOFT2_INITIALIZE)
	case I2CP_I2C2:
		i2c2_waitsendX(p);
		break;
#endif /* defined (TWISOFT2_INITIALIZE) */
	default:
		ASSERT(0);
		return;
	}
}

/* Версии функций с указанием скорости и порта I2C */
void i2cp_stop(const i2cp_t * p)
{
	switch (p->ch)
	{
	case I2CP_I2C1:
		i2c_stopX(p);
		break;
#if defined (TWISOFT2_INITIALIZE)
	case I2CP_I2C2:
		i2c2_stopX(p);
		break;
#endif /* defined (TWISOFT2_INITIALIZE) */
	default:
		ASSERT(0);
		return;
	}
}


#if 1

/* wrappers for old functions */
void i2c_start(uint_fast8_t address)
{
	i2cp_start(& i2cp_1, address);
}
/* wrappers for old functions */
void i2c_read(uint8_t * data, uint_fast8_t acknak)
{
	i2cp_read(& i2cp_1, data, acknak);
}
/* wrappers for old functions */
void i2c_write(uint_fast8_t data)
{
	i2cp_write(& i2cp_1, data);
}
/* wrappers for old functions */
void i2c_write_withrestart(uint_fast8_t data)	// запись, после чего restart
{
	i2cp_write_withrestart(& i2cp_1, data);
}
/* wrappers for old functions */
void i2c_waitsend(void)	// Вызвать после последнего i2c_write()
{
	i2cp_waitsend(& i2cp_1);
}
/* wrappers for old functions */
void i2c_stop(void)
{
	i2cp_stop(& i2cp_1);
}

#if defined (TWISOFT2_INITIALIZE)

// Работа со вторым каналом I2C
/* wrappers for old functions */
void i2c2_start(uint_fast8_t address)
{
	i2cp_start(& i2cp_2, address);
}
/* wrappers for old functions */
void i2c2_read(uint8_t * data, uint_fast8_t acknak)
{
	i2cp_read(& i2cp_2, data, acknak);
}
/* wrappers for old functions */
void i2c2_write(uint_fast8_t data)
{
	i2cp_write(& i2cp_2, data);
}
/* wrappers for old functions */
void i2c2_write_withrestart(uint_fast8_t data)	// запись, после чего restart
{
	i2cp_write_withrestart(& i2cp_2, data);
}
/* wrappers for old functions */
void i2c2_waitsend(void)	// Вызвать после последнего i2c_write()
{
	i2cp_waitsend(& i2cp_2);
}
/* wrappers for old functions */
void i2c2_stop(void)
{
	i2cp_stop(& i2cp_2);
}
#endif /* defined (TWISOFT2_INITIALIZE) */

#endif

#endif /* (WITHTWISW) && ! LINUX_SUBSYSTEM */
