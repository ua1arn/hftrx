/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "clocks.h"
#include "spi.h"
#include "gpio.h"
#include "formats.h"
#include <stdlib.h>

// битовые маски, соответствующие биту в байте по его номеру.
const uint_fast8_t rbvalues [8] =
{
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 
};

#if WITHSPIHW || WITHSPISW

	#if ! defined (SPI_ALLCS_BITS) && ! CPUSTYLE_XC7Z && ! CPUSTYLE_XCZU
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
spi_allcs_disable(void)
{
#if defined (SPI_ALLCS_DISABLE)

	SPI_ALLCS_DISABLE();

#elif CPUSTYLE_ARM || CPUSTYLE_ATXMEGA

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

static void
spi_cs_disable(
	spitarget_t target	/* addressing to chip */
	)
{
#if defined (SPI_CS_DEASSERT)
	SPI_CS_DEASSERT(target);
#else /* defined (SPI_CS_DEASSERT) */
	spi_allcs_disable();
#endif /* defined (SPI_CS_DEASSERT) */
}

// SPI chip select active
static void
spi_cs_enable(
	spitarget_t target	/* addressing to chip */
	)
{
#if WITHSPICSEMIO
	/* специфицеская конфигурация - управление сигналами CS SPI периферии выполняется через EMIO */

	SPI_CS_ASSERT(target);

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	SPI_CS_ASSERT(target);

#elif CPUSTYLE_ARM || CPUSTYLE_ATXMEGA

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
#if WITHSPICSEMIO
	/* специфицеская конфигурация - управление сигналами CS SPI периферии выполняется через EMIO */

#elif CPUSTYLE_ARM || CPUSTYLE_ATXMEGA

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

		#if CPUSTYLE_XC7Z || CPUSTYLE_XCZU
			return SPI_TARGET_MISO_PIN != 0;
		#elif SPI_BIDIRECTIONAL
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

void prog_unselect_impl(
	spitarget_t target	/* SHIFTED addressing to chip (on ATMEGA - may be bit mask) */
	)
{
#if UC1608_CSP
	spi_unselect255();
#endif /* UC1608_CSP */
	spi_cs_disable(target);	// chip select inactive - and latch in 74HC595
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
	spi_allcs_disable();	// chip select inactive
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

#if WITHSPIHW
	// аппаратный SPI

#if WITHFPGAWAIT_AS || WITHFPGALOAD_PS || WITHDSPEXTFIR
	hardware_spi_master_setfreq(SPIC_SPEEDUFAST, SPISPEEDUFAST);
#endif /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS || WITHDSPEXTFIR */

	hardware_spi_master_setfreq(SPIC_SPEEDFAST, SPISPEED);

#if defined (SPISPEED400k) || defined (SPISPEED100k)
	hardware_spi_master_setfreq(SPIC_SPEED100k, SPISPEED100k);		// 100 kHz for MICROCHIP MCP3204/MCP3208
	hardware_spi_master_setfreq(SPIC_SPEED400k, SPISPEED400k);
	hardware_spi_master_setfreq(SPIC_SPEED1M, 1000000uL);	/* 1 MHz для XPT2046 */
	hardware_spi_master_setfreq(SPIC_SPEED4M, 4000000uL);	/* 4 MHz для CS4272 */
	hardware_spi_master_setfreq(SPIC_SPEED10M, 10000000uL);	/* 10 MHz для ILI9341 */
	hardware_spi_master_setfreq(SPIC_SPEED25M, 25000000uL);	/* 25 MHz  */
#endif /* (SPISPEED400k) || defined (SPISPEED100k) */

#endif /* WITHSPIHW */
}

#endif /* WITHSPIHW || WITHSPISW */


#if (WITHNANDHW || WITHNANDSW)

#if WITHNANDSW

// поддержка работы с NAND по bit-bang

// Get Ready/Busy# pin state
static uint_fast8_t nand_rbc_get(void)
{
	return HARDWARE_NAND_RBC_GET();
}

// Chip enable
static void nand_csb_set(uint_fast8_t state)
{
	HARDWARE_NAND_CSB_SET(state);
}

// Address latch enable
static void nand_ale_set(uint_fast8_t state)
{
	HARDWARE_NAND_ALE_SET(state);
}

// Command latch enable
static void nand_cle_set(uint_fast8_t state)
{
	HARDWARE_NAND_CLE_SET(state);
}

// Read enable: Gates transfers from the NAND Flash device to the host system.
static void nand_reb_set(uint_fast8_t state)
{
	HARDWARE_NAND_REB_SET(state);
}

// Write enable: Gates transfers from the host system to the NAND Flash device
static void nand_web_set(uint_fast8_t state)
{
	HARDWARE_NAND_WEB_SET(state);
}

static void nand_wp_set(uint_fast8_t state)
{
#if defined (HARDWARE_NAND_WPB)
	HARDWARE_NAND_WPB_SET(state);
#endif
}

// bus programming: write data to chip
static void nand_data_bus_write(void)
{
	HARDWARE_NAND_BUS_WRITE();
}

// bus programming: write data to chip
static void nand_data_bus_read(void)
{
	HARDWARE_NAND_BUS_READ();
}


static void nand_data_out(uint_fast8_t v)
{
	HARDWARE_NAND_DATA_SET(v);
}

//
static uint_fast8_t nand_data_in(void)
{
	return HARDWARE_NAND_DATA_GET();
}

#elif WITHNANDHW
//	Аппаратная поддержка работы с NAND

#endif


/////////////////////
///
static void nand_cs_activate(void)
{
	nand_csb_set(0);
}

static void nand_cs_deactivate(void)
{
	nand_csb_set(1);
}

static void nand_write(uint_fast8_t v)
{
	//PRINTF("nand_write: %02X\n", v);
	nand_data_bus_write(); // OUT direction
	nand_web_set(0);
	nand_data_out(v);
	nand_web_set(1);
}

static void nand_write_command(uint_fast8_t v)
{
	//PRINTF("nand_write_command: %02X\n", v);
	nand_cle_set(1);
	nand_write(v);
	nand_cle_set(0);
}

static void nand_write_address(uint_fast8_t v)
{
	//PRINTF("nand_write_address: %02X\n", v);
	nand_ale_set(1);
	nand_write(v);
	nand_ale_set(0);
}

// Sequential data read
static void nand_read(uint8_t * buff, unsigned count)
{
	nand_data_bus_read();	// IN direction
	while (count --)
	{
		nand_reb_set(0);
		* buff ++ = nand_data_in();
		nand_reb_set(1);
	}
}

static void nand_waitbusy(void)
{
	local_delay_us(10);
	while (nand_rbc_get() == 0)
		;
}

///////////////////////////
///

void nand_reset(void)
{
	// Reset
	PRINTF("nand_reset\n");

	nand_cs_activate();
	nand_write_command(0xFF);	// RESET command
	nand_cs_deactivate();

	nand_waitbusy();

	PRINTF("nand_reset done\n");
}

void nand_read_id(void)
{
	//PRINTF("nand_read_id:\n");
#if WITHDEBUG
	uint8_t v [4];

	// Read ID
	nand_cs_activate();
	nand_write_command(0x90);
	nand_write_address(0x00);
	nand_read(v, ARRAY_SIZE(v));
	nand_cs_deactivate();

	// NAND IDs = 2C DA 90 95
	// DA == MT29F2G08AAC
	PRINTF("NAND IDs = %02X %02X %02X %02X\n", v [0], v [1], v [2], v [3]);
#endif /* WITHDEBUG */
	//PRINTF("nand_read_id: done\n");
}


void nand_readfull(void)
{
	//PRINTF("nand_readfull:\n");
	unsigned long columnaddr = 0;
	unsigned long blockaddr = 0;	// 0..2047
	unsigned long pageaddr = 0;		// 0..31
	// Memory x8
	// of blocks 0..2047
	// of pages 0..31
	// of bytes 0..2047 and 2048..2111 spare area
	static uint8_t buff [512];
	unsigned i;
	nand_cs_activate();
	nand_write_command(0x00);	// PAGE READ command
	nand_write_address((columnaddr >> 0) & 0xFF);	// Col Addr 1: ca7..ca0
	nand_write_address((columnaddr >> 8) & 0x0F);	// Col Addr 2: 0,0,0,0, ca11..ca8
	nand_write_address((((blockaddr >> 6) & 0x03) << 6) | ((pageaddr >> 0) & 0x3F));	// Row Addr 1: ba7..ba6, pa5..pa0
	nand_write_address((blockaddr >> 8) & 0xFF);	// Row Addr 2: ba15..ba8
	nand_write_address((blockaddr >> 16) & 0x01);	// Row Addr 3, 0,0,0,0,0,0,0, ba16
	nand_write_command(0x30);	// 0x30 command

	nand_waitbusy();

	unsigned long pagesize = 2 * 1024uL;
	unsigned long offset = 0;

	for (;offset < pagesize;)
	{
		nand_read(buff, ARRAY_SIZE(buff));
		printhex(offset, buff, 512);
		offset += 512;
	}

	nand_cs_deactivate();
	//PRINTF("nand_readfull: done\n");
}

void nand_initialize(void)
{
	//PRINTF("nand_initialize:\n");
	HARDWARE_NAND_INITIALIZE();

	nand_wp_set(0);		// CHip write protected

	nand_csb_set(1);
	nand_cle_set(0);
	nand_ale_set(0);
	nand_reb_set(1);
	nand_web_set(1);

	nand_reset();

	//PRINTF("nand_initialize: done\n");
}

void nand_tests(void)
{
	//PRINTF("nand_tests:\n");
	nand_read_id();
	//nand_readfull();
	//PRINTF("nand_tests: done\n");
}

#endif /* (WITHNANDHW || WITHNANDSW) */

//#define WIHSPIDFOVERSPI 1	/* В SPI программаторе для работы используется один из обычных каналов SPI */
//#define targetdataflash targetext1

#if WIHSPIDFHW || WIHSPIDFSW || WIHSPIDFOVERSPI

enum { SPDIFIO_READ, SPDIFIO_WRITE };	// в случае пеердачи только команды используем write */
enum { SPDFIO_1WIRE, SPDFIO_2WIRE, SPDFIO_4WIRE, SPDFIO_numwires };

static uint8_t readxb [SPDFIO_numwires] = { 0x0b, 0x00, 0x00 };
static uint8_t dmyb [SPDFIO_numwires];

#if WIHSPIDFSW || WIHSPIDFOVERSPI


/////////
// https://github.com/renesas-rz/rza1_qspi_flash/blob/master/qspi_flash.c


// Use block SPIBSC0
// 17. SPI Multi I/O Bus Controller
//

void spidf_initialize(void)
{
#if defined (SPIDF_SOFTINITIALIZE)
	// Connect I/O pins
	SPIDF_SOFTINITIALIZE();
#endif /* defined (SPIDF_SOFTINITIALIZE) */
}

#if ! WIHSPIDFOVERSPI

static uint_fast8_t spidf_rbit(uint_fast8_t v)
{
	uint_fast8_t r;
	SPIDF_MOSI(v);
	SPIDF_SCLK(0);
	r = SPIDF_MISO();
	SPIDF_SCLK(1);
	return r;
}

static uint_fast8_t spidf_rbitfast(void)
{
	uint_fast8_t r;
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


uint_fast8_t spidf_read_byte(uint_fast8_t v)
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

uint_fast8_t spidf_read_bytefast(void)
{
	uint_fast8_t r = 0;

	r = r * 2 + spidf_rbitfast();
	r = r * 2 + spidf_rbitfast();
	r = r * 2 + spidf_rbitfast();
	r = r * 2 + spidf_rbitfast();
	r = r * 2 + spidf_rbitfast();
	r = r * 2 + spidf_rbitfast();
	r = r * 2 + spidf_rbitfast();
	r = r * 2 + spidf_rbitfast();

	return r;
}

static void spidf_write_byte(uint_fast8_t v)
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
#endif

void spidf_uninitialize(void)
{
}

void spidf_hangoff(void)
{
}


#define SPIMODE_AT26DF081A	SPIC_MODE3

// Connrect I/O pins
static void spidf_select(void)
{
#if WIHSPIDFOVERSPI
	spitarget_t target = targetdataflash;	/* addressing to chip */
	spi_select(target, SPIMODE_AT26DF081A);
#else /* WIHSPIDFOVERSPI */
	SPIDF_SELECT();
#endif /* WIHSPIDFOVERSPI */
}

static void spidf_unselect(void)
{
#if WIHSPIDFOVERSPI
	spitarget_t target = targetdataflash;	/* addressing to chip */
	spi_unselect(target);
#else /* WIHSPIDFOVERSPI */
	SPIDF_UNSELECT();
#endif /* WIHSPIDFOVERSPI */
}

static void spidf_to_read(void)
{
#if WIHSPIDFOVERSPI
	spitarget_t target = targetdataflash;	/* addressing to chip */
	spi_to_read(target);
#else /* WIHSPIDFOVERSPI */
#endif /* WIHSPIDFOVERSPI */
}

static void spidf_to_write(void)
{
#if WIHSPIDFOVERSPI
	spitarget_t target = targetdataflash;	/* addressing to chip */
	spi_to_write(target);
#else /* WIHSPIDFOVERSPI */
#endif /* WIHSPIDFOVERSPI */
}

static void spidf_progval8_p1(uint_fast8_t sendval)
{
#if WIHSPIDFOVERSPI
	spitarget_t target = targetdataflash;	/* addressing to chip */
	spi_progval8_p1(target, sendval);
#else /* WIHSPIDFOVERSPI */
	spidf_write_byte(sendval);
#endif /* WIHSPIDFOVERSPI */
}

static void spidf_progval8_p2(uint_fast8_t sendval)
{
#if WIHSPIDFOVERSPI
	spitarget_t target = targetdataflash;	/* addressing to chip */
	spi_progval8_p2(target, sendval);
#else /* WIHSPIDFOVERSPI */
	spidf_write_byte(sendval);
#endif /* WIHSPIDFOVERSPI */
}

static uint_fast8_t spidf_complete(void)
{
#if WIHSPIDFOVERSPI
	spitarget_t target = targetdataflash;	/* addressing to chip */
	spi_complete(target);
#else /* WIHSPIDFOVERSPI */
#endif /* WIHSPIDFOVERSPI */
	return 0;
}

static uint_fast8_t spidf_progval8(uint_fast8_t sendval)
{
#if WIHSPIDFOVERSPI
	spitarget_t target = targetdataflash;	/* addressing to chip */
	return spi_progval8(target, sendval);
#else /* WIHSPIDFOVERSPI */
	return spidf_read_byte(sendval);
#endif /* WIHSPIDFOVERSPI */
}

static uint_fast8_t spidf_readval8(void)
{
#if WIHSPIDFOVERSPI
	spitarget_t target = targetdataflash;	/* addressing to chip */
	return spi_progval8(target, sendval);
#else /* WIHSPIDFOVERSPI */
	return spidf_read_bytefast();
#endif /* WIHSPIDFOVERSPI */
}

static void spidf_iostart(
	uint_fast8_t direction,	// 0: dataflash-to-memory, 1: Memory-to-dataflash
	uint_fast8_t cmd,
	uint_fast8_t readnb,	// признак работы по QSPI 4 bit - все кроме команды идет во 4-байтной шине
	uint_fast8_t ndummy,	// number of dummy bytes
	uint_fast32_t size,
	uint_fast8_t hasaddress,
	uint_fast32_t address
	)
{
	ASSERT(readnb == SPDFIO_1WIRE);
	spidf_select();	/* start sending data to target chip */
	spidf_progval8_p1(cmd);		/* The Read SFDP instruction code is 0x5A */

	if (hasaddress)
	{
		spidf_progval8_p2(address >> 16);
		spidf_progval8_p2(address >> 8);
		spidf_progval8_p2(address >> 0);
	}
	while (ndummy --)
		spidf_progval8_p2(0x00);	// dummy byte

	spidf_complete();	/* done sending data to target chip */
}


static void spidf_read(uint8_t * buff, uint_fast32_t size)
{
	spidf_to_read();
	while (size --)
		* buff ++ = spidf_readval8();
	spidf_to_write();
}


static uint_fast8_t spidf_verify(const uint8_t * buff, uint_fast32_t size)
{
	uint_fast8_t err = 0;
	spidf_to_read();
	while (size --)
		err |= * buff ++ != spidf_readval8();
	spidf_to_write();
	return err;
}


static void spidf_write(const uint8_t * buff, uint_fast32_t size)
{
	while (size --)
		spidf_progval8(* buff ++);
}

#elif WIHSPIDFHW && CPUSTYLE_XC7Z

// https://github.com/grub4android/lk/blob/579832fe57eeb616cefd82b93d991141f0db91ce/platform/zynq/qspi.c


#define QSPI_CONFIG             0xE000D000
#define  CFG_IFMODE             (1 << 31) // Inteligent Flash Mode
#define  CFG_LITTLE_ENDIAN      (0 << 26)
#define  CFG_BIG_ENDIAN         (1 << 26)
#define  CFG_HOLDB_DR           (1 << 19) // set to 1 for dual/quad spi mode
#define  CFG_NO_MODIFY_MASK     (1 << 17) // do not modify this bit
#define  CFG_MANUAL_START       (1 << 16) // start transaction
#define  CFG_MANUAL_START_EN    (1 << 15) // enable manual start mode
#define  CFG_MANUAL_CS_EN       (1 << 14) // enable manual CS control
#define  CFG_MANUAL_CS          (1 << 10) // directly drives n_ss_out if MANUAL_CS_EN==1
#define  CFG_FIFO_WIDTH_32      (3 << 6)  // only valid setting
#define  CFG_BAUD_MASK          (7 << 3)
#define  CFG_BAUD_DIV_2         (0 << 3)
#define  CFG_BAUD_DIV_4         (1 << 3)
#define  CFG_BAUD_DIV_8         (2 << 3)
#define  CFG_BAUD_DIV_16        (3 << 3)
#define  CFG_CPHA               (1 << 2) // clock phase
#define  CFG_CPOL               (1 << 1) // clock polarity
#define  CFG_MASTER_MODE        (1 << 0) // only valid setting

#define QSPI_IRQ_STATUS         0xE000D004 // ro status (write UNDERFLOW/OVERFLOW to clear)
#define QSPI_IRQ_ENABLE         0xE000D008 // write 1s to set mask bits
#define QSPI_IRQ_DISABLE        0xE000D00C // write 1s to clear mask bits
#define QSPI_IRQ_MASK           0xE000D010 // ro mask value (1 = irq enabled)
#define  TX_UNDERFLOW           (1 << 6)
#define  RX_FIFO_FULL           (1 << 5)
#define  RX_FIFO_NOT_EMPTY      (1 << 4)
#define  TX_FIFO_FULL           (1 << 3)
#define  TX_FIFO_NOT_FULL       (1 << 2)
#define  RX_OVERFLOW            (1 << 0)

#define QSPI_ENABLE             0xE000D014 // write 1 to enable

#define QSPI_DELAY              0xE000D018
#define QSPI_TXD0               0xE000D01C
#define QSPI_RXDATA             0xE000D020
#define QSPI_SLAVE_IDLE_COUNT   0xE000D024
#define QSPI_TX_THRESHOLD       0xE000D028
#define QSPI_RX_THRESHOLD       0xE000D02C
#define QSPI_GPIO               0xE000D030
#define QSPI_LPBK_DLY_ADJ       0xE000D038
#define QSPI_TXD1               0xE000D080
#define QSPI_TXD2               0xE000D084
#define QSPI_TXD3               0xE000D088

#define QSPI_LINEAR_CONFIG      0xE000D0A0
#define  LCFG_ENABLE            (1 << 31) // enable linear quad spi mode
#define  LCFG_TWO_MEM           (1 << 30)
#define  LCFG_SEP_BUS           (1 << 29) // 0=shared 1=separate
#define  LCFG_U_PAGE            (1 << 28)
#define  LCFG_MODE_EN           (1 << 25) // send mode bits (required for dual/quad io)
#define  LCFG_MODE_ON           (1 << 24) // only send instruction code for first read
#define  LCFG_MODE_BITS(n)      (((n) & 0xFF) << 16)
#define  LCFG_DUMMY_BYTES(n)    (((n) & 7) << 8)
#define  LCFG_INST_CODE(n)      ((n) & 0xFF)

#define QSPI_LINEAR_STATUS      0xE000D0A4
#define QSPI_MODULE_ID          0xE000D0FC

static uint32_t readl(uintptr_t addr)
{
	return ZYNQ_IORW32(addr);
}

static void writel(uint32_t data, uintptr_t addr)
{
	ZYNQ_IORW32(addr) = data;
}

struct qspi_ctxt
{
	uint32_t cfg;
	int linear_mode;
};

static struct qspi_ctxt qspi0;

////


static int qspi_enable_linear(struct qspi_ctxt *qspi)
{
	PRINTF("%s:\n", __func__);
	if (qspi->linear_mode)
		return 0;

	/* disable the controller */
	writel(0, QSPI_ENABLE);
	writel(0, QSPI_LINEAR_CONFIG);

	/* put the controller in auto chip select mode and assert chip select */
	qspi->cfg &= ~(CFG_MANUAL_START_EN | CFG_MANUAL_CS_EN | CFG_MANUAL_CS);
	XQSPIPS->CR = qspi->cfg;

#if 1
	// uses Quad I/O mode
	// should be 0x82FF02EB according to xilinx manual for spansion flashes
	writel(LCFG_ENABLE |
			LCFG_MODE_EN |
			LCFG_MODE_BITS(0xff) |
			LCFG_DUMMY_BYTES(2) |
			LCFG_INST_CODE(0xeb),
			QSPI_LINEAR_CONFIG);
#else
	// uses Quad Output Read mode
	// should be 0x8000016B according to xilinx manual for spansion flashes
	writel(LCFG_ENABLE |
			LCFG_MODE_BITS(0) |
			LCFG_DUMMY_BYTES(1) |
			LCFG_INST_CODE(0x6b),
			QSPI_LINEAR_CONFIG);
#endif

	/* enable the controller */
	writel(1, QSPI_ENABLE);

	qspi->linear_mode = 1 /* true*/;

	__DSB();

	return 0;
}

static int qspi_disable_linear(struct qspi_ctxt *qspi)
{
	PRINTF("%s:\n", __func__);
	if (! qspi->linear_mode)
		return 0;

	/* disable the controller */
	writel(0, QSPI_ENABLE);
	writel(0, QSPI_LINEAR_CONFIG);

	/* put the controller back into manual chip select mode */
	qspi->cfg |= (CFG_MANUAL_START_EN | CFG_MANUAL_CS_EN | CFG_MANUAL_CS);
	XQSPIPS->CR = qspi->cfg;

	/* enable the controller */
	writel(1, QSPI_ENABLE);

	qspi->linear_mode = 0 /* fasle */;

	__DSB();

	return 0;
}

static void qspi_cs(struct qspi_ctxt *qspi, unsigned int cs)
{
	PRINTF("%s:\n", __func__);
	PRINTF("qspi_cs(%d)\n", cs);
	ASSERT(cs <= 1);

	if (cs == 0)
		qspi->cfg &= ~ (CFG_MANUAL_CS);
	else
		qspi->cfg |= CFG_MANUAL_CS;

	XQSPIPS->CR = qspi->cfg;
}

static void qspi_xmit(struct qspi_ctxt *qspi)
{
	PRINTF("%s:\n", __func__);
	// start txn
	writel(qspi->cfg | CFG_MANUAL_START, QSPI_CONFIG);

	TP();
	// wait for command to transmit and TX fifo to be empty
	while ((XQSPIPS->SR & TX_FIFO_NOT_FULL) == 0)
		;
	TP();
}

static void qspi_flush_rx(void)
{
	PRINTF("%s:\n", __func__);
	TP();
	while (!(XQSPIPS->SR & RX_FIFO_NOT_EMPTY))
	{
		TP();
		readl(QSPI_RXDATA);
	}
	TP();
}

static const uint32_t TXFIFO[] = { QSPI_TXD1, QSPI_TXD2, QSPI_TXD3, QSPI_TXD0, QSPI_TXD0, QSPI_TXD0 };

static void qspi_rd(struct qspi_ctxt *qspi, uint32_t cmd, uint32_t asize, uint32_t *data, uint32_t count)
{
	PRINTF("%s:\n", __func__);
	uint32_t sent = 0;
	uint32_t rcvd = 0;

	ASSERT(qspi);
	ASSERT(asize < 6);

	qspi_cs(qspi, 0);

	writel(cmd, TXFIFO[asize]);
	TP();
	qspi_xmit(qspi);

	if (asize == 4) { // dummy byte
		writel(0, QSPI_TXD1);
		TP();
		qspi_xmit(qspi);
		qspi_flush_rx();
	}

	qspi_flush_rx();

	while (rcvd < count) {
		while (XQSPIPS->SR & RX_FIFO_NOT_EMPTY) {
			*data++ = readl(QSPI_RXDATA);
			rcvd++;
		}
		TP();
		while ((XQSPIPS->SR & TX_FIFO_NOT_FULL) && (sent < count)) {
			writel(0, QSPI_TXD0);
			sent++;
		}
		TP();
		qspi_xmit(qspi);
	}
	qspi_cs(qspi, 1);
}

static void qspi_wr(struct qspi_ctxt *qspi, uint32_t cmd, uint32_t asize, uint32_t *data, uint32_t count)
{
	PRINTF("%s:\n", __func__);
	uint32_t sent = 0;
	uint32_t rcvd = 0;

	ASSERT(qspi);
	ASSERT(asize < 6);

	qspi_cs(qspi, 0);

	writel(cmd, TXFIFO[asize]);
	TP();
	qspi_xmit(qspi);

	if (asize == 4) { // dummy byte
		writel(0, QSPI_TXD1);
		TP();
		qspi_xmit(qspi);
		qspi_flush_rx();
	}

	qspi_flush_rx();

	while (rcvd < count) {
		TP();
		while (XQSPIPS->SR & RX_FIFO_NOT_EMPTY) {
			readl(QSPI_RXDATA); // discard
			rcvd++;
		}
		TP();
		while ((XQSPIPS->SR & TX_FIFO_NOT_FULL) && (sent < count)) {
			writel(*data++, QSPI_TXD0);
			sent++;
		}
		TP();
		qspi_xmit(qspi);
	}

	qspi_cs(qspi, 1);
}

static void qspi_wr1(struct qspi_ctxt *qspi, uint32_t cmd)
{
	PRINTF("%s:\n", __func__);
	ASSERT(qspi);

	qspi_cs(qspi, 0);
	writel(cmd, QSPI_TXD1);
	TP();
	qspi_xmit(qspi);

	TP();
	while (!(XQSPIPS->SR & RX_FIFO_NOT_EMPTY))
		;
	TP();

	readl(QSPI_RXDATA);
	qspi_cs(qspi, 1);
}

static void qspi_wr2(struct qspi_ctxt *qspi, uint32_t cmd)
{
	PRINTF("%s:\n", __func__);
	ASSERT(qspi);

	qspi_cs(qspi, 0);
	writel(cmd, QSPI_TXD2);
	TP();
	qspi_xmit(qspi);

	TP();
	while (!(XQSPIPS->SR & RX_FIFO_NOT_EMPTY))
		;
	TP();

	readl(QSPI_RXDATA);
	qspi_cs(qspi, 1);
}

static void qspi_wr3(struct qspi_ctxt *qspi, uint32_t cmd)
{
	PRINTF("%s:\n", __func__);
	ASSERT(qspi);

	qspi_cs(qspi, 0);
	writel(cmd, QSPI_TXD3);
	TP();
	qspi_xmit(qspi);

	TP();
	while (!(XQSPIPS->SR & RX_FIFO_NOT_EMPTY))
		;
	TP();

	readl(QSPI_RXDATA);
	qspi_cs(qspi, 1);
}

static uint32_t qspi_rd1(struct qspi_ctxt *qspi, uint32_t cmd)
{
	PRINTF("%s:\n", __func__);
	qspi_cs(qspi, 0);
	writel(cmd, QSPI_TXD2);
	TP();
	qspi_xmit(qspi);

	TP();
	while (!(XQSPIPS->SR & RX_FIFO_NOT_EMPTY))
		;
	TP();

	qspi_cs(qspi, 1);
	return readl(QSPI_RXDATA);
}

/////

// https://github.com/grub4android/lk/blob/579832fe57eeb616cefd82b93d991141f0db91ce/platform/zynq/spiflash.c

static uint32_t qspi_rd_cr1(struct qspi_ctxt *qspi)
{
	PRINTF("%s:\n", __func__);
	return qspi_rd1(qspi, 0x35) >> 24;
}

static uint32_t qspi_rd_status(struct qspi_ctxt *qspi)
{
	PRINTF("%s:\n", __func__);
	return qspi_rd1(qspi, 0x05) >> 24;
}

////
static uint32_t InitQspi(void);
static uint32_t QspiAccess( uint32_t SourceAddress,
		void * DestinationAddress,
		uint32_t LengthBytes);
static uint32_t InitQspi(void);
static void flashPrepareLqspiCR(uint_fast8_t enableMmap);

static uint32_t QspiAccess( uint32_t SourceAddress,
		void * DestinationAddress,
		uint32_t LengthBytes);

static uint32_t FlashReadID(void);
static uint32_t SendBankSelect(uint8_t BankSel);

#define LQSPI_CLK_CTRL_DIVISOR_VALUE 8

void spidf_initialize(void)
{

	PRINTF("%s:\n", __func__);

	SCLR->SLCR_UNLOCK = 0x0000DF0DU;
	SCLR->APER_CLK_CTRL |= (0x01uL << 23);	// APER_CLK_CTRL.LQSPI_CPU_1XCLKACT
	(void) SCLR->APER_CLK_CTRL;

	SCLR->LQSPI_CLK_CTRL = (SCLR->LQSPI_CLK_CTRL & ~ 0x3F00) |
			(LQSPI_CLK_CTRL_DIVISOR_VALUE << 8) |
			0;

	PRINTF("spidf_initialize: xc7z_get_qspi_freq()=%lu\n", xc7z_get_qspi_freq());

	SPIDF_HARDINITIALIZE();
	InitQspi();
	return;

	PRINTF("1 XQSPIPS->CR=%08lX\n", XQSPIPS->CR);
	// после reset не работает
//	SCLR->LQSPI_RST_CTRL |= 0x01;
//	(void) SCLR->LQSPI_RST_CTRL;
//	SCLR->LQSPI_RST_CTRL &= ~ 0x01;
//	(void) SCLR->LQSPI_RST_CTRL;

	PRINTF("2 XQSPIPS->CR=%08lX\n", XQSPIPS->CR);

	XQSPIPS->CR |= (1uL << 19);		// Holdb_dr

	ASSERT(XQSPIPS->MOD_ID == 0x01090101);
	//PRINTF("spidf_initialize: MOD_ID=%08lX (expected 0x01090101)\n", XQSPIPS->MOD_ID);

	XQSPIPS->ER = 0;
	XQSPIPS->LQSPI_CR = 0;

	// flush rx fifo
	while ((XQSPIPS->SR & RX_FIFO_NOT_EMPTY) != 0)
		(void) XQSPIPS->RXD;

	XQSPIPS->CR = (XQSPIPS->CR & CFG_NO_MODIFY_MASK) |
	            //CFG_IFMODE |	// 1: Flash memory interface mode
				CFG_LITTLE_ENDIAN | // zero value
	            CFG_HOLDB_DR |	// D2 & D3 in 1 mit mode behaviour
	            CFG_FIFO_WIDTH_32 |	// Must be set to 2'b11 (32bits).
	            CFG_CPHA | 	// 1: the QSPI clock is inactive outside the word
				CFG_CPOL |	// 1: The QSPI clock is quiescent high
	            CFG_MASTER_MODE |	// 1: The QSPI is in master mode
				CFG_BAUD_DIV_4 |
	            //CFG_MANUAL_START_EN | // 1: enables manual start
				CFG_MANUAL_CS_EN |	// 1: manual CS mode
				CFG_MANUAL_CS |	// Peripheral chip select line, directly drive n_ss_out if Manual_C is set
				0;

	(void) XQSPIPS->CR;
	//qspi->khz = 100000;
	//qspi->linear_mode = 0 /* fasle */;
	PRINTF("3 XQSPIPS->CR=%08lX\n", XQSPIPS->CR);

	//writel(1, QSPI_ENABLE);
	//XQSPIPS->ER = 1;

	// clear sticky irqs
	//writel(TX_UNDERFLOW | RX_OVERFLOW, QSPI_IRQ_STATUS);
	XQSPIPS->SR = TX_UNDERFLOW | RX_OVERFLOW;

	SPIDF_HARDINITIALIZE();
}

void spidf_hangoff(void)
{
#if CPUSTYLE_XC7Z
	return;
#endif /* CPUSTYLE_XC7Z */
	SPIDF_HANGOFF();	// Отключить процессор от SERIAL FLASH
}

void spidf_uninitialize(void)
{
//	while ((QUADSPI->SR & QUADSPI_SR_BUSY_Msk) != 0)
//		;
	// Disconnect I/O pins
#if CPUSTYLE_XC7Z
	return;
#endif /* CPUSTYLE_XC7Z */
	XQSPIPS->ER = 0;
	SPIDF_HANGOFF();
}

static void spidf_unselect(void)
{
////	while ((SPIBSC0.CMNSR & SPIBSC_CMNSR_TEND) == 0)
////		;
//	while ((SPIBSC0.CMNSR & SPIBSC_CMNSR_SSLF) != 0)
//		;
	// Disconnect I/O pins
	//qspi_cs(& qspi0, 1);
	XQSPIPS->CR |= CFG_MANUAL_CS;	 // De-assert
	(void) XQSPIPS->CR;

	XQSPIPS->ER = 0;
	SPIDF_HANGOFF();
}

static void spidf_progval8_p1(uint_fast8_t v)
{
	XQSPIPS->TXD_01 = v; 	// Data to TX FIFO, for 1-byte instruction, not for normal data transfer.
}

static unsigned spidf_progval8_p2(uint_fast8_t v)
{
	while ((XQSPIPS->SR & TX_FIFO_NOT_FULL) == 0)
		;
	while ((XQSPIPS->SR & RX_FIFO_NOT_EMPTY) == 0)
		;
	unsigned v2 = XQSPIPS->RXD;
	PRINTF("v2=%08lX\n", v2);

	XQSPIPS->TXD_01 = v; 	// Data to TX FIFO, for 1-byte instruction, not for normal data transfer.

	return v2;
}

static uint_fast8_t spidf_complete(void)
{
	while ((XQSPIPS->SR & TX_FIFO_NOT_FULL) == 0)
		;
	while ((XQSPIPS->SR & RX_FIFO_NOT_EMPTY) == 0)
		;
	unsigned vc = XQSPIPS->RXD;
	PRINTF("vc=%08lX\n", vc);
	return vc;
}

static uint_fast8_t spidf_progval8(uint_fast8_t v)
{
	XQSPIPS->TXD_01 = v; 	// Data to TX FIFO, for 1-byte instruction, not for normal data transfer.

	while ((XQSPIPS->SR & TX_FIFO_NOT_FULL) == 0)
		;
	while ((XQSPIPS->SR & RX_FIFO_NOT_EMPTY) == 0)
		;
	unsigned v8 = XQSPIPS->RXD;
	PRINTF("v8=%08lX\n", v8);
	return v8;
}

static void spidf_iostart(
	uint_fast8_t direction,	// 0: dataflash-to-memory, 1: Memory-to-dataflash
	uint_fast8_t cmd,
	uint_fast8_t readnb,	// признак работы по QSPI 4 bit - все кроме команды идет во 4-байтной шине
	uint_fast8_t ndummy,	// number of dummy bytes
	uint_fast32_t size,
	uint_fast8_t hasaddress,
	uint_fast32_t address
	)
{
	//PRINTF("spidf_iostart: dir=%d, cmd=%02X, readnb=%d, ndummy=%d, size=%lu, ha=%d, addr=%08lX\n", direction, cmd, readnb, ndummy, size, hasaddress, address);
	const unsigned cmdlen = 1 + (hasaddress ? 3 : 0) + ndummy;
	//PRINTF("spidf_iostart: cmdlen=%u\n", cmdlen);

	// Read data: cmd A23_A16 A15_A8 A7_A0
	const uint_fast32_t v =
			((uint_fast32_t) ((address >> 0) & 0xFF) << 24) |
			((uint_fast32_t) ((address >> 8) & 0xFF) << 16) |
			((uint_fast32_t) ((address >> 16) & 0xFF) << 8) |
			((uint_fast32_t) (cmd & 0xFF) << 0) |
			0;

	XQSPIPS->ER = 1;

	// Assert CS
	XQSPIPS->CR &= ~ CFG_MANUAL_CS;
	(void) XQSPIPS->CR;


	spidf_progval8_p1(cmd);		/* The Read SFDP instruction code is 0x5A */

	if (hasaddress)
	{
		spidf_progval8_p2(address >> 16);
		spidf_progval8_p2(address >> 8);
		spidf_progval8_p2(address >> 0);
	}
	while (ndummy --)
		spidf_progval8_p2(0x00);	// dummy byte

	spidf_complete();	/* done sending data to target chip */
	return;


	switch (cmdlen)
	{
	case 1:
		TP();
		XQSPIPS->TXD_01 = cmd; // Data to TX FIFO, for 1-byte instruction, not for normal data transfer.
		//XQSPIPS->CR |= CFG_MANUAL_START_EN;
		//(void) XQSPIPS->CR;
		//XQSPIPS->CR |= CFG_MANUAL_START;
		//(void) XQSPIPS->CR;
		TP();
		while ((XQSPIPS->SR & TX_FIFO_NOT_FULL) == 0)
			;
		TP();
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		XQSPIPS->TXD_00 = v;	// Data to TX FIFO, for 4-byte instruction for normal read/write data transfer.
		//XQSPIPS->CR |= CFG_MANUAL_START_EN;
		//(void) XQSPIPS->CR;
		//XQSPIPS->CR |= CFG_MANUAL_START;
		//(void) XQSPIPS->CR;
		TP();
		while ((XQSPIPS->SR & TX_FIFO_NOT_FULL) == 0)
			;
		break;
	case 5:
		TP();
		XQSPIPS->TXD_00 = v;	// Data to TX FIFO, for 4-byte instruction for normal read/write data transfer.
		//XQSPIPS->CR |= CFG_MANUAL_START_EN;
		//(void) XQSPIPS->CR;
		//XQSPIPS->CR |= CFG_MANUAL_START;
		//(void) XQSPIPS->CR;
		TP();
		while ((XQSPIPS->SR & TX_FIFO_NOT_FULL) == 0)
			;
		TP();
		XQSPIPS->TXD_01 = 0x00;	// dummy data
		//XQSPIPS->CR |= CFG_MANUAL_START_EN;
		//(void) XQSPIPS->CR;
		//XQSPIPS->CR |= CFG_MANUAL_START;
		//(void) XQSPIPS->CR;
		while ((XQSPIPS->SR & TX_FIFO_NOT_FULL) == 0)
			;
		TP();
		break;
	}


//	while ((XQSPIPS->SR & TX_FIFO_NOT_FULL) == 0)
//		;
}

static void spidf_read(uint8_t * buff, uint_fast32_t size)
{
	while (size --)
		* buff ++ = spidf_progval8(0xff);
}


static uint_fast8_t spidf_verify(const uint8_t * buff, uint_fast32_t size)
{
	uint_fast8_t err = 0;
	while (size --)
		err |= * buff ++ != spidf_progval8(0xff);
	return err;
}


static void spidf_write(const uint8_t * buff, uint_fast32_t size)
{
	while (size --)
		spidf_progval8(* buff ++);
}

#elif WIHSPIDFHW && (CPUSTYLE_STM32MP1 || CPUSTYLE_STM32F)


// вычитываем все заказанное количество
static void spidf_read(uint8_t * buff, uint_fast32_t size)
{
	while (size --)
	{
		while ((QUADSPI->SR & QUADSPI_SR_FTF_Msk) == 0)
			;
		* buff ++ = * (volatile uint8_t *) & QUADSPI->DR;
	}
}

// передаем все заказанное количество
static void spidf_write(const uint8_t * buff, uint_fast32_t size)
{
	while (size --)
	{
		while ((QUADSPI->SR & QUADSPI_SR_FTF_Msk) == 0)
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
		while ((QUADSPI->SR & QUADSPI_SR_FTF_Msk) == 0)
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
	uint_fast8_t readnb,	// признак работы по QSPI 4 bit - все кроме команды идет во 4-байтной шине
	uint_fast8_t ndummy,	// number of dummy bytes
	uint_fast32_t size,
	uint_fast8_t hasaddress,
	uint_fast32_t address
	)
{
	/* код ширины шины */
	static const uint8_t nbits [3] =
	{
			0x01,	// single line
			0x02,	// two lines
			0x03,	// four lines
	};
	/* за сколько тактов пройдет один dummy byte */
	static const uint8_t nmuls [3] =
	{
			8,	// single line
			4,	// two lines
			2,	// four lines
	};
	const uint_fast32_t bw = nbits [readnb];
	const uint_fast32_t ml = nmuls [readnb];

	while ((QUADSPI->SR & QUADSPI_SR_BUSY_Msk) != 0)
		;

	// Connect I/O pins
	SPIDF_HARDINITIALIZE();

	//QUADSPI->AR = address;
	QUADSPI->DLR = size ? (size - 1) : 0;
	(void) QUADSPI->DLR;

	//PRINTF("QUADSPI->DR=%08x\n", QUADSPI->DR);
	QUADSPI->FCR = QUADSPI_FCR_CTCF_Msk;	// Clear Transfer Complete Flag
	(void) QUADSPI->FCR;
	QUADSPI->FCR = QUADSPI_FCR_CTEF_Msk;	// Clear Transfer Error Flag
	(void) QUADSPI->FCR;

	ASSERT((QUADSPI->SR & QUADSPI_SR_BUSY_Msk) == 0);

	QUADSPI->CCR =
		//(0 << QUADSPI_CCR_DDRM_Pos) |	// 0: DDR Mode disabled
		//(0 << QUADSPI_CCR_DHHC_Pos) |	// 0: Delay the data output using analog delay
		//(0 << QUADSPI_CCR_FRCM_Pos) |	// 0: Normal mode
		//(0 << QUADSPI_CCR_SIOO_Pos) |	// 0: Send instruction on every transaction
		((direction ? 0x00uL : 0x01uL) << QUADSPI_CCR_FMODE_Pos) |	// 01: Indirect read mode, 00: Indirect write mode
		(size != 0) * (bw << QUADSPI_CCR_DMODE_Pos) |	// 01: Data on a single line
		((ml * ndummy) << QUADSPI_CCR_DCYC_Pos) |	// This field defines the duration of the dummy phase (1..31).
		//0 * (bw << QUADSPI_CCR_ABSIZE_Pos) |	// 00: 8-bit alternate byte
		(0 << QUADSPI_CCR_ABMODE_Pos) |	// 00: No alternate bytes
		(0x02uL << QUADSPI_CCR_ADSIZE_Pos) |	// 010: 24-bit address
		(hasaddress != 0) * (bw << QUADSPI_CCR_ADMODE_Pos) |	// 01: Address on a single line
		(0x01uL << QUADSPI_CCR_IMODE_Pos) |	// 01: Instruction on a single line
		((uint_fast32_t) cmd << QUADSPI_CCR_INSTRUCTION_Pos) |	// Instruction to be send to the external SPI device.
		0;

	ASSERT(((QUADSPI->CCR & QUADSPI_CCR_INSTRUCTION_Msk) >> QUADSPI_CCR_INSTRUCTION_Pos) == cmd);

	if ((QUADSPI->CCR & QUADSPI_CCR_ADMODE_Msk) != 0)
	{
		// Initiate operation
		QUADSPI->AR = address & 0x00FFFFFF;	// В indirect режимах адрес должен быть в допустимых для указанного при ините размера памяти
		//PRINTF("spidf_iostart QUADSPI->AR=%08lX, QUADSPI->SR=%08lX\n", QUADSPI->AR, QUADSPI->SR);
		(void) QUADSPI->AR;
	}
}

void spidf_initialize(void)
{
#if CPUSTYLE_STM32MP1
	RCC->MP_AHB6ENSETR = RCC_MP_AHB6ENSETR_QSPIEN;
	(void) RCC->MP_AHB6ENSETR;
	RCC->MP_AHB6LPENSETR = RCC_MP_AHB6LPENSETR_QSPILPEN;
	(void) RCC->MP_AHB6LPENSETR;

#elif CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	RCC->AHB3ENR |= RCC_AHB3ENR_QSPIEN_Msk;
	(void) RCC->AHB3ENR;
	RCC->AHB3LPENR |= RCC_AHB3LPENR_QSPILPEN_Msk;
	(void) RCC->AHB3LPENR;

#else
	#error Add QSPI init code

#endif
	// Connect I/O pins
	//SPIDF_HARDINITIALIZE();

	QUADSPI->CR &= ~ QUADSPI_CR_EN_Msk;
	(void) QUADSPI->CR;

	QUADSPI->CCR = 0;
	(void) QUADSPI->CCR;

	// qspipre in range 1..256
	const unsigned long qspipre = ulmax32(1, ulmin32(calcdivround2(BOARD_QSPI_FREQ, SPISPEEDUFAST), 256));
	//PRINTF("spidf_initialize: qspipre=%lu\n", qspipre);

	QUADSPI->DCR = ((QUADSPI->DCR & ~ (QUADSPI_DCR_FSIZE_Msk | QUADSPI_DCR_CSHT_Msk | QUADSPI_DCR_CKMODE_Msk))) |
		(23 << QUADSPI_DCR_FSIZE_Pos) |	// FSIZE+1 is effectively the number of address bits required to address the Flash memory.
		(7 << QUADSPI_DCR_CSHT_Pos) |	// 0: nCS stays high for at least 1 cycle between Flash memory commands
		//(0 << QUADSPI_DCR_CKMODE_Pos) |	// 0: CLK must stay low while nCS is high (chip select released). This is referred to as mode 0.
		(1 << QUADSPI_DCR_CKMODE_Pos) |	// 1: CLK must stay high while nCS is high (chip select released). This is referred to as mode 3.
		0;
	(void) QUADSPI->DCR;

	QUADSPI->CR = ((QUADSPI->CR & ~ (QUADSPI_CR_PRESCALER_Msk | QUADSPI_CR_FTHRES_Msk | QUADSPI_CR_EN_Msk))) |
		(0x00uL << QUADSPI_CR_FTHRES_Pos) | // FIFO threshold level - one byte
		(((unsigned long) qspipre - 1) << QUADSPI_CR_PRESCALER_Pos) |
		0;
	(void) QUADSPI->CR;

	QUADSPI->CR |= QUADSPI_CR_EN_Msk;
	(void) QUADSPI->CR;
}

void spidf_uninitialize(void)
{
	while ((QUADSPI->SR & QUADSPI_SR_BUSY_Msk) != 0)
		;
	// Disconnect I/O pins
	SPIDF_HANGOFF();
}

void spidf_hangoff(void)
{
	SPIDF_HANGOFF();	// Отключить процессор от SERIAL FLASH
}

#elif WIHSPIDFHW && CPUSTYLE_R7S721

// передаем все заказанное количество
static void spidf_write(const uint8_t * buff, uint_fast32_t size)
{
	while ((SPIBSC0.CMNSR & SPIBSC_CMNSR_TEND) == 0)
		;
	/* подготовка к следующему шагу */
	// продолжение обмена без передачи команды, адреса... но с передаяей данных.
	SPIBSC0.SMENR =
			(SPIBSC0.SMENR & ~ (SPIBSC_SMENR_ADE | SPIBSC_SMENR_CDE | SPIBSC_SMENR_DME | SPIBSC_SMENR_OPDE | SPIBSC_SMENR_SPIDE)) |
			(0x08uL << SPIBSC_SMENR_SPIDE_SHIFT) |
			0;

	while (size --)
	{
		while ((SPIBSC0.CMNSR & SPIBSC_CMNSR_TEND) == 0)
			;
		SPIBSC0.SMWDR0.UINT8 [R_IO_LL] = * buff ++;

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

void spidf_hangoff(void)
{
	SPIDF_HANGOFF();	// Отключить процессор от SERIAL FLASH
}

static void spidf_iostart(
	uint_fast8_t direction,	// 0: dataflash-to-CPU, 1: CPU-to-dataflash
	uint_fast8_t cmd,
	uint_fast8_t readnb,	// признак работы по QSPI 4 bit - все кроме команды идет во 4-байтной шине
	uint_fast8_t ndummy,	// number of dummy bytes
	uint_fast32_t size,
	uint_fast8_t hasaddress,
	uint_fast32_t address
	)
{
	ASSERT(readnb == SPDFIO_1WIRE);
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

/* снять защиту записи для следующей команды */
void writeEnableDATAFLASH(void)
{
	spidf_iostart(SPDIFIO_WRITE, 0x06, SPDFIO_1WIRE, 0, 0, 0, 0);	/* 0x06: write enable */
	spidf_unselect();	/* done sending data to target chip */
}

void writeDisableDATAFLASH(void)
{
	spidf_iostart(SPDIFIO_WRITE, 0x04, SPDFIO_1WIRE, 0, 0, 0, 0);	/* 0x04: write disable */
	spidf_unselect();	/* done sending data to target chip */
}

int fullEraseDATAFLASH(void)
{
	if (timed_dataflash_read_status())
	{
		PRINTF(PSTR("fullEraseDATAFLASH: timeout\n"));
		return 1;
	}

	writeEnableDATAFLASH();		/* write enable */

	spidf_iostart(SPDIFIO_WRITE, 0x60, SPDFIO_1WIRE, 0, 0, 0, 0);	/*.2.18 Chip Erase (C7h / 60h) */
	spidf_unselect();	/* done sending data to target chip */

	if (largetimed_dataflash_read_status())
		return 1;
	return 0;
}

/* read status register #1 (bit0==busy flag) */
uint_fast8_t dataflash_read_status(void)
{
	uint8_t v;
	enum { SPDIF_IOSIZE = sizeof v };

	spidf_iostart(SPDIFIO_READ, 0x05, SPDFIO_1WIRE, 0, SPDIF_IOSIZE, 0, 0x00000000);	/* read status register */
	spidf_read(& v, SPDIF_IOSIZE);
	spidf_unselect();	/* done sending data to target chip */
	//PRINTF("dataflash_read_status: v=%02X\n", v);
	return v;
}

int timed_dataflash_read_status(void)
{
	unsigned long w = 400000;
	while (w --)
	{
		if ((dataflash_read_status() & 0x01) == 0)
			return 0;
	}
	PRINTF(PSTR("DATAFLASH timeout error\n"));
	return 1;
}

// infinity waiting
int largetimed_dataflash_read_status(void)
{
	unsigned long w = 4000000;
	while (w)
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
static void readSFDPDATAFLASH(unsigned long flashoffset, uint8_t * buff, unsigned size)
{
	// Read SFDP
	spidf_iostart(SPDIFIO_READ, 0x5A, SPDFIO_1WIRE, 1, size, 1, flashoffset);	// READ SFDP (with dummy bytes)
	spidf_read(buff, size);
	spidf_unselect();	/* done sending data to target chip */
}

static int seekparamSFDPDATAFLASH(unsigned long * paramoffset, uint_fast8_t * paramlength, uint_fast8_t id, uint_fast8_t lastnum)
{
	uint8_t buff8 [8];
	unsigned i;

	for (i = 0; i <= lastnum; ++ i)
	{
		readSFDPDATAFLASH((i + 1) * 8uL, buff8, 8);
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

static void modeDATAFLASH(uint_fast16_t dw, const char * title, int buswID)
{
	const unsigned bw = 1u << buswID;
	const unsigned ndmy = (((dw >> 5) & 0x07) * bw + ((dw >> 0) & 0x1F) * bw) / 8;
	switch ((dw >> 8) & 0xFF)
	{
	case 0x00:
	case 0xFF:
		//PRINTF("SFDP: %s not supported\n", title);
		break;
	default:
		//PRINTF("SFDP: %s Opcode=%02X, mobbits=%u, ws=%u, ndmy=%u\n", title, (dw >> 8) & 0xFF, (dw >> 5) & 0x07, (dw >> 0) & 0x1F, ndmy);
		readxb [buswID] = (dw >> 8) & 0xFF;	// opcode
		dmyb [buswID] = ndmy;	// dummy bytes
		break;
	}
}

static uint_fast8_t sectorEraseCmd = 0xD8;			// 64KB SECTOR ERASE
static uint_fast32_t sectorSize = (1uL << 16);		// default sectoir size 64kB
static uint_fast32_t chipSize = BOOTLOADER_FLASHSIZE;	// default chip size

char nameDATAFLASH [64];

int testchipDATAFLASH(void)
{
#if CPUSTYLE_XC7Z
	return 0;
#endif /* CPUSTYLE_XC7Z */
	unsigned char mf_id;	// Manufacturer ID
	unsigned char mf_devid1;	// device ID (part 1)
	unsigned char mf_devid2;	// device ID (part 2)
	unsigned char mf_dlen;	// Extended Device Information String Length

	/* Ожидание бита ~RDY в слове состояния. Для FRAM не имеет смысла.
	Вставлено для возможности использования DATAFLASH */

	if (timed_dataflash_read_status())
	{
		PRINTF(PSTR("testchipDATAFLASH: timeout\n"));
		return 1;
	}

	{

		uint8_t mfa [4];

		enum { SPDIF_IOSIZE = sizeof mfa };
		spidf_iostart(SPDIFIO_READ, 0x9F, SPDFIO_1WIRE, 0, SPDIF_IOSIZE, 0, 0x00000000);	/* read id register */
		spidf_read(mfa, SPDIF_IOSIZE);
		spidf_unselect();	/* done sending data to target chip */

		mf_id = mfa [0];
		mf_devid1 = mfa [1];
		mf_devid2 = mfa [2];
		mf_dlen = mfa [3];

		//PRINTF(PSTR("spidf: ID=0x%02X devId=0x%02X%02X, mf_dlen=0x%02X\n"), mf_id, mf_devid1, mf_devid2, mf_dlen);
	}


	local_snprintf_P(nameDATAFLASH, ARRAY_SIZE(nameDATAFLASH),
			PSTR("%s, SPIDF:%02X:%02X%02X:%02X"),
			USBD_DFU_FLASHNAME,
			//(unsigned) (chipSize / 1024 / (1024 / 8)),
			mf_id, mf_devid1, mf_devid2, mf_dlen
			);


	// Read root SFDP
	uint8_t buff8 [8];
	readSFDPDATAFLASH(0x000000, buff8, 8);

	const uint_fast32_t signature = USBD_peek_u32(& buff8 [0]);

	//PRINTF(PSTR("SFDP: signature=%08lX, lastparam=0x%02X\n"), signature, buff8 [6]);
	if (signature == 0x50444653)
	{
		// Serial Flash Discoverable Parameters (SFDP), for Serial NOR Flash
		const uint_fast8_t lastparam = buff8 [6];
		unsigned long ptp;
		uint_fast8_t len4;
		if (seekparamSFDPDATAFLASH(& ptp, & len4, 0x00, lastparam))
		{
			PRINTF("SFDP parameter 0x00 not found\n");
			return 0;
		}

		//PRINTF("SFDP: ptp=%08lX, len4=%02X\n", ptp, len4);
		if (len4 < 9 || len4 > 16)
			return 0;
		uint8_t buff32 [len4 * 4];
		readSFDPDATAFLASH(ptp, buff32, len4 * 4);
		//const uint_fast32_t dword1 = USBD_peek_u32(buff32 + 4 * 0);
		const uint_fast32_t dword2 = USBD_peek_u32(buff32 + 4 * 1);
		const uint_fast32_t dword3 = USBD_peek_u32(buff32 + 4 * 2);
		const uint_fast32_t dword4 = USBD_peek_u32(buff32 + 4 * 3);
		const uint_fast32_t dword5 = USBD_peek_u32(buff32 + 4 * 4);
		const uint_fast32_t dword6 = USBD_peek_u32(buff32 + 4 * 5);
		const uint_fast32_t dword7 = USBD_peek_u32(buff32 + 4 * 6);
		const uint_fast32_t dword8 = USBD_peek_u32(buff32 + 4 * 7);
		const uint_fast32_t dword9 = USBD_peek_u32(buff32 + 4 * 8);
		//printhex(ptp, buff32, 256);

		///////////////////////////////////
		/* Print density information. */
		if ((dword2 & 0x80000000uL) == 0)
		{
			const unsigned Kbi = (dword2 >> 10) + 1;
			const unsigned MB = (dword2 >> 23) + 1;
			//PRINTF("SFDP: density=%08lX (%u Kbi, %u MB)\n", dword2, Kbi, MB);
			chipSize = (dword2 >> 3) + 1uL;
		}
		else
		{
			const unsigned Mbi = 1u << ((dword2 & 0x7FFFFFFF) - 10);
			const unsigned MB = 1u << ((dword2 & 0x7FFFFFFF) - 10 - 3);
			//PRINTF("SFDP: density=%08lX (%u Mbi, %u MB)\n", dword2, Mbi, MB);
			chipSize = 1uL << ((dword2 & 0x7FFFFFFF) - 3);
		}
		///////////////////////////////////
		// dword8, dword9 - 4KB Erase opcode, Sector size, Sector erase opcode
		// Автоматическое определение наибольшего размера сектора
		unsigned sct [4];
		sct [0] = (dword8 >> 0) & 0xFFFF;
		sct [1] = (dword8 >> 16) & 0xFFFF;
		sct [2] = (dword9 >> 0) & 0xFFFF;
		sct [3] = (dword9 >> 16) & 0xFFFF;
		//PRINTF("SFDP: opcd1..4: 0x%02X, 0x%02X, 0x%02X, 0x%02X\n", (sct [0] >> 8) & 0xFF, (sct [1] >> 8) & 0xFF, (sct [2] >> 8) & 0xFF, (sct [3] >> 8) & 0xFF);
		//PRINTF("SFDP: size1..4: %lu, %lu, %lu, %lu\n", 1uL << (sct [0] & 0xFF), 1uL << (sct [1] & 0xFF), 1uL << (sct [2] & 0xFF), 1uL << (sct [3] & 0xFF));
		unsigned i;
		unsigned sctRESULT = 0;
		for (i = 0; i < ARRAY_SIZE(sct); ++ i)
		{
			const unsigned newsct = sct [i];
			if ((newsct & 0xFF) == 0)
				continue;
			if (sctRESULT == 0 || (sctRESULT & 0xFF) > (newsct & 0xFF))
				sctRESULT = newsct;
		}
		if (sctRESULT != 0)
		{
			sectorEraseCmd = (sctRESULT >> 8) & 0xFF;
			sectorSize = 1uL << (sctRESULT & 0xFF);
			//PRINTF("SFDP: Selected opcode=0x%02X, size=%lu\n", (unsigned) sectorEraseCmd, (unsigned long) sectorSize);
		}
		///////////////////////////////////
		//PRINTF("SFDP: Sector Type 1 Size=%08lX, Sector Type 1 Opcode=%02lX\n", 1uL << ((dword8 >> 0) & 0xFF), (dword8 >> 8) & 0xFF);
		// установка кодов операции
		modeDATAFLASH(dword3 >> 0, "(1-4-4) Fast Read", SPDFIO_4WIRE);
		modeDATAFLASH(dword4 >> 16, "(1-2-2) Fast Read", SPDFIO_2WIRE);
	}

	local_snprintf_P(nameDATAFLASH, ARRAY_SIZE(nameDATAFLASH),
			PSTR("x25%c%u, SPIDF:%02X:%02X%02X:%02X"),
#if WIHSPIDFHW4BIT
			'Q',
#else /* WIHSPIDFHW4BIT */
			'F',
#endif /* WIHSPIDFHW4BIT */
			(unsigned) (chipSize / 1024 / (1024 / 8)),
			mf_id, mf_devid1, mf_devid2, mf_dlen
			);

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
		spidf_iostart(SPDIFIO_WRITE, 0x01, SPDFIO_1WIRE, 0, 1, 0, 0);	/* Write Status Register */
		spidf_write(& v, 1);
		spidf_unselect();	/* done sending data to target chip */
	}

	return timed_dataflash_read_status();
}

unsigned long sectorsizeDATAFLASH(void)
{
	return sectorSize;
}

unsigned long chipsizeDATAFLASH(void)
{

	return chipSize;
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
	spidf_iostart(SPDIFIO_WRITE, sectorEraseCmd, SPDFIO_1WIRE, 0, 0, 1, flashoffset);
	spidf_unselect();	/* done sending data to target chip */
	if (timed_dataflash_read_status())
		return 1;
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

	spidf_iostart(SPDIFIO_WRITE, 0x02, SPDFIO_1WIRE, 0, len, 1, flashoffset);		/* Page Program */
	spidf_write(data, len);
	spidf_unselect();	/* done sending data to target chip */

	//PRINTF(PSTR( Prog to address %08lX %02X done\n"), flashoffset, len);
	if (timed_dataflash_read_status())
		return 1;
	return 0;
}

int writeDATAFLASH(unsigned long flashoffset, const uint8_t * data, unsigned long len)
{
	//PRINTF(PSTR("Write to address %08lX %02X\n"), flashoffset, len);
	while (len != 0)
	{
		const unsigned long offset = flashoffset & 0xFF;
		const unsigned long part = ulmin32(len, ulmin32(256, 256 - offset));

		if (writesinglepageDATAFLASH(flashoffset, data, part))
			return 1;

		len -= part;
		flashoffset += part;
		data += part;
	}
	//PRINTF(PSTR("Write to address %08lX %02X done\n"), flashoffset, len);
	return 0;
}

static void
spdif_iostartread(unsigned long len, unsigned long flashoffset)
{
	if (0)
		;
#if WIHSPIDFHW4BIT
	else if (readxb [SPDFIO_4WIRE] != 0x00)
		spidf_iostart(SPDIFIO_READ, readxb [SPDFIO_4WIRE], SPDFIO_4WIRE, dmyb [SPDFIO_4WIRE], len, 1, flashoffset);	/* 0xEB: Fast Read Quad I/O */
#endif /* WIHSPIDFHW4BIT */
#if WIHSPIDFHW2BIT
	else if (readxb [SPDFIO_2WIRE] != 0x00)
		spidf_iostart(SPDIFIO_READ, readxb [SPDFIO_2WIRE], SPDFIO_2WIRE, dmyb [SPDFIO_2WIRE], len, 1, flashoffset);	/* 0xBB: Fast Read Dual I/O */
#endif /* WIHSPIDFHW2BIT */
	else
		spidf_iostart(SPDIFIO_READ, 0x03, SPDFIO_1WIRE, 0, len, 1, flashoffset);	/* 0x03: sequential read block */
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

	spdif_iostartread(len, flashoffset);
	err = spidf_verify(data, len);
	spidf_unselect();	/* done sending data to target chip */

	if (err)
		PRINTF(PSTR("verifyDATAFLASH: Done compare, have errors\n"));

	return err;
}

int readDATAFLASH(unsigned long flashoffset, uint8_t * data, unsigned long len)
{
#if CPUSTYLE_XC7Z
	flashPrepareLqspiCR(0);
	QspiAccess(flashoffset, data, len);
	return 0;
#endif /* CPUSTYLE_XC7Z */
	//PRINTF("readDATAFLASH start, data=%p, len=%lu\n", data, len);
	if (timed_dataflash_read_status())
	{
		PRINTF("readDATAFLASH: timeout\n");
		return 1;
	}

	spdif_iostartread(len, flashoffset);
	spidf_read(data, len);
	spidf_unselect();	/* done sending data to target chip */
	//PRINTF("readDATAFLASH done\n");
	return 0;
}


void bootloader_readimage(unsigned long flashoffset, uint8_t * dest, unsigned Len)
{
	spidf_initialize();
	testchipDATAFLASH();	// устанока кодов опрерации для скоростных режимов
	readDATAFLASH(flashoffset, dest, Len);
	spidf_uninitialize();
}

void bootloader_chiperase(void)
{
	spidf_initialize();
	testchipDATAFLASH();	// устанока кодов опрерации для скоростных режимов
	fullEraseDATAFLASH();
	spidf_uninitialize();

}

#else /* WIHSPIDFHW || WIHSPIDFSW */

void bootloader_readimage(unsigned long flashoffset, uint8_t * dest, unsigned Len)
{
}


unsigned long sectorsizeDATAFLASH(void)
{
	return 4096;
}

char nameDATAFLASH [] = "NoChip";

int testchipDATAFLASH(void)
{
	return 1;
}

void spidf_initialize(void)
{

}

void spidf_uninitialize(void)
{

}

void spidf_hangoff(void)
{
}

#endif /* WIHSPIDFHW || WIHSPIDFSW */

#if CPUSTYLE_XC7Z

#include "xqspips_hw.h"
#include "xqspips.h"

/************************** Constant Definitions *****************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define QSPI_DEVICE_ID		XPAR_XQSPIPS_0_DEVICE_ID

/*
 * The following constants define the commands which may be sent to the FLASH
 * device.
 */
#define QUAD_READ_CMD		0x6B
#define READ_ID_CMD			0x9F

#define WRITE_ENABLE_CMD	0x06
#define BANK_REG_RD			0x16
#define BANK_REG_WR			0x17
/* Bank register is called Extended Address Reg in Micron */
#define EXTADD_REG_RD		0xC8
#define EXTADD_REG_WR		0xC5

#define COMMAND_OFFSET		0 /* FLASH instruction */
#define ADDRESS_1_OFFSET	1 /* MSB byte of address to read or write */
#define ADDRESS_2_OFFSET	2 /* Middle byte of address to read or write */
#define ADDRESS_3_OFFSET	3 /* LSB byte of address to read or write */
#define DATA_OFFSET			4 /* Start of Data for Read/Write */
#define DUMMY_OFFSET		4 /* Dummy byte offset for fast, dual and quad
				     reads */
#define DUMMY_SIZE			1 /* Number of dummy bytes for fast, dual and
				     quad reads */
#define RD_ID_SIZE			4 /* Read ID command + 3 bytes ID response */
#define BANK_SEL_SIZE		2 /* BRWR or EARWR command + 1 byte bank value */
#define WRITE_ENABLE_CMD_SIZE	1 /* WE command */
/*
 * The following constants specify the extra bytes which are sent to the
 * FLASH on the QSPI interface, that are not data, but control information
 * which includes the command and address
 */
#define OVERHEAD_SIZE		4

/*
 * The following constants specify the max amount of data and the size of the
 * the buffer required to hold the data and overhead to transfer the data to
 * and from the FLASH.
 */
#define DATA_SIZE		4096

#define LQSPI_CR_FAST_SFDPREAD			0x0000005A	/* Read SFDP Register */

/*
 * The following defines are for dual flash interface.
 */
#define LQSPI_CR_FAST_READ			0x0000000B
#define LQSPI_CR_FAST_DUAL_READ		0x0000003B
#define LQSPI_CR_FAST_QUAD_READ		0x0000006B /* Fast Quad Read output */
#define LQSPI_CR_1_DUMMY_BYTE		0x00000100 /* 1 Dummy Byte between
						     address and return data */

#define SINGLE_QSPI_CONFIG_FAST_SFDPREAD	(XQSPIPS_LQSPI_CR_LINEAR_MASK | \
					 LQSPI_CR_1_DUMMY_BYTE | \
					 LQSPI_CR_FAST_SFDPREAD)

#define SINGLE_QSPI_CONFIG_FAST_READ	(XQSPIPS_LQSPI_CR_LINEAR_MASK | \
					 LQSPI_CR_1_DUMMY_BYTE | \
					 LQSPI_CR_FAST_READ)

#define SINGLE_QSPI_CONFIG_FAST_DUAL_READ	(XQSPIPS_LQSPI_CR_LINEAR_MASK | \
					 LQSPI_CR_1_DUMMY_BYTE | \
					 LQSPI_CR_FAST_DUAL_READ)

#define SINGLE_QSPI_CONFIG_FAST_QUAD_READ	(XQSPIPS_LQSPI_CR_LINEAR_MASK | \
					 LQSPI_CR_1_DUMMY_BYTE | \
					 LQSPI_CR_FAST_QUAD_READ)

#define DUAL_QSPI_CONFIG_FAST_QUAD_READ	(XQSPIPS_LQSPI_CR_LINEAR_MASK | \
					 XQSPIPS_LQSPI_CR_TWO_MEM_MASK | \
					 XQSPIPS_LQSPI_CR_SEP_BUS_MASK | \
					 LQSPI_CR_1_DUMMY_BYTE | \
					 LQSPI_CR_FAST_QUAD_READ)

#define DUAL_STACK_CONFIG_FAST_READ	(XQSPIPS_LQSPI_CR_TWO_MEM_MASK | \
					 LQSPI_CR_1_DUMMY_BYTE | \
					 LQSPI_CR_FAST_READ)

#define DUAL_STACK_CONFIG_FAST_DUAL_READ	(XQSPIPS_LQSPI_CR_TWO_MEM_MASK | \
					 LQSPI_CR_1_DUMMY_BYTE | \
					 LQSPI_CR_FAST_DUAL_READ)

#define DUAL_STACK_CONFIG_FAST_QUAD_READ	(XQSPIPS_LQSPI_CR_TWO_MEM_MASK | \
					 LQSPI_CR_1_DUMMY_BYTE | \
					 LQSPI_CR_FAST_QUAD_READ)

#define SINGLE_QSPI_IO_CONFIG_FAST_READ	(LQSPI_CR_1_DUMMY_BYTE | \
					 LQSPI_CR_FAST_READ)

