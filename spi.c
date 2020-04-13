/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include <stdlib.h>
//#include <assert.h>
#include "spi.h"
#include "gpio.h"
#include "formats.h"

// битовые маски, соответствующие биту в байте по его номеру.
const uint_fast8_t rbvalues [8] =
{
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 
};

#if WITHSPIHW || WITHSPISW

	#if ! defined (SPI_ALLCS_BITS)
		#error SPI_ALLCS_BITS should be defined in any cases
	#endif

#if UC1608_CSP

// Выдача единички как чипселект
static void 
spi_select255(void)
{
	#if CPUSTYLE_ARM || CPUSTYLE_ATXMEGA
		UC1608_CSP_PORT_S(UC1608_CSP);
	#elif (CPUSTYLE_ATMEGA)
		UC1608_CSP_PORT |= UC1608_CSP;
	#else
		#error Undefined CPUSTYLE_XXX
	#endif
	hardware_spi_io_delay(); 
}

// снять чипселект
static void 
spi_unselect255(void)
{
	#if CPUSTYLE_ARM || CPUSTYLE_ATXMEGA
		UC1608_CSP_PORT_C(UC1608_CSP);
	#elif (CPUSTYLE_ATMEGA)
		UC1608_CSP_PORT &= ~ UC1608_CSP;
	#else
		#error Undefined CPUSTYLE_XXX
	#endif
	hardware_spi_io_delay(); 
}

static void 
spi_hwinit255(void)
{
	#if CPUSTYLE_ARM || CPUSTYLE_ATXMEGA
		UC1608_CSP_INITIALIZE();
	#elif (CPUSTYLE_ATMEGA)
		UC1608_CSP_PORT &= ~ UC1608_CSP;	/* неактивное состояние */
		UC1608_CSP_DDR |= UC1608_CSP;		/* Выход */
	#else
		#error Undefined CPUSTYLE_XXX
	#endif
}
#endif /* UC1608_CSP */

// SPI chip select inactive
static void
spi_cs_disable(void)
{
#if CPUSTYLE_ARM || CPUSTYLE_ATXMEGA

	#if WITHSPISPLIT	
		/* для двух разных потребителей формируются отдельные сигналы MOSI, SCK, CS */
		SPI0_TARGET_PORT_S(SPI0_CS_BIT);
		SPI1_TARGET_PORT_S(SPI1_CS_BIT);
		#if defined (SPI_CSEL2)
			SPI2_TARGET_PORT_S(SPI2_CS_BIT);
		#endif
	#endif /* WITHSPISPLIT */

	#if SPI_ALLCS_BITS != 0 && SPI_ALLCS_BITSNEG != 0

		SPI_ALLCS_PORT_S(SPI_ALLCS_BITS & (SPI_ALLCS_BITS ^ SPI_ALLCS_BITSNEG));	// Запрещение - все биты чипселектов в 1, ту что активны "1" - в "0".
		SPI_ALLCS_PORT_C(SPI_ALLCS_BITS & SPI_ALLCS_BITSNEG);	// Запрещение - все биты чипселектов в 1, ту что активны "1" - в "0".

	#elif SPI_ALLCS_BITS != 0

		SPI_ALLCS_PORT_S(SPI_ALLCS_BITS);	// Запрещение - все биты чипселектов в 1

	#endif /* defined (SPI_ALLCS_BITS) */

	#if defined (SPI_NAEN_BIT)
		SPI_NAEN_PORT_S(SPI_NAEN_BIT);	// Запрещение чипселектов единицей
	#elif defined (SPI_AEN_BIT)
		SPI_AEN_PORT_C(SPI_AEN_BIT);	// Запрещение чипселектов нулём
	#endif
	hardware_spi_io_delay(); 

#elif (CPUSTYLE_ATMEGA)

	#if WITHSPISPLIT
		/* для двух разных потребителей формируются отдельные сигналы MOSI, SCK, CS */
		SPI0_TARGET_CS_PORT |= SPI0_CS_BIT;
		SPI1_TARGET_CS_PORT |= SPI1_CS_BIT;
		#if defined (SPI_CSEL2)
			SPI2_TARGET_CS_PORT |= SPI2_CS_BIT;
		#endif
	#endif /* WITHSPISPLIT */

	#if SPI_ALLCS_BITS != 0 && SPI_ALLCS_BITSNEG != 0

		SPI_ALLCS_PORT |= (SPI_ALLCS_BITS & (SPI_ALLCS_BITS ^ SPI_ALLCS_BITSNEG));	// Запрещение - все биты чипселектов в 1, ту что активны "1" - в "0".
		SPI_ALLCS_PORT &= ~ (SPI_ALLCS_BITS & SPI_ALLCS_BITSNEG);	// Запрещение - все биты чипселектов в 1, ту что активны "1" - в "0".

	#elif SPI_ALLCS_BITS != 0

		SPI_ALLCS_PORT |= SPI_ALLCS_BITS;	// Все биты чипселектов в 1

	#endif /* defined (SPI_ALLCS_BITS) */

	#if defined (SPI_NAEN_BIT)
		SPI_NAEN_PORT |= SPI_NAEN_BIT;	// Запрещение чипселектов единицей
	#elif defined (SPI_AEN_BIT)
		SPI_AEN_PORT &= ~ SPI_AEN_BIT;	// Запрещение чипселектов нулём
	#endif
	hardware_spi_io_delay(); 
#else

	#error Undefined CPUSTYLE_XXX

#endif
}
// SPI chip select active
static void
spi_cs_enable(
	spitarget_t target	/* addressing to chip */
	)
{
#if CPUSTYLE_ARM || CPUSTYLE_ATXMEGA

	#if WITHSPISPLIT
		/* для двух разных потребителей формируются отдельные сигналы MOSI, SCK, CS */
		switch (target)
		{
		case SPI_CSEL0:
			SPI0_TARGET_PORT_C(SPI0_CS_BIT);
			break;
		case SPI_CSEL1:
			SPI1_TARGET_PORT_C(SPI1_CS_BIT);
			break;
	  #if defined (SPI_CSEL2)
		case SPI_CSEL2:
			SPI2_TARGET_PORT_C(SPI2_CS_BIT);
			break;
	  #endif
		}

	#endif /* WITHSPISPLIT */

	// бездешифраторная схема управления - CS формируются выходами процессора напрямую.
	#if SPI_ALLCS_BITS != 0 && SPI_ALLCS_BITSNEG != 0
		if ((target & SPI_ALLCS_BITSNEG) != 0)
			SPI_ALLCS_PORT_S(target & SPI_ALLCS_BITSNEG);
		else if ((target & SPI_ALLCS_BITS) != 0)
			SPI_ALLCS_PORT_C(target & SPI_ALLCS_BITS);
	#elif SPI_ALLCS_BITS != 0
		if ((target & SPI_ALLCS_BITS) != 0)
			SPI_ALLCS_PORT_C(target);
	#endif /* defined (SPI_ALLCS_BITS) */
	// Управление стробом дешифратора
	#if defined (SPI_NAEN_BIT)
		if ((target & SPI_ALLCS_BITS) == 0)
			SPI_NAEN_PORT_C(SPI_NAEN_BIT);
	#elif defined (SPI_AEN_BIT)
		if ((target & SPI_ALLCS_BITS) == 0)
			SPI_AEN_PORT_S(SPI_AEN_BIT);
	#endif
	hardware_spi_io_delay(); 

#elif (CPUSTYLE_ATMEGA)

	#if WITHSPISPLIT
		/* для двух разных потребителей формируются отдельные сигналы MOSI, SCK, CS */
		switch (target)
		{
		case SPI_CSEL0:
			SPI0_TARGET_CS_PORT &= ~ SPI0_CS_BIT;
			break;
		case SPI_CSEL1:
			SPI1_TARGET_CS_PORT &= ~ SPI1_CS_BIT;
			break;
	  #if defined (SPI_CSEL2)
		case SPI_CSEL2:
			SPI2_TARGET_CS_PORT &= ~ SPI2_CS_BIT;
			break;
	  #endif
		}
	#endif /* WITHSPISPLIT */

	// бездешифраторная схема управления - CS формируются выходами процессора напрямую.
	#if SPI_ALLCS_BITS != 0 && SPI_ALLCS_BITSNEG != 0
		if ((target & SPI_ALLCS_BITSNEG) != 0)
			SPI_ALLCS_PORT |= (target & SPI_ALLCS_BITSNEG);	// установить в "1", если этот выход требует "1" как активное состояние
		else if ((target & SPI_ALLCS_BITS) != 0)
			SPI_ALLCS_PORT &= ~ (SPI_ALLCS_BITS & (target ^ SPI_ALLCS_BITSNEG));
	#elif SPI_ALLCS_BITS != 0
		if ((target & SPI_ALLCS_BITS) != 0)
			SPI_ALLCS_PORT &= ~ target;
	#endif /* defined (SPI_ALLCS_BITS) */

	// Управление стробом дешифратора
	#if defined (SPI_NAEN_BIT)
		if ((target & SPI_ALLCS_BITS) == 0)
			SPI_NAEN_PORT &= ~ SPI_NAEN_BIT;
	#elif defined (SPI_AEN_BIT)
		if ((target & SPI_ALLCS_BITS) == 0)
			SPI_AEN_PORT |= SPI_AEN_BIT;
	#endif
	hardware_spi_io_delay(); 

#else

	#error Undefined CPUSTYLE_XXX

#endif
}

