/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "spifuncs.h"

#include <stdlib.h>
//#include <assert.h>

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

	SPI_ALLCS_INITIALIZE();		/* инициализация лиий выбора периферийных микросхем */
	SPI_ADDRESS_AEN_INITIALIZE();	/* инициализация сигналов управлдения дешифратором CS */
	SPI_ADDRESS_NAEN_INITIALIZE();	/* инициализация сигналов управлдения дешифратором CS */

	SPI_IOUPDATE_INITIALIZE();
	SPI_IORESET_INITIALIZE();

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
	hardware_spi_master_setfreq(SPIC_SPEED10M, 10000000uL);	/* 10 MHz для ILI9341 */
	hardware_spi_master_setfreq(SPIC_SPEED25M, 25000000uL);	/* 25 MHz  */
#endif /* (SPISPEED400k) || defined (SPISPEED100k) */

}

#endif /* WITHSPIHW || WITHSPISW */
