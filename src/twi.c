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

// Обслуживание I2C без использования аппаратных средств процессора
// программное "ногодрыгание" выводами.

#if CPUSTYLE_ATMEGA
	#define SET_TWCK() do { TARGET_TWI_TWCK_DDR &= ~ TARGET_TWI_TWCK; hardware_spi_io_delay(); } while (0)	// SCL = 1
	#define CLR_TWCK() do { TARGET_TWI_TWCK_DDR |= TARGET_TWI_TWCK; hardware_spi_io_delay(); } while (0)	// SCL = 0
	#define SET_TWD() do { TARGET_TWI_TWD_DDR &= ~ TARGET_TWI_TWD;  hardware_spi_io_delay(); } while (0)	// SDA = 1
	#define CLR_TWD() do { TARGET_TWI_TWD_DDR |= TARGET_TWI_TWD; hardware_spi_io_delay(); } while (0)	// SDA = 0

	#define GET_TWCK() ((TARGET_TWI_TWCK_PIN & TARGET_TWI_TWCK) != 0)
	#define GET_TWD() ((TARGET_TWI_TWD_PIN & TARGET_TWI_TWD) != 0)

#elif CPUSTYLE_ARM || CPUSTYLE_ATXMEGA

	#define SET_TWCK() do { TARGET_TWI_TWCK_PORT_S(TARGET_TWI_TWCK); hardware_spi_io_delay(); } while (0)	// SCL = 1
	#define CLR_TWCK() do { TARGET_TWI_TWCK_PORT_C(TARGET_TWI_TWCK); hardware_spi_io_delay(); } while (0)	// SCL = 0
	#define SET_TWD() do { TARGET_TWI_TWD_PORT_S(TARGET_TWI_TWD); hardware_spi_io_delay(); } while (0)	// SDA = 1
	#define CLR_TWD() do { TARGET_TWI_TWD_PORT_C(TARGET_TWI_TWD); hardware_spi_io_delay(); } while (0)	// SDA = 0

	#define GET_TWCK() ((TARGET_TWI_TWCK_PIN & TARGET_TWI_TWCK) != 0)
	#define GET_TWD() ((TARGET_TWI_TWD_PIN & TARGET_TWI_TWD) != 0)

#else
	#error Undefined CPUSTYLE_xxx
#endif

#if WITHTWIHW

#if CPUSTYLE_ATMEGA

#include <util/twi.h>

void i2c_initialize(void)
{

#if 1
	TWISOFT_INITIALIZE();

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
#endif

	TWISOFT_INITIALIZE();
	hardware_twi_master_configure();
	TWIHARD_INITIALIZE();
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
#if 1
	TWISOFT_INITIALIZE();

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
#endif

	TWISOFT_INITIALIZE();

    // Configure clock
	hardware_twi_master_configure();
	TWIHARD_INITIALIZE();
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
	debug_printf_P(PSTR("i2c_waitforevent timeout (event=%" PRIxFAST32 ")\n"), event);
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
	//debug_printf_P(PSTR("i2c_read trapped\n"));
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
/* char */ void i2c_write_withrestart(uint_fast8_t data)
{
	i2c_write(data);
	i2c_waitsend();
}


// Вызвать после последнего i2c_write()
void i2c_waitsend(void)
{

}

void i2c_stop(void)
{
	i2c_waitforevent(I2C_EVENT_MASTER_BYTE_TRANSMITTED);	// I2C_SR1_BTF
	i2c_set_stop();
}

void i2c_initialize(void)
{
	hardware_twi_master_configure();
	TWIHARD_INITIALIZE();		// соединение периферии с выводами
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
#if 1
	TWISOFT_INITIALIZE();

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
#endif

	TWISOFT_INITIALIZE();
	
	hardware_twi_master_configure();
	TWIHARD_INITIALIZE();
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
			debug_printf_P(PSTR("i2c_waittxready: bus error ISR: I2C_ISR_TIMEOUT\n"));
			I2C1->ICR = I2C_ICR_TIMOUTCF;
			return 1;
		}
		if ((isr & I2C_ISR_NACKF) != 0)
		{
			debug_printf_P(PSTR("i2c_waittxready: bus error ISR: I2C_ISR_NACKF\n"));
			I2C1->ICR = I2C_ICR_NACKCF;
			return 1;
		}
		//debug_printf_P(PSTR("i2c_waittxready: waiting: ISR=%08lX\n"), isr);
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
			debug_printf_P(PSTR("i2c_waitrxready: bus error ISR: I2C_ISR_TIMEOUT\n"));
			I2C1->ICR = I2C_ICR_TIMOUTCF;
			return 1;
		}
		if ((isr & I2C_ISR_NACKF) != 0)
		{
			debug_printf_P(PSTR("i2c_waitrxready: bus error ISR: I2C_ISR_NACKF\n"));
			I2C1->ICR = I2C_ICR_NACKCF;
			return 1;
		}
		debug_printf_P(PSTR("i2c_waitrxready: waiting: ISR=%08lX\n"), isr);
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
		; //debug_printf_P(PSTR("i2c_start: waiting, address=%02x\n"), address);

	if ((I2C1->ISR & (I2C_ISR_NACKF | I2C_ISR_TIMEOUT)) != 0)
	{
		debug_printf_P(PSTR("i2c_start: bus error ISR: %08lx\n"), I2C1->ISR);
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
		; //debug_printf_P(PSTR("i2cmakestop: waiting\n"));
	// Reset the STOPF bit
	I2C1->ICR = I2C_ICR_STOPCF;
}