#define SINGLE_QSPI_IO_CONFIG_FAST_DUAL_READ	(LQSPI_CR_1_DUMMY_BYTE | \
					 LQSPI_CR_FAST_DUAL_READ)

#define SINGLE_QSPI_IO_CONFIG_FAST_QUAD_READ	(LQSPI_CR_1_DUMMY_BYTE | \
					 LQSPI_CR_FAST_QUAD_READ)

#define DUAL_QSPI_IO_CONFIG_FAST_QUAD_READ	(XQSPIPS_LQSPI_CR_TWO_MEM_MASK | \
					 XQSPIPS_LQSPI_CR_SEP_BUS_MASK | \
					 LQSPI_CR_1_DUMMY_BYTE | \
					 LQSPI_CR_FAST_QUAD_READ)

#define QSPI_BUSWIDTH_ONE	0U
#define QSPI_BUSWIDTH_TWO	1U
#define QSPI_BUSWIDTH_FOUR	2U

#define SINGLE_FLASH_CONNECTION			0
#define DUAL_STACK_CONNECTION			1
#define DUAL_PARALLEL_CONNECTION		2
#define FLASH_SIZE_16MB					0x1000000

/*
 * Bank mask
 */
#define BANKMASK 0xF000000

/*
 * Identification of Flash
 * Micron:
 * Byte 0 is Manufacturer ID;
 * Byte 1 is first byte of Device ID - 0xBB or 0xBA
 * Byte 2 is second byte of Device ID describes flash size:
 * 128Mbit : 0x18; 256Mbit : 0x19; 512Mbit : 0x20
 * Spansion:
 * Byte 0 is Manufacturer ID;
 * Byte 1 is Device ID - Memory Interface type - 0x20 or 0x02
 * Byte 2 is second byte of Device ID describes flash size:
 * 128Mbit : 0x18; 256Mbit : 0x19; 512Mbit : 0x20
 */