static void 
spi_setaddress(
	spitarget_t target	/* addressing to chip */
	)
{
#if CPUSTYLE_ARM || CPUSTYLE_ATXMEGA

	#if defined (SPI_NAEN_BIT)
		if ((target & SPI_ALLCS_BITS) == 0)
		{
			SPI_ADDRESS_PORT_C(SPI_ADDRESS_BITS & ~ target);
			SPI_ADDRESS_PORT_S(SPI_ADDRESS_BITS & target);
			hardware_spi_io_delay(); 
		}

	#elif defined (SPI_AEN_BIT)
		if ((target & SPI_ALLCS_BITS) == 0)
		{
			SPI_ADDRESS_PORT_C(SPI_ADDRESS_BITS & ~ target);
			SPI_ADDRESS_PORT_S(SPI_ADDRESS_BITS & target);
			hardware_spi_io_delay(); 
		}
	#endif

#elif (CPUSTYLE_ATMEGA)

	#if defined (SPI_NAEN_BIT)
		SPI_ADDRESS_PORT = (SPI_ADDRESS_PORT & ~ SPI_ADDRESS_BITS) | target;
		hardware_spi_io_delay(); 
	#elif defined (SPI_AEN_BIT)
		SPI_ADDRESS_PORT = (SPI_ADDRESS_PORT & ~ SPI_ADDRESS_BITS) | target;
		hardware_spi_io_delay(); 
	#endif

#else

	#error Undefined CPUSTYLE_XXX

#endif
}

/* make negative pulse on ioupdate line */
void 
prog_pulse_ioupdate(void)
{
	// SPI_IOUPDATE_PORT was SPI_TARGET_PORT
#if defined (SPI_IOUPDATE_BIT)
	#if CPUSTYLE_ARM || CPUSTYLE_ATXMEGA

		SPI_IOUPDATE_PORT_C(SPI_IOUPDATE_BIT);
		hardware_spi_io_delay();
		SPI_IOUPDATE_PORT_S(SPI_IOUPDATE_BIT);

	#elif CPUSTYLE_ATMEGA

		SPI_IOUPDATE_PORT &= ~ SPI_IOUPDATE_BIT;
		hardware_spi_io_delay();
		SPI_IOUPDATE_PORT |= SPI_IOUPDATE_BIT;

	#else
		#error Undefined CPUSTYLE_XXX
	#endif
#elif defined (targetupd1)

	// Если для DDS требуется сигнал IOUPDAYE и не выделен бит в порту - управляем тут.
	prog_select(targetupd1);
	prog_unselect(targetupd1);

#endif
}

#if SPI_BIDIRECTIONAL

/* переключение вывода SPI DATA на чтение */
void 
prog_spi_to_read_impl(void)
{
	SPIIO_MOSI_TO_INPUT();
	hardware_spi_io_delay();  
}

/* переключение вывода SPI DATA на выдачу даннах. Состояне после инициализации порта */
void 
prog_spi_to_write_impl(void)
{
	SPIIO_MOSI_TO_OUTPUT();
	hardware_spi_io_delay();  
}

#endif /* SPI_BIDIRECTIONAL */

#if WITHSPISW

#if ! WITHSPISPLIT
	/*
	 * просто выдача тактового сигнала и чтение одного бита
	 */
	static
	uint_fast8_t 
	//RAMFUNC_NONILINE 
	spi_pulse_clk(void)
	{
		SCLK_NPULSE();	/* latch to chips */

		#if SPI_BIDIRECTIONAL
			return (SPI_TARGET_MOSI_PIN & SPI_MOSI_BIT) != 0;
		#else /* SPI_BIDIRECTIONAL */
			return (SPI_TARGET_MISO_PIN & SPI_MISO_BIT) != 0;
		#endif /* SPI_BIDIRECTIONAL */
	}

	//////////////////////////
	// Получение 8 бит с SPI
	uint_fast8_t 
	prog_spi_read_byte_impl(uint_fast8_t bytetosend)
	{
		uint_fast8_t i = 8;
		uint_fast8_t v = 0;
		while (i --)
		{
		#if ! SPI_BIDIRECTIONAL
			SDO_SET(bytetosend & 0x80);		// запись бита информации
		#endif /*  ! SPI_BIDIRECTIONAL */
			v = v * 2 + spi_pulse_clk();	// спадающий перепад на SPI CLK переключает FRAM в режим выдачи
			bytetosend <<= 1;
		}
		return v;
	}