void i2c_read(uint8_t *data, uint_fast8_t ack_type)
{
	//debug_printf_P(PSTR("i2c_read: ack_type=%08lX\n"), ack_type);
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
	//debug_printf_P(PSTR("i2c_read done: ack_type=%08lX\n"), ack_type);
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
	while(w -- && (I2C1->SR1 & I2C_SR1_ADDR) == 0)	 // жду окончания передачи адреса
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
	
	hardware_twi_master_configure();

	TWIHARD_INITIALIZE();
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
#if 1
	TWISOFT_INITIALIZE();

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
#endif

	TWISOFT_INITIALIZE();

    // Configure clock
	hardware_twi_master_configure();
	// 
	TWIHARD_INITIALIZE();
}

#elif CPUSTYLE_ATXMEGA

void i2c_initialize(void)
{
#if 1
	TWISOFT_INITIALIZE();

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
#endif

	TWISOFT_INITIALIZE();

	hardware_twi_master_configure();
	TWIHARD_INITIALIZE();
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

#else
	#error I2C hardware implementation for CPUSTYPE_xxx is not avaliable

#endif // CPUSTYLE_ATMEGA

#elif WITHTWISW

// программно-реализованный I2C интерфейс

//you'll need to change for a different processor.

//#define SCL     TRISB4 // I2C bus
//#define SDA     TRISB1 //
//#define SCL_IN  RB4    //
//#define SDA_IN  RB1    //

//We use a small delay routine between SDA and SCL changes to give a clear 
// sequence on the I2C bus. This is nothing more than a subroutine call and return.
static void i2c_dly(void)
{
	local_delay_us(10);
#if PCF8576C
	local_delay_us(15);
#endif /* PCF8576C */
}

//To initialize the ports set the output resisters to 0 and the tristate registers to 1 
// which disables the outputs and allows them to be pulled high by the resistors.
//SDA = SCL = 1;
//SCL_IN = SDA_IN = 0;
void i2c_initialize(void)
{
	// программирование выводов, управляющих I2C
	TWISOFT_INITIALIZE();

#if 0
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

#endif

	SET_TWD();
	i2c_dly();
	SET_TWCK();
	i2c_dly();
}


//The following 4 functions provide the primitive start, stop, read and write sequences. 
// All I2C transactions can be built up from these.

// i2c start bit sequence
// negative edge on SD while SCL is HIGH
void i2c_start(uint_fast8_t address)
{
	             
	SET_TWD();	//SDA = 1;
	i2c_dly();
	SET_TWCK();	//SCL = 1;
	i2c_dly();
	CLR_TWD();	//SDA = 0;// negative edge on SCL
	i2c_dly();
	CLR_TWCK();	//SCL = 0;
	i2c_dly();

	i2c_write(address);
	//return 0;
}

// Вызвать после последнего i2c_write()
void i2c_waitsend(void)
{

}

// i2c stop bit sequence
void i2c_stop(void)
{
	CLR_TWD();//SDA = 0;  
	i2c_dly();
	SET_TWCK();//SCL = 1;
	i2c_dly();
	SET_TWD();//SDA = 1;
	i2c_dly();

}

void i2c_read(uint8_t *data, uint_fast8_t ack_type)
{ 
	volatile int n;
	char x, d=0;
	SET_TWD(); 
	i2c_dly();
	for(x=0; x<8; x++) 
	{
		n = 10000;
		do {
			SET_TWCK();
			i2c_dly();
		}
		while (n -- && GET_TWCK() == 0)    // wait for any SCL clock stretching
			;
		d *= 2;
		i2c_dly();
		if (GET_TWD() != 0) 
			d += 1;
		CLR_TWCK();
		i2c_dly();
	} 

	switch (ack_type)
	{
	case I2C_READ_ACK_1:
	case I2C_READ_ACK:
		CLR_TWD();
		i2c_dly();
		break;
	case I2C_READ_NACK:
	case I2C_READ_ACK_NACK:	/* чтение первого и единственного байта ответа */
		SET_TWD();
		i2c_dly();
		break;
	}

	SET_TWCK();
	i2c_dly();             // send (N)ACK bit
	CLR_TWCK();
	SET_TWD();
	i2c_dly();
	* data = d;
	//return 0;
}

void i2c_write(uint_fast8_t d)
{
	uint_fast8_t x;
	//char b;
	for (x = 8; x != 0; x --) 
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
		i2c_dly();
		//SCL = 1;
		SET_TWCK();
		d <<= 1;
		//SCL = 0;
		i2c_dly();
		CLR_TWCK();
		i2c_dly();
	}
	
	//SDA = 1;
	SET_TWD();
    i2c_dly();
	//SCL = 1;
	SET_TWCK();
	i2c_dly();
	//b = SDA_IN;          // possible ACK bit
	//b = 0;
	//SCL = 0;
	CLR_TWCK();
	i2c_dly();
	//return b;
}

// запись, после чего restart
/* char */ void i2c_write_withrestart(uint_fast8_t data)
{
	i2c_write(data);
	i2c_waitsend();
}

#endif /* WITHTWISW */


#endif /* WITHTWIHW || WITHTWISW */