#define MICRON_ID		0x20
#define SPANSION_ID		0x01
#define WINBOND_ID		0xEF
#define MACRONIX_ID		0xC2
#define ISSI_ID			0x9D

#define FLASH_SIZE_ID_8M		0x14
#define FLASH_SIZE_ID_16M		0x15
#define FLASH_SIZE_ID_32M		0x16
#define FLASH_SIZE_ID_64M		0x17
#define FLASH_SIZE_ID_128M		0x18
#define FLASH_SIZE_ID_256M		0x19
#define FLASH_SIZE_ID_512M		0x20
#define FLASH_SIZE_ID_1G		0x21
/* Macronix size constants are different for 512M and 1G */
#define MACRONIX_FLASH_SIZE_ID_512M		0x1A
#define MACRONIX_FLASH_SIZE_ID_1G		0x1B

/*
 * Size in bytes
 */
#define FLASH_SIZE_8M			0x0100000
#define FLASH_SIZE_16M			0x0200000
#define FLASH_SIZE_32M			0x0400000
#define FLASH_SIZE_64M			0x0800000
#define FLASH_SIZE_128M			0x1000000
#define FLASH_SIZE_256M			0x2000000
#define FLASH_SIZE_512M			0x4000000
#define FLASH_SIZE_1G			0x8000000