#endif /* ! WITHSPISPLIT */

static 
//RAMFUNC_NONILINE 
uint8_t phase_getbit(
	const phase_t * v,
	uint_fast8_t i		/* bit position, LSB = 0 */
	)
{
	/* ключевое слово const удалено для помещения констант в RAM (на ARM) для повышения быстродействия. */
	static /* const */ uint_fast8_t mask [8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
	//return (((const uint8_t *) v) [i >> 3] & mask [i & 07]);

	const div_t res = div(i, 8);
	return (((const unsigned char *) v) [res.quot] & mask [res.rem]) != 0;
}


/* send bits (starting from MSB) */
void RAMFUNC_NONILINE (prog_phbits_impl)(
	spitarget_t target,	/* addressing to chip */
	const phase_t * val,
	uint_fast8_t i,				/* left of 1-st bit to set */
	uint_fast8_t n				/* number of bits to send */
	)
{
	while (n --)
	{
		prog_bit(target, phase_getbit(val, -- i));
	}
}

void RAMFUNC_NONILINE (prog_val_impl)(
	spitarget_t target,	/* addressing to chip */
	uint_fast8_t value,
	uint_fast8_t n				/* number of bits to send */
	)
{
	uint_fast8_t i;
	uint_fast8_t mask = ((uint_fast8_t) 1) << (n - 1);
	for (i = n; -- i < n; mask >>= 1)
	{
		prog_bit(target, (uint_fast8_t) (value & mask));
	}
}

// выдача 8-ми бит на SPI
void NOINLINEAT (prog_val8_impl)(
	spitarget_t target,	/* addressing to chip */
	uint_fast8_t value
	)
{
	prog_bit(target, (uint_fast8_t) (value & 0x80));
	prog_bit(target, (uint_fast8_t) (value & 0x40));
	prog_bit(target, (uint_fast8_t) (value & 0x20));
	prog_bit(target, (uint_fast8_t) (value & 0x10));
	prog_bit(target, (uint_fast8_t) (value & 0x08));
	prog_bit(target, (uint_fast8_t) (value & 0x04));
	prog_bit(target, (uint_fast8_t) (value & 0x02));
	prog_bit(target, (uint_fast8_t) (value & 0x01));
}

#endif /* WITHSPISW */


// Send a frame of bytes via SPI
void 
prog_spi_send_frame(
	spitarget_t target,
	const uint8_t * buff, 
	unsigned int size
	)
{
	spi_progval8_p1(target, * buff);
	while (-- size)
		spi_progval8_p2(target, * ++ buff);
	spi_complete(target);
}

// Read a frame of bytes via SPI
// На сигнале MOSI при этом должно обеспачиваться состояние логической "1" для корректной работы SD CARD
void 
prog_spi_read_frame(
	spitarget_t target,
	uint8_t * buff, 
	unsigned int size
	)
{
	while (size --)
		* buff ++ = spi_read_byte(target, 0xff);
}

/* 
 * интерфейс с платой - управление чипселектом
 */

void prog_select_impl(
	spitarget_t target	/* SHIFTED addressing to chip (on ATMEGA - may be bit mask) */
	)
{
	spi_to_write(target);	// shifted address
#if UC1608_CSP
	if (target == SPI_CSEL255)
	{
		spi_select255();
		return;
	}
#endif /* UC1608_CSP */
	spi_setaddress(target);	// shifted address
	spi_cs_enable(target);	// chip select active
}

void prog_unselect_impl(void)
{
#if UC1608_CSP
	spi_unselect255();
#endif /* UC1608_CSP */
	spi_cs_disable();	// chip select inactive - and latch in 74HC595
}

/* switch off all chip selects and data enable */

static void 
prog_select_init(void)
{
#if UC1608_CSP
	spi_hwinit255();
#endif /* SPI_CSEL255 */

#if defined (SPI_ALLCS_INITIALIZE)
	SPI_ALLCS_INITIALIZE();		/* инициализация лиий выбора периферийных микросхем */
#endif /* defined (SPI_ALLCS_INITIALIZE) */

#if defined (SPI_ADDRESS_AEN_INITIALIZE)
	SPI_ADDRESS_AEN_INITIALIZE();	/* инициализация сигналов управлдения дешифратором CS */
#endif /* defined (SPI_ADDRESS_AEN_INITIALIZE) */

#if defined (SPI_ADDRESS_NAEN_INITIALIZE)
	SPI_ADDRESS_NAEN_INITIALIZE();	/* инициализация сигналов управлдения дешифратором CS */
#endif /* defined (SPI_ADDRESS_NAEN_INITIALIZE) */

#if defined (SPI_IOUPDATE_INITIALIZE)
	SPI_IOUPDATE_INITIALIZE();
#endif /* defined (SPI_IOUPDATE_INITIALIZE) */

#if defined (SPI_IORESET_INITIALIZE)
	SPI_IORESET_INITIALIZE();
#endif /* defined (SPI_IORESET_INITIALIZE) */

	//spi_to_write(target);
	spi_cs_disable();	// chip select inactive
	//SCLK_SET();	// initial state of SCLK - logical "1" - обеспечивается в hardwate_spi_select_init()
}

/*
	See: http://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith32Bits

	uint_fast8_t b = v & 0xff;
	b = ((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16; 
	spi_progval8_pX(target, b);

*/

// Функция разворота младших восьми бит
uint_fast8_t revbits8(uint_fast8_t v)
{
	uint_fast8_t b = v & 0xff;
	b = ((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16; 
	return b & 0xff;
}

#if 0
/* Выдача байта МЛАДШИМ битом вперёд */
const FLASHMEM unsigned char revbittable [256] = 
{
	0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
	0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
	0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
	0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
	0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
	0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
	0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
	0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
	0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
	0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
	0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
	0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
	0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
	0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
	0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
	0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF,
};
#endif


void spi_initialize(void)
{

#if WITHSPIHW && WITHSPISW

	// программный и аппаратный SPI

	// Для работы Hittite HMC830 в Open mode инициализация требутеn условие "b":
	// a. If a rising edge on SEN is detected first HMC Mode is selected.
	// b. If a rising edge on SCLK is detected first Open mode is selected.

	SPIIO_INITIALIZE();			// основные сигналы SPI при программном управлении
	hardware_spi_master_initialize();
	prog_select_init();		// spi CS initialize

#elif WITHSPIHW

	// только аппаратный SPI

	// Для работы Hittite HMC830 в Open mode инициализация требутеn условие "b":
	// a. If a rising edge on SEN is detected first HMC Mode is selected.
	// b. If a rising edge on SCLK is detected first Open mode is selected.

	hardware_spi_master_initialize();
	prog_select_init();		// spi CS initialize

#elif WITHSPISW

	// только программный SPI

	// Для работы Hittite HMC830 в Open mode инициализация требутеn условие "b":
	// a. If a rising edge on SEN is detected first HMC Mode is selected.
	// b. If a rising edge on SCLK is detected first Open mode is selected.

	SPIIO_INITIALIZE();			// основные сигналы SPI при программном управлении
	prog_select_init();		// spi CS initialize

#endif

#if WITHFPGAWAIT_AS || WITHFPGALOAD_PS || WITHDSPEXTFIR
	hardware_spi_master_setfreq(SPIC_SPEEDUFAST, SPISPEEDUFAST);
#endif /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS || WITHDSPEXTFIR */

	hardware_spi_master_setfreq(SPIC_SPEEDFAST, SPISPEED);

#if defined (SPISPEED400k) || defined (SPISPEED100k)
	hardware_spi_master_setfreq(SPIC_SPEED100k, SPISPEED100k);		// 100 kHz for MICROCHIP MCP3204/MCP3208
	hardware_spi_master_setfreq(SPIC_SPEED400k, SPISPEED400k);
	hardware_spi_master_setfreq(SPIC_SPEED4M, 4000000uL);	/* 4 MHz для CS4272 */
	hardware_spi_master_setfreq(SPIC_SPEED10M, 10000000uL);	/* 10 MHz для ILI9341 */
	hardware_spi_master_setfreq(SPIC_SPEED25M, 25000000uL);	/* 25 MHz  */
#endif /* (SPISPEED400k) || defined (SPISPEED100k) */

}

#endif /* WITHSPIHW || WITHSPISW */



#if WIHSPIDFHW || WIHSPIDFSW

enum { SPDIFIO_READ, SPDIFIO_WRITE };	// в случае пеердачи только команды используем write */

#if WIHSPIDFSW


/////////
// https://github.com/renesas-rz/rza1_qspi_flash/blob/master/qspi_flash.c


// Use block SPIBSC0
// 17. SPI Multi I/O Bus Controller
//

void spidf_initialize(void)
{
	// Connect I/O pins
	SPIDF_SOFTINITIALIZE();
}

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
}

// Connrect I/O pins
static void spidf_select(spitarget_t target, uint_fast8_t mode)
{
	SPIDF_SELECT();
}

static void spidf_unselect(void)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */
	SPIDF_UNSELECT();
}

static void spidf_to_read(spitarget_t target)
{
}

static void spidf_to_write(spitarget_t target)
{
}

static void spidf_progval8_p1(spitarget_t target, uint_fast8_t sendval)
{
	spidf_write_byte(target, sendval);
}

static void spidf_progval8_p2(spitarget_t target, uint_fast8_t sendval)
{
	spidf_write_byte(target, sendval);
}

static uint_fast8_t spidf_complete(spitarget_t target)
{
	return 0;
}

static uint_fast8_t spidf_progval8(spitarget_t target, uint_fast8_t sendval)
{
	return spidf_read_byte(target, sendval);
}

static void spidf_iostart(
	uint_fast8_t direction,	// 0: dataflash-to-memory, 1: Memory-to-dataflash
	uint_fast8_t cmd,
	uint_fast8_t ndummy,	// number of dummy bytes
	uint_fast32_t size,
	uint_fast8_t hasaddress,
	uint_fast32_t address
	)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	spidf_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spidf_progval8_p1(target, cmd);		/* The Read SFDP instruction code is 0x5A */

	if (hasaddress)
	{
		spidf_progval8_p2(target, address >> 16);
		spidf_progval8_p2(target, address >> 8);
		spidf_progval8_p2(target, address >> 0);
	}
	while (ndummy --)
		spidf_progval8_p2(target, 0x00);	// dummy byte

	spidf_complete(target);	/* done sending data to target chip */
}


static void spidf_read(uint8_t * buff, uint_fast32_t size)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */
	spidf_to_read(target);
	while (size --)
		* buff ++ = spidf_read_byte(target, 0xff);
	spidf_to_write(target);
}


static uint_fast8_t spidf_verify(const uint8_t * buff, uint_fast32_t size)
{
	uint_fast8_t err = 0;
	spitarget_t target = targetdataflash;	/* addressing to chip */
	spidf_to_read(target);
	while (size --)
		err |= * buff ++ != spidf_read_byte(target, 0xff);
	spidf_to_write(target);
	return err;
}


static void spidf_write(const uint8_t * buff, uint_fast32_t size)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */
	while (size --)
		 spidf_read_byte(target, * buff ++);
}

#elif WIHSPIDFHW && CPUSTYLE_STM32MP1


// вычитываем все заказанное количество
static void spidf_read(uint8_t * buff, uint_fast32_t size)
{
	while (size --)
	{
		while ((QUADSPI->SR & QUADSPI_SR_FLEVEL_Msk) == 0)
			;
		* buff ++ = * (volatile uint8_t *) & QUADSPI->DR;
	}
}

// передаем все заказанное количество
static void spidf_write(const uint8_t * buff, uint_fast32_t size)
{
	while (size --)
	{
		while ((QUADSPI->SR & QUADSPI_SR_FLEVEL_Msk) == QUADSPI_SR_FLEVEL_Msk)
			;
		* (volatile uint8_t *) & QUADSPI->DR = * buff ++;
	}
}

// вычитываем все заказанное количество
static uint_fast8_t spidf_verify(const uint8_t * buff, uint_fast32_t size)
{
	uint_fast8_t err = 0;
	while (size --)
	{
		while ((QUADSPI->SR & QUADSPI_SR_FLEVEL_Msk) == 0)
			;
		err |= * buff ++ != * (volatile uint8_t *) & QUADSPI->DR;
	}
	return err;
}

static void spidf_unselect(void)
{
	while ((QUADSPI->SR & QUADSPI_SR_BUSY_Msk) != 0)
		;
	// Disconnect I/O pins
	SPIDF_HANGOFF();
}