/************************** Function Prototypes ******************************/
/************************** Variable Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

static XQspiPs QspiInstance;
static XQspiPs * const QspiInstancePtr = & QspiInstance;
static uint32_t QspiFlashSize = FLASH_SIZE_16M;
static uint32_t QspiFlashMake = WINBOND_ID;
static uint32_t FlashReadBaseAddress;
static uint8_t LinearBootDeviceFlag;
static uint8_t ReadBuffer [DATA_OFFSET + DUMMY_SIZE + DATA_SIZE];
static uint8_t WriteBuffer [DATA_OFFSET + DUMMY_SIZE];


/******************************************************************************
*
* This function reads serial FLASH ID connected to the SPI interface.
* It then deduces the make and size of the flash and obtains the
* connection mode to point to corresponding parameters in the flash
* configuration table. The flash driver will function based on this and
* it presently supports Micron and Spansion - 128, 256 and 512Mbit and
* Winbond 128Mbit
*
* @param	none
*
* @return	XST_SUCCESS if read id, otherwise XST_FAILURE.
*
* @note		None.
*
******************************************************************************/
static uint32_t FlashReadID(void)
{
	uint32_t Status;

	/*
	 * Read ID in Auto mode.
	 */
	WriteBuffer[COMMAND_OFFSET]   = READ_ID_CMD;
	WriteBuffer[ADDRESS_1_OFFSET] = 0x00;		/* 3 dummy bytes */
	WriteBuffer[ADDRESS_2_OFFSET] = 0x00;
	WriteBuffer[ADDRESS_3_OFFSET] = 0x00;

	Status = XQspiPs_PolledTransfer(QspiInstancePtr, WriteBuffer, ReadBuffer,
				RD_ID_SIZE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	PRINTF("Single Flash Information\n");

	PRINTF("FlashID=0x%x 0x%x 0x%x\n", ReadBuffer[1],
			ReadBuffer[2],
			ReadBuffer[3]);

	/*
	 * Deduce flash make
	 */
//	if (ReadBuffer[1] == MICRON_ID) {
//		QspiFlashMake = MICRON_ID;
//		PRINTF( "MICRON ");
//	} else if(ReadBuffer[1] == SPANSION_ID) {
//		QspiFlashMake = SPANSION_ID;
//		PRINTF( "SPANSION ");
//	} else if(ReadBuffer[1] == WINBOND_ID) {
//		QspiFlashMake = WINBOND_ID;
//		PRINTF( "WINBOND ");
//	} else if(ReadBuffer[1] == MACRONIX_ID) {
//		QspiFlashMake = MACRONIX_ID;
//		PRINTF( "MACRONIX ");
//	} else if(ReadBuffer[0] == ISSI_ID) {
//		QspiFlashMake = ISSI_ID;
//		PRINTF( "ISSI ");
//	}

	/*
	 * Deduce flash Size
	 */
//	if (ReadBuffer[2] == FLASH_SIZE_ID_8M) {
//		QspiFlashSize = FLASH_SIZE_8M;
//		PRINTF( "8M Bits\n");
//	} else if (ReadBuffer[2] == FLASH_SIZE_ID_16M) {
//		QspiFlashSize = FLASH_SIZE_16M;
//		PRINTF( "16M Bits\n");
//	} else if (ReadBuffer[2] == FLASH_SIZE_ID_32M) {
//		QspiFlashSize = FLASH_SIZE_32M;
//		PRINTF( "32M Bits\n");
//	} else if (ReadBuffer[2] == FLASH_SIZE_ID_64M) {
//		QspiFlashSize = FLASH_SIZE_64M;
//		PRINTF( "64M Bits\n");
//	} else if (ReadBuffer[3] == FLASH_SIZE_ID_128M) {
//		QspiFlashSize = FLASH_SIZE_128M;
//		PRINTF( "128M Bits\n");
//	} else if (ReadBuffer[3] == FLASH_SIZE_ID_256M) {
//		QspiFlashSize = FLASH_SIZE_256M;
//		PRINTF( "256M Bits\n");
//	} else if ((ReadBuffer[3] == FLASH_SIZE_ID_512M)
//			|| (ReadBuffer[3] == MACRONIX_FLASH_SIZE_ID_512M)) {
//		QspiFlashSize = FLASH_SIZE_512M;
//		PRINTF( "512M Bits\n");
//	} else if ((ReadBuffer[3] == FLASH_SIZE_ID_1G)
//			|| (ReadBuffer[3] == MACRONIX_FLASH_SIZE_ID_1G)) {
//		QspiFlashSize = FLASH_SIZE_1G;
//		PRINTF( "1G Bits\n");
//	}

	return XST_SUCCESS;
}


/******************************************************************************
*
* This function reads from the  serial FLASH connected to the
* QSPI interface.
*
* @param	Address contains the address to read data from in the FLASH.
* @param	ByteCount contains the number of bytes to read.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void FlashRead(uint32_t Address, uint32_t ByteCount)
{
	/*
	 * Setup the write command with the specified address and data for the
	 * FLASH
	 */
	uint32_t LqspiCrReg;
	uint8_t  ReadCommand;

	LqspiCrReg = XQspiPs_GetLqspiConfigReg(QspiInstancePtr);
	ReadCommand = (uint8_t) (LqspiCrReg & XQSPIPS_LQSPI_CR_INST_MASK);

	WriteBuffer[COMMAND_OFFSET]   = ReadCommand;
	WriteBuffer[ADDRESS_1_OFFSET] = (uint8_t)((Address & 0xFF0000) >> 16);
	WriteBuffer[ADDRESS_2_OFFSET] = (uint8_t)((Address & 0xFF00) >> 8);
	WriteBuffer[ADDRESS_3_OFFSET] = (uint8_t)(Address & 0xFF);

	ByteCount += DUMMY_SIZE;

	/*
	 * Send the read command to the FLASH to read the specified number
	 * of bytes from the FLASH, send the read command and address and
	 * receive the specified number of bytes of data in the data buffer
	 */
	XQspiPs_PolledTransfer(QspiInstancePtr, WriteBuffer, ReadBuffer,
				ByteCount + OVERHEAD_SIZE);
}