static void spidf_iostart(
	uint_fast8_t direction,	// 0: dataflash-to-cpu, 1: cpu-to-dataflash
	uint_fast8_t cmd,
	uint_fast8_t ndummy,	// number of dummy bytes
	uint_fast32_t size,
	uint_fast8_t hasaddress,
	uint_fast32_t address
	)
{
	while ((QUADSPI->SR & QUADSPI_SR_BUSY_Msk) != 0)
		;

	// Connect I/O pins
	SPIDF_HARDINITIALIZE();

	//QUADSPI->AR = address;
	QUADSPI->DLR = size ? (size - 1) : 0;

	//PRINTF("QUADSPI->DR=%08x\n", QUADSPI->DR);
	QUADSPI->FCR = QUADSPI_FCR_CTCF_Msk;	// Clear Transfer Complete Flag
	QUADSPI->FCR = QUADSPI_FCR_CTEF_Msk;	// Clear Transfer Error Flag

	QUADSPI->CCR =
		//(0 << QUADSPI_CCR_DDRM_Pos) |	// 0: DDR Mode disabled
		//(0 << QUADSPI_CCR_DHHC_Pos) |	// 0: Delay the data output using analog delay
		//(0 << QUADSPI_CCR_FRCM_Pos) |	// 0: Normal mode
		//(0 << QUADSPI_CCR_SIOO_Pos) |	// 0: Send instruction on every transaction
		((direction ? 0x00 : 0x01) << QUADSPI_CCR_FMODE_Pos) |	// 01: Indirect read mode, 00: Indirect write mode
		//(0x00 << QUADSPI_CCR_FMODE_Pos) |	//
		(size != 0) * (0x01 << QUADSPI_CCR_DMODE_Pos) |	// 01: Data on a single line
		((8 * ndummy)  << QUADSPI_CCR_DCYC_Pos) |	// This field defines the duration of the dummy phase (0..15).
		//(0 << QUADSPI_CCR_ABSIZE_Pos) |	// 00: 8-bit alternate byte
		(0 << QUADSPI_CCR_ABMODE_Pos) |	// 00: No alternate bytes
		(0x02 << QUADSPI_CCR_ADSIZE_Pos) |	// 010: 24-bit address
		(hasaddress != 0) * (0x01 << QUADSPI_CCR_ADMODE_Pos) |	// 01: Address on a single line
		(0x01 << QUADSPI_CCR_IMODE_Pos) |	// 01: Instruction on a single line
		(cmd << QUADSPI_CCR_INSTRUCTION_Pos) |	// Instruction to be send to the external SPI device.
		0;

	if ((QUADSPI->CCR & QUADSPI_CCR_ADMODE_Msk) != 0)
	{
		// Initiate operation
		QUADSPI->AR = address & 0x00FFFFFF;	// В indirect режимах адрес должен быть в допустимых для указанного при ините размера памяти
		//PRINTF("spidf_iostart QUADSPI->AR=%08lX, QUADSPI->SR=%08lX\n", QUADSPI->AR, QUADSPI->SR);
	}
}

void spidf_initialize(void)
{
	RCC->MP_AHB6ENSETR = RCC_MC_AHB6ENSETR_QSPIEN;
	(void) RCC->MP_AHB6ENSETR;
	RCC->MP_AHB6LPENSETR = RCC_MC_AHB6LPENSETR_QSPILPEN;
	(void) RCC->MP_AHB6LPENSETR;

	// Connect I/O pins
	//SPIDF_HARDINITIALIZE();

	//PRINTF("QUADSPI->IPIDR=%08x\n", QUADSPI->IPIDR);

	QUADSPI->CR &= ~ QUADSPI_CR_EN_Msk;
	QUADSPI->CCR = 0;

	QUADSPI->DCR = ((QUADSPI->DCR & ~ (QUADSPI_DCR_FSIZE_Msk | QUADSPI_DCR_CSHT_Msk | QUADSPI_DCR_CKMODE_Msk))) |
		(23 << QUADSPI_DCR_FSIZE_Pos) |	// FSIZE+1 is effectively the number of address bits required to address the Flash memory.
		(7 << QUADSPI_DCR_CSHT_Pos) |	// 0: nCS stays high for at least 1 cycle between Flash memory commands
		(0 << QUADSPI_DCR_CKMODE_Pos) |	// 0: CLK must stay low while nCS is high (chip select released). This is referred to as mode 0.
		//(1 << QUADSPI_DCR_CKMODE_Pos) |	// 1: CLK must stay high while nCS is high (chip select released). This is referred to as mode 3.
		0;

	QUADSPI->CR = ((QUADSPI->CR & ~ (QUADSPI_CR_PRESCALER_Msk))) |
		(0x00 << QUADSPI_CR_PRESCALER_Pos) | // 1: FCLK = Fquadspi_ker_ck/2
		0;
	QUADSPI->CR |= QUADSPI_CR_EN_Msk;
}

void spidf_uninitialize(void)
{
	while ((QUADSPI->SR & QUADSPI_SR_BUSY_Msk) != 0)
		;
	// Disconnect I/O pins
	SPIDF_HANGOFF();
}

#elif WIHSPIDFHW && CPUSTYLE_R7S721

// передаем все заказанное количество
static void spidf_write(const uint8_t * buff, uint_fast32_t size)
{
	while (size --)
	{

		while ((SPIBSC0.CMNSR & SPIBSC_CMNSR_TEND) == 0)
			;
		SPIBSC0.SMWDR0.UINT8 [R_IO_LL] = * buff ++;
		/* подготовка к следующему шагу */
		// продолжение обмена без передачи команды, адреса...
		SPIBSC0.SMENR &= ~ (SPIBSC_SMENR_ADE | SPIBSC_SMENR_CDE | SPIBSC_SMENR_DME | SPIBSC_SMENR_OPDE);
		SPIBSC0.SMENR |= 0x08uL << SPIBSC_SMENR_SPIDE_SHIFT;

		if (size == 0)
		{
			// последний обмен
			// SSLKP 0: SPBSSL signal is negated at the end of transfer.
			SPIBSC0.SMCR = SPIBSC_SMCR_SPIWE | SPIBSC_SMCR_SPIE;
		}
		else
		{
			// не последний обмен - удерживаем CS в нуле
			SPIBSC0.SMCR = SPIBSC_SMCR_SPIWE | SPIBSC_SMCR_SPIE | SPIBSC_SMCR_SSLKP;
		}
	}
}

// вычитываем все заказанное количество
static uint_fast8_t spidf_verify(const uint8_t * buff, uint_fast32_t size)
{
	uint_fast8_t err = 0;
	while (size --)
	{
		if (size == 0)
		{
			// последний обмен
			// SSLKP 0: SPBSSL signal is negated at the end of transfer.
			SPIBSC0.SMCR = SPIBSC_SMCR_SPIRE | SPIBSC_SMCR_SPIE;
		}
		else
		{
			// не последний обмен - удерживаем CS в нуле
			SPIBSC0.SMCR = SPIBSC_SMCR_SPIRE | SPIBSC_SMCR_SPIE | SPIBSC_SMCR_SSLKP;
		}

		while ((SPIBSC0.CMNSR & SPIBSC_CMNSR_TEND) == 0)
			;
		err |= * buff ++ != SPIBSC0.SMRDR0.UINT8 [R_IO_LL];
		/* подготовка к следующему шагу */
		// продолжение обмена без передачи команды, адреса...
		SPIBSC0.SMENR &= ~ (SPIBSC_SMENR_ADE | SPIBSC_SMENR_CDE | SPIBSC_SMENR_DME | SPIBSC_SMENR_OPDE);
	}
	return err;
}

// вычитываем все заказанное количество
static void spidf_read(uint8_t * buff, uint_fast32_t size)
{
	while (size --)
	{
		if (size == 0)
		{
			// последний обмен
			// SSLKP 0: SPBSSL signal is negated at the end of transfer.
			SPIBSC0.SMCR = SPIBSC_SMCR_SPIRE | SPIBSC_SMCR_SPIE;
		}
		else
		{
			// не последний обмен - удерживаем CS в нуле
			SPIBSC0.SMCR = SPIBSC_SMCR_SPIRE | SPIBSC_SMCR_SPIE | SPIBSC_SMCR_SSLKP;
		}

		while ((SPIBSC0.CMNSR & SPIBSC_CMNSR_TEND) == 0)
			;
		* buff ++ = SPIBSC0.SMRDR0.UINT8 [R_IO_LL];
		/* подготовка к следующему шагу */
		// продолжение обмена без передачи команды, адреса...
		SPIBSC0.SMENR &= ~ (SPIBSC_SMENR_ADE | SPIBSC_SMENR_CDE | SPIBSC_SMENR_DME | SPIBSC_SMENR_OPDE);
	}
}

static void spidf_unselect(void)
{
//	while ((SPIBSC0.CMNSR & SPIBSC_CMNSR_TEND) == 0)
//		;
	while ((SPIBSC0.CMNSR & SPIBSC_CMNSR_SSLF) != 0)
		;
	// Disconnect I/O pins
	SPIDF_HANGOFF();
}

void spidf_initialize(void)
{

	//PRINTF("SPIBSC0.SMDMCR=%08lX\n", SPIBSC0.SMDMCR);
	//PRINTF("SPIBSC0.SPBCR=%08lX\n", SPIBSC0.SPBCR);
	//PRINTF("SPIBSC0.DRENR=%08lX\n", SPIBSC0.DRENR);
	//PRINTF("SPIBSC0.CMNCR=%08lX\n", SPIBSC0.CMNCR);
	//PRINTF("SPIBSC0.SSLDR=%08lX\n", SPIBSC0.SSLDR);

	// spi multi-io hang on
	CPG.STBCR9 &= ~ CPG_STBCR9_BIT_MSTP93;	// Module Stop 93	- 0: Clock supply to channel 0 of the SPI multi I/O bus controller is runnuing.
	(void) CPG.STBCR9;			/* Dummy read */

	SPIBSC0.SMCR = 0;

	// 17.4.2 SSL Delay Register (SSLDR)
	SPIBSC0.SSLDR =
		(0x07uL << SPIBSC_SSLDR_SPNDL_SHIFT) |
		(0x07uL << SPIBSC_SSLDR_SLNDL_SHIFT) |
		(0x07uL << SPIBSC_SSLDR_SCKDL_SHIFT) |
		0;

	// 17.4.1 Common Control Register (CMNCR)
	SPIBSC0.CMNCR =
		SPIBSC_CMNCR_MD |	// spi mode
		(1uL << SPIBSC_CMNCR_SFDE_SHIFT) |	// after reset: 1
		(1uL << SPIBSC_CMNCR_MOIIO3_SHIFT) |	// after reset: 2
		(1uL << SPIBSC_CMNCR_MOIIO2_SHIFT) |	// after reset: 2
		(1uL << SPIBSC_CMNCR_MOIIO1_SHIFT) |	// after reset: 2
		(1uL << SPIBSC_CMNCR_MOIIO0_SHIFT) |	// after reset: 2
		(1uL << SPIBSC_CMNCR_IO3FV_SHIFT) |	// after reset: 1
		(1uL << SPIBSC_CMNCR_IO2FV_SHIFT) |	// after reset: 0
		(1uL << SPIBSC_CMNCR_IO0FV_SHIFT) |	// after reset: 0
		0uL * SPIBSC_CMNCR_CPHAR |	// after reset: 0
		0uL * SPIBSC_CMNCR_CPHAT |	// after reset: 0
		0uL * SPIBSC_CMNCR_SSLP |	// after reset: 0
		0uL * SPIBSC_CMNCR_BSZ |	// after reset: 0
		0;

	// Baud rate
	SPIBSC0.SPBCR = (SPIBSC0.SPBCR & ~ (SPIBSC_SPBCR_BRDV | SPIBSC_SPBCR_SPBR)) |
		(0 << SPIBSC_SPBCR_BRDV_SHIFT) |	// 0..3
		(2 << SPIBSC_SPBCR_SPBR_SHIFT) |	// 0..255
		0;

	SPIBSC0.SMDMCR =
		(0 << SPIBSC_SMDMCR_DMDB_SHIFT) |	// 1 bit dummy bytes
		//(0x07uL << SPIBSC_SMDMCR_DMCYC_SHIFT) |	// 8 dummy bits
		0;


//	for (;;)
//		writeEnableDATAFLASH();		/* write enable */

	//PRINTF("SPIBSC0.SMDMCR=%08lX\n", SPIBSC0.SMDMCR);
	//PRINTF("SPIBSC0.SPBCR=%08lX\n", SPIBSC0.SPBCR);
	//PRINTF("SPIBSC0.CMNCR=%08lX\n", SPIBSC0.CMNCR);
	//PRINTF("SPIBSC0.SSLDR=%08lX\n", SPIBSC0.SSLDR);
}

void spidf_uninitialize(void)
{
//	while ((SPIBSC0.CMNSR & SPIBSC_CMNSR_TEND) == 0)
//		;
	while ((SPIBSC0.CMNSR & SPIBSC_CMNSR_SSLF) != 0)
		;
	// Disconnect I/O pins
	SPIDF_HANGOFF();
}