/******************************************************************************/
/**
*
* This function provides the QSPI FLASH interface for the Simplified header
* functionality.
*
* @param	SourceAddress is address in FLASH data space
* @param	DestinationAddress is address in DDR data space
* @param	LengthBytes is the length of the data in Bytes
*
* @return
*		- XST_SUCCESS if the write completes correctly
*		- XST_FAILURE if the write fails to completes correctly
*
* @note	none.
*
****************************************************************************/
static uint32_t QspiAccess( uint32_t SourceAddress, void * DestinationAddress, uint32_t LengthBytes)
{
	uint8_t	*BufferPtr;
	uint32_t Length = 0;
	uint32_t BankSel = 0;
	uint32_t Status;
	uint8_t BankSwitchFlag = 1;

	/*
	 * Linear access check
	 */
	if (LinearBootDeviceFlag == 1) {
		/*
		 * Check for non-word tail, add bytes to cover the end
		 */
//		if ((LengthBytes%4) != 0){
//			LengthBytes += (4 - (LengthBytes & 0x00000003));
//		}

		memcpy(DestinationAddress, (const void *) (SourceAddress + FlashReadBaseAddress), LengthBytes);
	}
	else
	{
		/*
		 * Non Linear access
		 */
		BufferPtr = DestinationAddress;

		/*
		 * Dual parallel connection actual flash is half
		 */
		if (XPAR_XQSPIPS_0_QSPI_MODE == DUAL_PARALLEL_CONNECTION) {
			SourceAddress = SourceAddress/2;
		}

		while (LengthBytes > 0)
		{
			/*
			 * Local of DATA_SIZE size used for read/write buffer
			 */
			if(LengthBytes > DATA_SIZE) {
				Length = DATA_SIZE;
			} else {
				Length = LengthBytes;
			}

			/*
			 * Dual stack connection
			 */
			if (XPAR_XQSPIPS_0_QSPI_MODE == DUAL_STACK_CONNECTION) {
				uint32_t LqspiCrReg;
				/*
				 * Get the current LQSPI configuration value
				 */
				LqspiCrReg = XQspiPs_GetLqspiConfigReg(QspiInstancePtr);

				/*
				 * Select lower or upper Flash based on sector address
				 */
				if (SourceAddress >= (QspiFlashSize / 2)) {
					/*
					 * Set selection to U_PAGE
					 */
//					XQspiPs_SetLqspiConfigReg(QspiInstancePtr,
//							LqspiCrReg | XQSPIPS_LQSPI_CR_U_PAGE_MASK);
					XQSPIPS->LQSPI_CR = LqspiCrReg | XQSPIPS_LQSPI_CR_U_PAGE_MASK;

					/*
					 * Subtract first flash size when accessing second flash
					 */
					SourceAddress = SourceAddress - (QspiFlashSize / 2);

					//PRINTF( "stacked - upper CS \n\r");

					/*
					 * Assert the FLASH chip select.
					 */
					XQspiPs_SetSlaveSelect(QspiInstancePtr);
				}
			}

			/*
			 * Select bank
			 */
			if ((SourceAddress >= FLASH_SIZE_16MB) && (BankSwitchFlag == 1)) {
				BankSel = SourceAddress/FLASH_SIZE_16MB;

				PRINTF( "Bank Selection %lu\n\r", BankSel);

				Status = SendBankSelect(BankSel);
				if (Status != XST_SUCCESS) {
					PRINTF( "Bank Selection Failed\n\r");
					return XST_FAILURE;
				}

				BankSwitchFlag = 0;
			}

			/*
			 * If data to be read spans beyond the current bank, then
			 * calculate length in current bank else no change in length
			 */
			if (XPAR_XQSPIPS_0_QSPI_MODE == DUAL_PARALLEL_CONNECTION) {
				/*
				 * In dual parallel mode, check should be for half
				 * the length.
				 */
				if((SourceAddress & BANKMASK) != ((SourceAddress + (Length/2)) & BANKMASK))
				{
					Length = (SourceAddress & BANKMASK) + FLASH_SIZE_16MB - SourceAddress;
					/*
					 * Above length calculated is for single flash
					 * Length should be doubled since dual parallel
					 */
					Length = Length * 2;
					BankSwitchFlag = 1;
				}
			} else {
				if((SourceAddress & BANKMASK) != ((SourceAddress + Length) & BANKMASK))
				{
					Length = (SourceAddress & BANKMASK) + FLASH_SIZE_16MB - SourceAddress;
					BankSwitchFlag = 1;
				}
			}

			/*
			 * Copying the image to local buffer
			 */
			FlashRead(SourceAddress, Length);

			/*
			 * Moving the data from local buffer to DDR destination address
			 */
			memcpy(BufferPtr, &ReadBuffer[DATA_OFFSET + DUMMY_SIZE], Length);

			/*
			 * Updated the variables
			 */
			LengthBytes -= Length;

			/*
			 * For Dual parallel connection address increment should be half
			 */
			if (XPAR_XQSPIPS_0_QSPI_MODE == DUAL_PARALLEL_CONNECTION) {
				SourceAddress += Length/2;
			} else {
				SourceAddress += Length;
			}

			BufferPtr = (uint8_t*)((uint32_t)BufferPtr + Length);
		}

		/*
		 * Reset Bank selection to zero
		 */
		Status = SendBankSelect(0);
		if (Status != XST_SUCCESS) {
			PRINTF( "Bank Selection Reset Failed\n\r");
			return XST_FAILURE;
		}

		if (XPAR_XQSPIPS_0_QSPI_MODE == DUAL_STACK_CONNECTION) {
			uint32_t LqspiCrReg;
			/*
			 * Get the current LQSPI configuration value
			 */
			LqspiCrReg = XQspiPs_GetLqspiConfigReg(QspiInstancePtr);

			/*
			 * Reset selection to L_PAGE
			 */
//			XQspiPs_SetLqspiConfigReg(QspiInstancePtr,
//					LqspiCrReg & (~XQSPIPS_LQSPI_CR_U_PAGE_MASK));
			XQSPIPS->LQSPI_CR = LqspiCrReg & (~XQSPIPS_LQSPI_CR_U_PAGE_MASK);

			//PRINTF( "stacked - lower CS \n\r");

			/*
			 * Assert the FLASH chip select.
			 */
			XQspiPs_SetSlaveSelect(QspiInstancePtr);
		}
	}

	return XST_SUCCESS;
}