static void spidf_iostart(
	uint_fast8_t direction,	// 0: dataflash-to-CPU, 1: CPU-to-dataflash
	uint_fast8_t cmd,
	uint_fast8_t ndummy,	// number of dummy bytes
	uint_fast32_t size,
	uint_fast8_t hasaddress,
	uint_fast32_t address
	)
{
	// Connect I/O pins
	SPIDF_HARDINITIALIZE();
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
	// 17.4.13 SPI Mode Enable Setting Register (SMENR)
	SPIBSC0.SMENR =
		(0x00uL << SPIBSC_SMENR_CDB_SHIFT) | /* 1 data bit */
		(0x00uL << SPIBSC_SMENR_OCDB_SHIFT) | /* 1 optional command bit */
		(0x00uL << SPIBSC_SMENR_ADB_SHIFT) | /* 1 address bit */
		(0x00uL << SPIBSC_SMENR_OPDB_SHIFT) | /* 1 optional data bit */
		(0x00uL << SPIBSC_SMENR_SPIDB_SHIFT) | /* Transfer Data Bit Size */
		(0x01uL << SPIBSC_SMENR_CDE_SHIFT) | /* 1: Command output enabled */
		(0x00uL << SPIBSC_SMENR_OCDE_SHIFT) | /* 0: Optional command output disabled */
		(0x00uL << SPIBSC_SMENR_OPDE_SHIFT) | /* Option Data Enable 0000: Output disabled */
		((hasaddress ? 0x07 : 0x00) << SPIBSC_SMENR_ADE_SHIFT) | /* No address send or 0111: ADR[23:0] */
		((ndummy != 0) << SPIBSC_SMENR_DME_SHIFT) |
		((size && ! direction ? 0x08uL : 0) << SPIBSC_SMENR_SPIDE_SHIFT) | /* 8 bits transferred (enables data at address 0 of the SPI mode read/write data registers 0) */
		0;
	// 17.4.10 SPI Mode Command Setting Register (SMCMR)
	SPIBSC0.SMCMR =
		(cmd << SPIBSC_SMCMR_CMD_SHIFT) | /* command byte */
		(0x00uL << SPIBSC_SMCMR_OCMD_SHIFT) | /* optional command */
		0;

	SPIBSC0.SMADR = address;

    SPIBSC0.SMCR =
		((direction) ? SPIBSC_SMCR_SPIWE : SPIBSC_SMCR_SPIRE) |
		((size > 1) * SPIBSC_SMCR_SSLKP) | // 0: SPBSSL signal is negated at the end of transfer.
		((size == 0 || direction) * SPIBSC_SMCR_SPIE) |	// запускаем если не будет обмена данными
		0;

	// при передаче формируется только команла и адрес при необходимости
}

#endif /* WIHSPIDFHW */

/* получить 32-бит значение */
static uint_fast32_t
USBD_peek_u32(
	const uint8_t * buff
	)
{
	return
		((uint_fast32_t) buff [3] << 24) +
		((uint_fast32_t) buff [2] << 16) +
		((uint_fast32_t) buff [1] << 8) +
		((uint_fast32_t) buff [0] << 0);
}

/* получить 32-бит значение */
static uint_fast32_t
USBD_peek_u24(
	const uint8_t * buff
	)
{
	return
		((uint_fast32_t) buff [2] << 16) +
		((uint_fast32_t) buff [1] << 8) +
		((uint_fast32_t) buff [0] << 0);
}

static unsigned long ulmin(
	unsigned long a,
	unsigned long b)
{
	return a < b ? a : b;
}

/* снять защиту записи для следующей команды */
void writeEnableDATAFLASH(void)
{
	spidf_iostart(SPDIFIO_WRITE, 0x06, 0, 0, 0, 0);	/* 0x06: write enable */
	spidf_unselect();	/* done sending data to target chip */
}

void writeDisableDATAFLASH(void)
{
	spidf_iostart(SPDIFIO_WRITE, 0x04, 0, 0, 0, 0);	/* 0x04: write disable */
	spidf_unselect();	/* done sending data to target chip */
}

uint_fast8_t dataflash_read_status(void)
{
	uint8_t v;
	enum { SPDIF_IOSIZE = sizeof v };

	spidf_iostart(SPDIFIO_READ, 0x05, 0, SPDIF_IOSIZE, 0, 0x00000000);	/* read status register */
	spidf_read(& v, SPDIF_IOSIZE);
	spidf_unselect();	/* done sending data to target chip */

	return v;
}

int timed_dataflash_read_status(void)
{
	unsigned long w = 40000;
	while (w --)
	{
		if ((dataflash_read_status() & 0x01) == 0)
			return 0;
	}
	PRINTF(PSTR("DATAFLASH timeout error\n"));
	return 1;
}

static int largetimed_dataflash_read_status(void)
{
	unsigned long w = 40000000;
	while (w --)
	{
		if ((dataflash_read_status() & 0x01) == 0)
			return 0;
	}
	PRINTF(PSTR("DATAFLASH erase timeout error\n"));
	return 1;
}

/* чтение параметра с требуемым индексом
 *
 */
static void readSFDPDATAFLASH(spitarget_t target, unsigned long flashoffset, uint8_t * buff, unsigned size)
{
	// Read SFDP
	spidf_iostart(SPDIFIO_READ, 0x5A, 1, size, 1, flashoffset);	// READ SFDP (with dummy bytes)
	spidf_read(buff, size);
	spidf_unselect();	/* done sending data to target chip */
}

static int seekparamSFDPDATAFLASH(spitarget_t target, unsigned long * paramoffset, uint_fast8_t * paramlength, uint_fast8_t id, uint_fast8_t lastnum)
{
	uint8_t buff8 [8];
	unsigned i;

	for (i = 0; i <= lastnum; ++ i)
	{
		readSFDPDATAFLASH(target, (i + 1) * 8uL, buff8, 8);
		if (buff8 [0] == id)
		{
			* paramlength = buff8 [3];	// in double words
			* paramoffset = USBD_peek_u24(& buff8 [4]);
			return 0;
		}
	}
	/* parameter id not found */
	return 1;
}

// Atmel Data Flash: Read: ID = 0x1F devId = 0x4501, mf_dlen=0x00

int testchipDATAFLASH(void)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	/* Ожидание бита ~RDY в слове состояния. Для FRAM не имеет смысла.
	Вставлено для возможности использования DATAFLASH */

	if (timed_dataflash_read_status())
	{
		PRINTF(PSTR("testchipDATAFLASH: timeout\n"));
		return 1;
	}

#if WITHDEBUG
	{
		unsigned char mf_id;	// Manufacturer ID
		unsigned char mf_devid1;	// device ID (part 1)
		unsigned char mf_devid2;	// device ID (part 2)
		unsigned char mf_dlen;	// Extended Device Information String Length

		uint8_t mfa [4];

		enum { SPDIF_IOSIZE = sizeof mfa };
		spidf_iostart(SPDIFIO_READ, 0x9F, 0, SPDIF_IOSIZE, 0, 0x00000000);	/* read id register */
		spidf_read(mfa, SPDIF_IOSIZE);
		spidf_unselect();	/* done sending data to target chip */

		mf_id = mfa [0];
		mf_devid1 = mfa [1];
		mf_devid2 = mfa [02];
		mf_dlen = mfa [3];

		PRINTF(PSTR("spidf: ID = 0x%02X devId = 0x%02X%02X, mf_dlen=0x%02X\n"), mf_id, mf_devid1, mf_devid2, mf_dlen);
	}
#endif /* WITHDEBUG */

	// Read root SFDP
	uint8_t buff8 [8];
	readSFDPDATAFLASH(target, 0x000000, buff8, 8);

	uint_fast32_t signature = USBD_peek_u32(& buff8 [0]);

	//PRINTF(PSTR("SFDP: signature=%08lX, lastparam=0x%02X\n"), signature, buff8 [6]);
	if (signature == 0x50444653)
	{
		// Serial Flash Discoverable Parameters (SFDP), for Serial NOR Flash
		const uint_fast8_t lastparam = buff8 [6];
		unsigned long ptp;
		uint_fast8_t len4;
		if (seekparamSFDPDATAFLASH(target, & ptp, & len4, 0x00, lastparam))
		{
			PRINTF("SFDP parameter 0x00 not found\n");
			return 0;
		}

		//PRINTF("SFDP: ptp=%08lX, len4=%02X\n", ptp, len4);
		if (len4 < 9 || len4 > 16)
			return 0;
		uint8_t buff32 [len4 * 4];
		readSFDPDATAFLASH(target, ptp, buff32, len4 * 4);
		const uint_fast32_t dword1 = USBD_peek_u32(buff32 + 4 * 0);
		const uint_fast32_t dword2 = USBD_peek_u32(buff32 + 4 * 1);
		const uint_fast32_t dword3 = USBD_peek_u32(buff32 + 4 * 2);
		const uint_fast32_t dword4 = USBD_peek_u32(buff32 + 4 * 3);
		const uint_fast32_t dword5 = USBD_peek_u32(buff32 + 4 * 4);
		const uint_fast32_t dword6 = USBD_peek_u32(buff32 + 4 * 5);
		const uint_fast32_t dword7 = USBD_peek_u32(buff32 + 4 * 6);
		const uint_fast32_t dword8 = USBD_peek_u32(buff32 + 4 * 7);
		const uint_fast32_t dword9 = USBD_peek_u32(buff32 + 4 * 8);
		//printhex(ptp, buff32, 256);
		/* Print density information. */
		if ((dword2 & 0x80000000uL) == 0)
			PRINTF("SFDP: density=%08lX (%u Kbi)\n", dword2, (dword2 >> 10) + 1);
		else
			PRINTF("SFDP: density=%08lX (%u Mbi)\n", dword2, 1uL << ((dword2 & 0x7FFFFFFF) - 10));
		//PRINTF("SFDP: Sector Type 1 Size=%08lX, Sector Type 1 Opcode=%02lX\n", 1uL << ((dword8 >> 0) & 0xFF), (dword8 >> 8) & 0xFF);
	}
	return 0;
}

int prepareDATAFLASH(void)
{
	const uint_fast8_t status = dataflash_read_status();

	if ((status & 0x1C) != 0)
	{
		if (timed_dataflash_read_status())
		{
			PRINTF(PSTR("prepareDATAFLASH: timeout\n"));
			return 1;
		}

		PRINTF(PSTR("prepareDATAFLASH: Clear write protect bits\n"));
		writeEnableDATAFLASH();		/* write enable */

		uint8_t v = 0x00;	/* status register data */
		// Write Status Register
		spidf_iostart(SPDIFIO_WRITE, 0x01, 0, 1, 0, 0);	/* Write Status Register */
		spidf_write(& v, 1);
		spidf_unselect();	/* done sending data to target chip */
		TP();
	}

	return timed_dataflash_read_status();
}

int sectoreraseDATAFLASH(unsigned long flashoffset)
{
	//PRINTF(PSTR(" Erase sector at address %08lX\n"), flashoffset);

	if (timed_dataflash_read_status())
	{
		PRINTF(PSTR("sectoreraseDATAFLASH: timeout\n"));
		return 1;
	}

	writeEnableDATAFLASH();		/* write enable */

	// start byte programm
	spidf_iostart(SPDIFIO_WRITE, 0xD8, 0, 0, 1, flashoffset);		/* 64KB SECTOR ERASE */
	spidf_unselect();	/* done sending data to target chip */
	//timed_dataflash_read_status(target);
	return 0;
}

int writesinglepageDATAFLASH(unsigned long flashoffset, const unsigned char * data, unsigned long len)
{
	//PRINTF(PSTR(" Prog to address %08lX %02X\n"), flashoffset, len);

	if (timed_dataflash_read_status())
	{
		PRINTF(PSTR("writesinglepageDATAFLASH: timeout\n"));
		return 1;
	}

	writeEnableDATAFLASH();		/* write enable */

	// start byte programm

	spidf_iostart(SPDIFIO_WRITE, 0x02, 0, len, 1, flashoffset);		/* Page Program */
	spidf_write(data, len);
	spidf_unselect();	/* done sending data to target chip */

	//PRINTF(PSTR( Prog to address %08lX %02X done\n"), flashoffset, len);
	//timed_dataflash_read_status(target);
	return 0;
}

int writeDATAFLASH(unsigned long flashoffset, const uint8_t * data, unsigned long len)
{
	//PRINTF(PSTR("Write to address %08lX %02X\n"), flashoffset, len);
	while (len != 0)
	{
		unsigned long offset = flashoffset & 0xFF;
		unsigned long part = ulmin(len, ulmin(256, 256 - offset));

		if (writesinglepageDATAFLASH(flashoffset, data, part))
			return 1;

		len -= part;
		flashoffset += part;
		data += part;
	}
	//PRINTF(PSTR("Write to address %08lX %02X done\n"), flashoffset, len);
	return 0;
}

int verifyDATAFLASH(unsigned long flashoffset, const uint8_t * data, unsigned long len)
{
	unsigned long err = 0;

	//PRINTF(PSTR("Compare from address %08lX\n"), flashoffset);

	if (timed_dataflash_read_status())
	{
		PRINTF(PSTR("verifyDATAFLASH: timeout\n"));
		return 1;
	}

	spidf_iostart(SPDIFIO_READ, 0x03, 0, len, 1, flashoffset);	/* 0x03: sequential read block */
	err = spidf_verify(data, len);
	spidf_unselect();	/* done sending data to target chip */

	if (err)
		PRINTF(PSTR("verifyDATAFLASH: Done compare, have errors\n"));

	return err;
}

int readDATAFLASH(unsigned long flashoffset, uint8_t * data, unsigned long len)
{
	//PRINTF("readDATAFLASH start, data=%p, len=%lu\n", data, len);
	if (timed_dataflash_read_status())
	{
		PRINTF("readDATAFLASH: timeout\n");
		return 1;
	}
	//PRINTF("readDATAFLASH start2\n");

	spidf_iostart(SPDIFIO_READ, 0x03, 0, len, 1, flashoffset);	/* 0x03: sequential read block */
	spidf_read(data, len);
	spidf_unselect();	/* done sending data to target chip */
	//PRINTF("readDATAFLASH done\n");
	return 0;
}


void bootloader_readimage(unsigned long flashoffset, uint8_t * dest, unsigned Len)
{
	spidf_initialize();
	readDATAFLASH(flashoffset, dest, Len);
	spidf_uninitialize();
}


#endif /* WIHSPIDFHW || WIHSPIDFSW */