/******************************************************************************
*
* This functions selects the current bank
*
* @param	BankSel is the bank to be selected in the flash device(s).
*
* @return	XST_SUCCESS if bank selected
*			XST_FAILURE if selection failed
* @note		None.
*
******************************************************************************/
static uint32_t SendBankSelect(uint8_t BankSel)
{
	uint32_t Status;

	/*
	 * bank select commands for Micron and Spansion are different
	 * Macronix bank select is same as Micron
	 */
	if (QspiFlashMake == MICRON_ID || QspiFlashMake == MACRONIX_ID)	{
		/*
		 * For micron command WREN should be sent first
		 * except for some specific feature set
		 */
		WriteBuffer[COMMAND_OFFSET] = WRITE_ENABLE_CMD;
		Status = XQspiPs_PolledTransfer(QspiInstancePtr, WriteBuffer, NULL,
				WRITE_ENABLE_CMD_SIZE);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Send the Extended address register write command
		 * written, no receive buffer required
		 */
		WriteBuffer[COMMAND_OFFSET]   = EXTADD_REG_WR;
		WriteBuffer[ADDRESS_1_OFFSET] = BankSel;
		Status = XQspiPs_PolledTransfer(QspiInstancePtr, WriteBuffer, NULL,
				BANK_SEL_SIZE);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}

	if (QspiFlashMake == SPANSION_ID) {
		WriteBuffer[COMMAND_OFFSET]   = BANK_REG_WR;
		WriteBuffer[ADDRESS_1_OFFSET] = BankSel;

		/*
		 * Send the Extended address register write command
		 * written, no receive buffer required
		 */
		Status = XQspiPs_PolledTransfer(QspiInstancePtr, WriteBuffer, NULL,
				BANK_SEL_SIZE);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}

	/*
	 * For testing - Read bank to verify
	 */
	if (QspiFlashMake == SPANSION_ID) {
		WriteBuffer[COMMAND_OFFSET]   = BANK_REG_RD;
		WriteBuffer[ADDRESS_1_OFFSET] = 0x00;

		/*
		 * Send the Extended address register write command
		 * written, no receive buffer required
		 */
		Status = XQspiPs_PolledTransfer(QspiInstancePtr, WriteBuffer, ReadBuffer,
				BANK_SEL_SIZE);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}

	if (QspiFlashMake == MICRON_ID || QspiFlashMake == MACRONIX_ID) {
		WriteBuffer[COMMAND_OFFSET]   = EXTADD_REG_RD;
		WriteBuffer[ADDRESS_1_OFFSET] = 0x00;

		/*
		 * Send the Extended address register write command
		 * written, no receive buffer required
		 */
		Status = XQspiPs_PolledTransfer(QspiInstancePtr, WriteBuffer, ReadBuffer,
				BANK_SEL_SIZE);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}

	if (ReadBuffer[1] != BankSel) {
		PRINTF( "BankSel %d != Register Read %d\n\r", BankSel,
				ReadBuffer[1]);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

static void flashPrepareLqspiCR(uint_fast8_t enableMmap)
{
	LinearBootDeviceFlag = 0;
	if (XPAR_XQSPIPS_0_QSPI_MODE == SINGLE_FLASH_CONNECTION) {

		//PRINTF("QSPI is in single flash connection\n");
		/*
		 * For Flash size <128Mbit controller configured in linear mode
		 */
		if (enableMmap && QspiFlashSize <= FLASH_SIZE_16MB) {
			//uint32_t ConfigCmd;
			LinearBootDeviceFlag = 1;

			/*
			 * Enable linear mode
			 */
			XQspiPs_SetOptions(QspiInstancePtr,  XQSPIPS_LQSPI_MODE_OPTION |
					XQSPIPS_HOLD_B_DRIVE_OPTION);

			/*
			 * Single linear read
			 */
#if WIHSPIDFHW4BIT
			//PRINTF("Linear QSPI is in 4-bit mode\n");
			//ConfigCmd = SINGLE_QSPI_CONFIG_FAST_QUAD_READ;
			//XQspiPs_SetLqspiConfigReg(QspiInstancePtr, ConfigCmd);
			XQSPIPS->LQSPI_CR = SINGLE_QSPI_CONFIG_FAST_QUAD_READ;
#elif WIHSPIDFHW2BIT
			//PRINTF("Linear QSPI is in 2-bit mode\n");
			//ConfigCmd = SINGLE_QSPI_CONFIG_FAST_DUAL_READ;
			//XQspiPs_SetLqspiConfigReg(QspiInstancePtr, ConfigCmd);
			XQSPIPS->LQSPI_CR = SINGLE_QSPI_CONFIG_FAST_DUAL_READ;
#else
			//PRINTF("Linear QSPI is in 1-bit mode\n");
			//ConfigCmd = SINGLE_QSPI_CONFIG_FAST_READ;
			//XQspiPs_SetLqspiConfigReg(QspiInstancePtr, ConfigCmd);
			XQSPIPS->LQSPI_CR = SINGLE_QSPI_CONFIG_FAST_READ;
#endif



			/*
			 * Enable the controller
			 */
			//XQspiPs_Enable(QspiInstancePtr);
			XQSPIPS->ER = 0x00000001;
		} else {
			// No MMAP
			/*
			 * Single flash IO read
			 */
			//uint32_t ConfigCmd;

#if WIHSPIDFHW4BIT
			//PRINTF("QSPI is in 4-bit mode\n");
			//ConfigCmd = SINGLE_QSPI_IO_CONFIG_FAST_QUAD_READ;
//			XQspiPs_SetLqspiConfigReg(QspiInstancePtr, ConfigCmd);
			XQSPIPS->LQSPI_CR = SINGLE_QSPI_IO_CONFIG_FAST_QUAD_READ;
#elif WIHSPIDFHW2BIT
			//PRINTF("QSPI is in 2-bit mode\n");
			//ConfigCmd = SINGLE_QSPI_IO_CONFIG_FAST_DUAL_READ;
//			XQspiPs_SetLqspiConfigReg(QspiInstancePtr, ConfigCmd);
			XQSPIPS->LQSPI_CR = SINGLE_QSPI_IO_CONFIG_FAST_DUAL_READ;
#else
			//PRINTF("QSPI is in 1-bit mode\n");
			//ConfigCmd = SINGLE_QSPI_IO_CONFIG_FAST_READ;
	//			XQspiPs_SetLqspiConfigReg(QspiInstancePtr, ConfigCmd);
			XQSPIPS->LQSPI_CR = SINGLE_QSPI_IO_CONFIG_FAST_READ;
#endif

			/*
			 * Enable the controller
			 */
			//XQspiPs_Enable(QspiInstancePtr);
			XQSPIPS->ER = 0x00000001;
		}
	} else if (XPAR_XQSPIPS_0_QSPI_MODE == DUAL_PARALLEL_CONNECTION) {
		//uint32_t ConfigCmd;

		//PRINTF("QSPI is in Dual Parallel connection\n");
		/*
		 * For Single Flash size <128Mbit controller configured in linear mode
		 */
		if (enableMmap && QspiFlashSize <= FLASH_SIZE_16MB) {
			/*
			 * Setting linear access flag
			 */
			LinearBootDeviceFlag = 1;

			/*
			 * Enable linear mode
			 */
			XQspiPs_SetOptions(QspiInstancePtr,  XQSPIPS_LQSPI_MODE_OPTION |
					XQSPIPS_HOLD_B_DRIVE_OPTION);

			/*
			 * Dual linear read
			 */
//			XQspiPs_SetLqspiConfigReg(QspiInstancePtr, DUAL_QSPI_CONFIG_FAST_QUAD_READ);
			XQSPIPS->LQSPI_CR = DUAL_QSPI_CONFIG_FAST_QUAD_READ;

			/*
			 * Enable the controller
			 */
			//XQspiPs_Enable(QspiInstancePtr);
			XQSPIPS->ER = 0x00000001;
		} else {
			/*
			 * Dual flash IO read
			 */
//			XQspiPs_SetLqspiConfigReg(QspiInstancePtr, DUAL_QSPI_IO_CONFIG_FAST_QUAD_READ);
			XQSPIPS->LQSPI_CR = DUAL_QSPI_IO_CONFIG_FAST_QUAD_READ;

			/*
			 * Enable the controller
			 */
			//XQspiPs_Enable(QspiInstancePtr);
			XQSPIPS->ER = 0x00000001;

		}

		/*
		 * Total flash size is two time of single flash size
		 */
		QspiFlashSize = 2 * QspiFlashSize;

	} else 	if (XPAR_XQSPIPS_0_QSPI_MODE == DUAL_STACK_CONNECTION) {
		/*
		 * It is expected to same flash size for both chip selection
		 */
		//uint32_t ConfigCmd;

		//PRINTF("QSPI is in Dual Stack connection\n");

		QspiFlashSize = 2 * QspiFlashSize;

		/*
		 * Enable two flash memories on separate buses
		 */
#if WIHSPIDFHW4BIT
		//PRINTF("QSPI is in 4-bit mode\n");
		//ConfigCmd =  DUAL_STACK_CONFIG_FAST_QUAD_READ;
		//XQspiPs_SetLqspiConfigReg(QspiInstancePtr, ConfigCmd);
		XQSPIPS->LQSPI_CR = DUAL_STACK_CONFIG_FAST_QUAD_READ;
#elif WIHSPIDFHW2BIT
		//PRINTF("QSPI is in 2-bit mode\n");
		//ConfigCmd =  DUAL_STACK_CONFIG_FAST_DUAL_READ;
		//XQspiPs_SetLqspiConfigReg(QspiInstancePtr, ConfigCmd);
		XQSPIPS->LQSPI_CR = DUAL_STACK_CONFIG_FAST_DUAL_READ;
#else
		//PRINTF("QSPI is in 1-bit mode\n");
		//ConfigCmd =  DUAL_STACK_CONFIG_FAST_READ;
		//XQspiPs_SetLqspiConfigReg(QspiInstancePtr, ConfigCmd);
		XQSPIPS->LQSPI_CR = DUAL_STACK_CONFIG_FAST_READ;
#endif
	}
}

/******************************************************************************/
/**
*
* This function initializes the controller for the QSPI interface.
*
* @param	None
*
* @return	None
*
* @note		None
*
****************************************************************************/
static uint32_t InitQspi(void)
{
	XQspiPs_Config *QspiConfig;
	int Status;


	/*
	 * Set up the base address for access
	 */
	FlashReadBaseAddress = XPS_QSPI_LINEAR_BASEADDR;

	/*
	 * Initialize the QSPI driver so that it's ready to use
	 */
	QspiConfig = XQspiPs_LookupConfig(QSPI_DEVICE_ID);
	if (NULL == QspiConfig) {
		return XST_FAILURE;
	}

	Status = XQspiPs_CfgInitialize(QspiInstancePtr, QspiConfig,
					QspiConfig->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Set Manual Chip select options and drive HOLD_B pin high.
	 */
	XQspiPs_SetOptions(QspiInstancePtr, XQSPIPS_FORCE_SSELECT_OPTION |
			XQSPIPS_HOLD_B_DRIVE_OPTION);

	/*
	 * Set the prescaler for QSPI clock
	 */
	XQspiPs_SetClkPrescaler(QspiInstancePtr, XQSPIPS_CLK_PRESCALE_8);

#if 0
	/*
	 * Assert the FLASH chip select.
	 */
	XQspiPs_SetSlaveSelect(QspiInstancePtr);

	/*
	 * Read Flash ID and extract Manufacture and Size information
	 */
	Status = FlashReadID();
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
#endif



	return XST_SUCCESS;
}

#endif /* CPUSTYLE_XC7Z */

