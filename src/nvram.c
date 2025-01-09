/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"
#include "formats.h"	// for debug

#include <stdlib.h>
#include <string.h>		// for memcpy


#if defined (NVRAM_TYPE) && (NVRAM_TYPE != NVRAM_TYPE_NOTHING)

#include "spi.h"

#define NVRAM_SPIMODE SPIC_MODE3		// mode 3 or mode 0 suppeoted by FRAM chip.
#define NVRAM_SPISPEED SPIC_SPEEDFAST

#define WREN	0x06
#define WRITE	0x02
#define READ	0x03
#define RDSR	0x05
#define WRSR	0x01
#define WRDI	0x04

#if ! defined (NVRAM_TYPE) || ! defined (NVRAM_END)
	#error NVRAM_TYPE or NVRAM_END not defined here
#endif

// Использование проверки перед работой с FRAM не требуется,
// кроме того при отсутствующей микросхеме BUSY всегда будет в "1" - программа будет висеть без диагностики.

#if \
	NVRAM_TYPE == NVRAM_TYPE_FM25L04 ||		/* SERIAL FRAM 4KBit	*/ \
	0
	static const uint_fast8_t nvram_abytes = 0;		/* 0 - 1 байт адреса, 1 - два байта адреса, 2 - три байта адреса */
	static const uint_fast8_t nvram_chstatus = 0;	/* 0 - fram - нет нужды проверять статус и разбивать запись на страницы */
#elif \
	NVRAM_TYPE == NVRAM_TYPE_FM25L16 ||		/* SERIAL FRAM 16Kbit	*/ \
	NVRAM_TYPE == NVRAM_TYPE_FM25L64 ||		/* SERIAL FRAM 64Kbit	*/ \
	NVRAM_TYPE == NVRAM_TYPE_FM25L256 ||	/* SERIAL FRAM 256KBit	*/ \
	0
	static const uint_fast8_t nvram_abytes = 1;		/* 0 - 1 байт адреса, 1 - два байта адреса, 2 - три байта адреса */
	static const uint_fast8_t nvram_chstatus = 0;	/* 0 - fram - нет нужды проверять статус и разбивать запись на страницы */
#elif \
	NVRAM_TYPE == NVRAM_TYPE_AT25040A ||	/* SERIAL EEPROM 4KBit	*/ \
	0
	static const uint_fast8_t nvram_abytes = 0;		/* 0 - 1 байт адреса, 1 - два байта адреса, 2 - три байта адреса */
	static const uint_fast8_t nvram_chstatus = 1;	/* 0 - fram - нет нужды проверять статус и разбивать запись на страницы */
#elif \
	NVRAM_TYPE == NVRAM_TYPE_AT25L16 ||		/* SERIAL EEPROM 16Kbit	*/ \
	NVRAM_TYPE == NVRAM_TYPE_AT25256A ||	/* SERIAL EEPROM 256KBit */ \
	0
	static const uint_fast8_t nvram_abytes = 1;		/* 0 - 1 байт адреса, 1 - два байта адреса, 2 - три байта адреса */
	static const uint_fast8_t nvram_chstatus = 1;	/* 0 - fram - нет нужды проверять статус */
#elif NVRAM_TYPE == NVRAM_TYPE_FM25XXXX 	/* SERIAL FRAM autodetect	*/
	static uint_fast8_t nvram_abytes = 0;		/* 0 - 1 байт адреса, 1 - два байта адреса, 2 - три байта адреса */
	static const uint_fast8_t nvram_chstatus = 0;	/* 1 - eeprom - надо проверять статус и разбивать запись на страницы */
	void
	nvram_set_abytes(uint_fast8_t v)
	{
		 nvram_abytes = v;
	}
#endif

#if NVRAM_TYPE != NVRAM_TYPE_CPUEEPROM && NVRAM_TYPE != NVRAM_TYPE_BKPSRAM

#define NVRAMBUSY 0x01
/*
	Up to 16 bytes (25XX160A) or 32
	bytes (25XX160B) of data can be sent to the device
	before a write cycle is necessary. The only restriction is
	that all of the bytes must reside in the same page.
*/
#define EEPROMPAGEMASK	0x0f	// Page size = 16

static uint_fast8_t 
//NOINLINEAT
eeprom_read_status(
	spitarget_t target	/* addressing to chip */
	)
{
	static const uint8_t cmd_rdsr [] = { RDSR }; /* read status register */
	uint8_t v;
	prog_spi_io(target, NVRAM_SPISPEED, NVRAM_SPIMODE, cmd_rdsr, ARRAY_SIZE(cmd_rdsr), NULL, 0, & v, 1);
	return v;
}

static void 
//NOINLINEAT
eeprom_writeenable(
	spitarget_t target	/* addressing to chip */
	)
{
	// +++ РАЗРЕШЕНИЕ ЗАПИСИ
	static const uint8_t cmd_wren [] = { WREN }; /* set write-enable latch */

	prog_spi_io(target, NVRAM_SPISPEED, NVRAM_SPIMODE, cmd_wren, ARRAY_SIZE(cmd_wren), NULL, 0, NULL, 0);
	// --- РАЗРЕШЕНИЕ ЗАПИСИ
}


/* one byte + one bit address 512-byte chips */
static void 
//NOINLINEAT
eeprom_a1_write(
	spitarget_t target,	/* addressing to chip */
	uint_fast16_t addr, 	/* начальное смещение памяти для обмена */
	const uint8_t * data,
	uint_fast8_t len)	/* количество байт */
{
	uint_fast8_t i;


	eeprom_writeenable(target);

	// +++ Запись данных
	const uint8_t cmd [] =
	{
		(addr > 0xff) * 0x08 | WRITE,	/* write, a8=0 or a8=1 */
		(addr & 0xFF),
	};
	prog_spi_io(target, NVRAM_SPISPEED, NVRAM_SPIMODE, cmd, ARRAY_SIZE(cmd), data, len, NULL, 0);
	// --- Запись данных
}

/* one byte + one bit address 512-byte chips */
static void 
//NOINLINEAT
eeprom_a1_read(
	spitarget_t target,	/* addressing to chip */
	uint_fast16_t addr, 
	uint8_t * data,
	unsigned len)
{
	const uint8_t cmd [] =
	{
		(addr > 0xff) * 0x08 | READ,	/* read, a8=0 or a8=1 */
		(addr & 0xFF),
	};
	prog_spi_io(target, NVRAM_SPISPEED, NVRAM_SPIMODE, cmd, ARRAY_SIZE(cmd), NULL, 0, data, len);
}

/* two bytes address 2K-byte chips */
static void 
eeprom_a2_write(
	spitarget_t target,	/* addressing to chip */
	uint_fast16_t addr, 
	const uint8_t * data,
	uint_fast8_t len)
{

	eeprom_writeenable(target);

	const uint8_t cmd [] =
	{
		WRITE,
		(uint_fast8_t) (addr >> 8),
		(uint_fast8_t) (addr >> 0),
	};
	prog_spi_io(target, NVRAM_SPISPEED, NVRAM_SPIMODE, cmd, ARRAY_SIZE(cmd), data, len, NULL, 0);
}

/* two bytes address 2K-byte chips */
static void 
//NOINLINEAT
eeprom_a2_read(
	spitarget_t target,	/* addressing to chip */
	uint_fast16_t addr, 
	uint8_t * data,
	unsigned len)
{
	const uint8_t cmd [] =
	{
		READ,
		(uint_fast8_t) (addr >> 8),
		(uint_fast8_t) (addr >> 0),
	};
	prog_spi_io(target, NVRAM_SPISPEED, NVRAM_SPIMODE, cmd, ARRAY_SIZE(cmd), NULL, 0, data, len);
}

#if 0
/* three bytes address 512K-byte chips */
static void 
eeprom_a3_write(
	spitarget_t target,	/* addressing to chip */
	uint_fast32_t addr, 
	const uint8_t * data,
	uint_fast8_t len)
{

	eeprom_writeenable(target);

	const uint8_t cmd [] =
	{
		WRITE,
		(uint_fast8_t) (addr >> 16),
		(uint_fast8_t) (addr >> 8),
		(uint_fast8_t) (addr >> 0),
	};

	prog_spi_io(target, NVRAM_SPISPEED, NVRAM_SPIMODE, cmd, ARRAY_SIZE(cmd), data, len, NULL, 0);
}

/* three bytes address 512K-byte chips */
static void 
eeprom_a3_read(
	spitarget_t target,	/* addressing to chip */
	uint_fast32_t addr, 
	uint8_t * data,
	uint_fast8_t len)
{
	const uint8_t cmd [] =
	{
		READ,
		(uint_fast8_t) (addr >> 16),
		(uint_fast8_t) (addr >> 8),
		(uint_fast8_t) (addr >> 0),
	};

	prog_spi_io(target, NVRAM_SPISPEED, NVRAM_SPIMODE, cmd, ARRAY_SIZE(cmd), NULL, 0, data, len);
}

#endif

static void
eeprom_wait_until_ready(
	spitarget_t target	/* addressing to chip */
	)
{
	uint_fast16_t w = 5000;
	while (nvram_chstatus != 0 && w -- != 0 && (eeprom_read_status(targetnvram) & NVRAMBUSY) != 0)
		continue;
}

static void 
//NOINLINEAT
eeprom_initialize(
	spitarget_t target	/* addressing to chip */
	)
{
	// VDD(min) to First Access Start - 10 mS
	local_delay_ms(20);

	/* Ожидание бита ~RDY в слове состояния. Для FRAM не имеет смысла.
	Вставлено для возможности использования EEPROM */
	eeprom_wait_until_ready(target);

	// принудительное "передёргивание" сигнала чипселект - решение проблемы с запуском.
	//prog_select(targetnvram);	/* done sending data to target chip */
	//spi_unselect(targetnvram);	/* done sending data to target chip */


	// +++ РАЗРЕШЕНИЕ ЗАПИСИ
	static const uint8_t cmd_wren [] = { WREN }; /* set write-enable latch */
	prog_spi_io(target, NVRAM_SPISPEED, NVRAM_SPIMODE, cmd_wren, ARRAY_SIZE(cmd_wren), NULL, 0, NULL, 0);
	// --- РАЗРЕШЕНИЕ ЗАПИСИ

	// +++ WSR 0
	static const uint8_t wrsr_0 [] = { WRSR, 0x00 }; /* set status register data */
	prog_spi_io(target, NVRAM_SPISPEED, NVRAM_SPIMODE, wrsr_0, ARRAY_SIZE(wrsr_0), NULL, 0, NULL, 0);
	// --- WSR 0
}


static void  
//NOINLINEAT
nvram_write_withinpage(uint_least16_t addr, const uint8_t * data, unsigned len)
{
	/* Ожидание бита ~RDY в слове состояния. Для FRAM не имеет смысла.
	Вставлено для возможности использования EEPROM */
	eeprom_wait_until_ready(targetnvram);

	switch (nvram_abytes)
	{
	default:
	case 0:	/* for sizes up to 512 bytes */
		eeprom_a1_write(targetnvram,	addr, data, len);
		break;
	case 1:
		eeprom_a2_write(targetnvram,	addr, data, len);
		break;
	//case 2:
	//	eeprom_a3_write(targetnvram,	addr, data, len);
	//	break;
	}
}

static uint_least16_t 
ui16min(uint_least16_t a, uint_least16_t b)
{
	return a < b ? a : b;
}

#endif /* NVRAM_TYPE != NVRAM_TYPE_CPUEEPROM && NVRAM_TYPE != NVRAM_TYPE_BKPSRAM */

/* интерфейсные функции NVRAM */

#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32MP1
#if (NVRAM_TYPE == NVRAM_TYPE_BKPSRAM)
	// Разрешить запись в Backup domain
	static void stm32f4xx_bdenable(void)
	{
	#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX

		PWR->CR |= PWR_CR_DBP;  
		(void) PWR->CR;
		while ((PWR->CR & PWR_CR_DBP) == 0)
			;

	#else /* CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX */

		PWR->CR1 |= PWR_CR1_DBP;  // 1: Write access to RTC and backup domain registers enabled.
		(void) PWR->CR1;
		while ((PWR->CR1 & PWR_CR1_DBP) == 0)
			;

	#endif /* CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX */
	}

	// Запретить запись в Backup domain
	static void stm32f4xx_bddisable(void)
	{
	#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX

		PWR->CR &= ~ PWR_CR_DBP;	
		(void) PWR->CR;
		while ((PWR->CR & PWR_CR_DBP) != 0)
			;

	#else /* CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX */

		PWR->CR1 &= ~ PWR_CR1_DBP;	// Write access to RTC and backup domain registers disabled.
		(void) PWR->CR1;
		while ((PWR->CR1 & PWR_CR1_DBP) != 0)
			;

	#endif /* CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX */
	}
#endif /* (NVRAM_TYPE != NVRAM_TYPE_CPUEEPROM) */
#endif /* CPUSTYLE_STM32F */

/* вызывается при разрешённых прерываниях. */
void nvram_initialize(void)
{
	//PRINTF(PSTR("nvram_initialize\n"));
#if (NVRAM_TYPE == NVRAM_TYPE_BKPSRAM)

	#if CPUSTYLE_STM32MP1

		RCC->MP_AHB5ENSETR = RCC_MP_AHB5ENSETR_BKPSRAMEN;
		(void) RCC->MP_AHB5ENSETR;
		RCC->MP_AHB5LPENSETR = RCC_MP_AHB5LPENSETR_BKPSRAMLPEN;
		(void) RCC->MP_AHB5LPENSETR;

		stm32f4xx_bdenable();

		//		PWR->CR2 |= PWR_CR2_RREN;	// Retention regulator enable.
//		(void) PWR->CR2;
//		while ((PWR->CR2 & PWR_CR2_RRRDY) == 0)
			;

		PWR->CR2 |= PWR_CR2_BREN;	// Backup regulator enable.
		(void) PWR->CR2;
		while ((PWR->CR2 & PWR_CR2_BRRDY) == 0)
			;

		stm32f4xx_bddisable();

	#elif CPUSTYLE_STM32F
		// RCC_APB1ENR_RTCAPBEN ???
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;	// включить тактирование power management
		__DSB();

		stm32f4xx_bdenable();
		RCC->AHB1ENR |= RCC_AHB1ENR_BKPSRAMEN;	// включил BKPSRAM
		__DSB();

		#if defined (PWR_CSR1_BRE)

			PWR->CSR1 |= PWR_CSR1_BRE;			// Разрешить питание BACKUP SRAM от батареи при выключенном питании процессора
			while ((PWR->CSR1 & PWR_CSR1_BRR) == 0)	// дождаться готовности
				;

		#elif defined (PWR_CSR_BRE)

			PWR->CSR |= PWR_CSR_BRE;			// Разрешить питание BACKUP SRAM от батареи при выключенном питании процессора
			while ((PWR->CSR & PWR_CSR_BRR) == 0)	// дождаться готовности
				;

		#endif /* defined (PWR_CSR1_BRE) */

		stm32f4xx_bddisable();

	#endif /* CPUSTYLE_STM32F */
#elif (NVRAM_TYPE != NVRAM_TYPE_CPUEEPROM)

	eeprom_initialize(targetnvram);

#endif /* NVRAM_TYPE != NVRAM_TYPE_CPUEEPROM */
	//PRINTF(PSTR("nvram_initialize: done\n"));
}

//static uint8_t simnvram [NVRAM_END + 1];

void
nvram_write(nvramaddress_t addr, const uint8_t * data, unsigned len)
{
	ASSERT((addr + len - 1) <= NVRAM_END);
#if (NVRAM_TYPE == NVRAM_TYPE_BKPSRAM)

	volatile uint8_t * const p = (uint8_t *) BKPSRAM_BASE + addr;
	//uint8_t * const p = simnvram + addr;

	stm32f4xx_bdenable();
	if (len == sizeof (uint16_t))
	{
		p [0] = ((const uint8_t *) data) [0];
		p [1] = ((const uint8_t *) data) [1];
	}
	else if (len == sizeof (uint32_t))
	{
		p [0] = ((const uint8_t *) data) [0];
		p [1] = ((const uint8_t *) data) [1];
		p [2] = ((const uint8_t *) data) [2];
		p [3] = ((const uint8_t *) data) [3];
	}
	else if (len == sizeof (uint8_t))
	{
		p [0] = ((const uint8_t *) data) [0];
	}
	else
	{
		memcpy(p, data, len);
	}
	//dcache_clean((uintptr_t) BKPSRAM_BASE, 4096);
	stm32f4xx_bddisable();

#elif (NVRAM_TYPE == NVRAM_TYPE_CPUEEPROM)

	eeprom_busy_wait();
	eeprom_update_block(data, (void *) addr, len);	// was: eeprom_write_block

#else
	if (nvram_chstatus == 0 || len == 1)
	{
		// разбивки записи на страницы не требуется - FRAM или запись единственного байта в EEPROM
		nvram_write_withinpage(addr, data, len);
	}
	else
	{
		// Для EEPROM требуется соблюдать непереход одной операции записи через границы страниц
		const uint_least16_t EEPROMPAGELENGTH = (EEPROMPAGEMASK + 1);
		const uint8_t * const bp = data;
		uint_least16_t score = 0;
		if ((addr & EEPROMPAGEMASK) != 0)
		{
			/* запись первой страницы, если начало не выровнено */
			score = ui16min(EEPROMPAGELENGTH - (addr & EEPROMPAGEMASK), len);
			nvram_write_withinpage(addr, bp, score);
		}
		while ((len - score) >= EEPROMPAGELENGTH)
		{
			/* запись полследующих - выровненных страниц */
			nvram_write_withinpage(addr + score, bp + score, EEPROMPAGELENGTH);
			score += EEPROMPAGELENGTH;
		}
		if ((len - score) != 0)
		{
			/* запись последней - выровненной но не полной страницы */
			nvram_write_withinpage(addr + score, bp + score, len - score);
		}
	}

#endif
}

void
nvram_read(nvramaddress_t addr, uint8_t * data, unsigned len)
{
	ASSERT((addr + len - 1) <= NVRAM_END);
#if (NVRAM_TYPE == NVRAM_TYPE_BKPSRAM)

	const volatile uint8_t * const p = (const uint8_t *) BKPSRAM_BASE + addr;
	//const uint8_t * const p = simnvram + addr;

	if (len == sizeof (uint16_t))
	{
		((uint8_t *) data) [0] = p [0];
		((uint8_t *) data) [1] = p [1];
	}
	else if (len == sizeof (uint32_t))
	{
		((uint8_t *) data) [0] = p [0];
		((uint8_t *) data) [1] = p [1];
		((uint8_t *) data) [2] = p [2];
		((uint8_t *) data) [3] = p [3];
	}
	else if (len == sizeof (uint8_t))
	{
		((uint8_t *) data) [0] = p [0];
	}
	else
	{
		memcpy(data, b, len);
	}

#elif (NVRAM_TYPE == NVRAM_TYPE_CPUEEPROM)

		eeprom_busy_wait();
		eeprom_read_block(data, (void *) addr, len);
#else
	/* Ожидание бита ~RDY в слове состояния. Для FRAM не имеет смысла.
	Вставлено для возможности использования EEPROM */
	eeprom_wait_until_ready(targetnvram);

	switch (nvram_abytes)
	{
	default:
	case 0:	/* for sizes up to 512 bytes */
		eeprom_a1_read(targetnvram,	addr, data, len);
		break;
	case 1:
		eeprom_a2_read(targetnvram,	addr, data, len);
		break;
	//case 2:
	//	eeprom_a3_read(targetnvram,	addr, data, len);
	//	break;
	}

#endif
}




/* выборка по указанному индексу из FRAM одного байта */
uint_fast8_t 
//NOINLINEAT
restore_i8(nvramaddress_t addr)
{
	uint8_t vb [1];
	nvram_read(addr, & vb [0], sizeof vb);
	return vb [0];
}

/* сохранение по указанному индексу в FRAM одного байта */
void 
//NOINLINEAT
save_i8(nvramaddress_t addr, uint_fast8_t v)
{
	uint8_t vb [1];
	vb [0] = v;
	nvram_write(addr, & vb [0], sizeof vb);
}

/* выборка по указанному индексу из FRAM одного 16-битного слова */
uint_fast16_t 
//NOINLINEAT
restore_i16(nvramaddress_t addr)
{
	uint8_t vb [2];

	nvram_read(addr, & vb [0], sizeof vb);
	return USBD_peek_u16(vb);
}

/* сохранение по указанному индексу в FRAM одного 16-битного слова */
void 
//NOINLINEAT
save_i16(nvramaddress_t addr, uint_fast16_t v)
{
	uint8_t vb [2];

	USBD_poke_u16(vb, v);
	nvram_write(addr, & vb [0], sizeof vb);
}

/* выборка по указанному индексу из FRAM одного 24-битного слова */
uint_fast32_t
//NOINLINEAT
restore_i24(nvramaddress_t addr)
{
	uint8_t vb [3];

	nvram_read(addr, vb, sizeof vb);
	return USBD_peek_u24(vb);
}

/* сохранение по указанному индексу в FRAM одного 24-битного слова */
void
//NOINLINEAT
save_i24(nvramaddress_t addr, uint_fast32_t v)
{
	uint8_t vb [3];

	USBD_poke_u24(vb, v);
	nvram_write(addr, & vb [0], sizeof vb);
}

/* выборка по указанному индексу из FRAM одного 32-битного слова */
uint_fast32_t 
//NOINLINEAT
restore_i32(nvramaddress_t addr)
{
	uint8_t vb [4];

	nvram_read(addr, vb, sizeof vb);
	return USBD_peek_u32(vb);
}

/* сохранение по указанному индексу в FRAM одного 32-битного слова */
void 
//NOINLINEAT
save_i32(nvramaddress_t addr, uint_fast32_t v)
{
	uint8_t vb [4];

	USBD_poke_u32(vb, v);
	nvram_write(addr, & vb [0], sizeof vb);
}

#else /* defined (NVRAM_TYPE) && (NVRAM_TYPE != NVRAM_TYPE_NOTHING) */

#if 1
/* выборка по указанному индексу из FRAM одного байта */
uint_fast8_t 
//NOINLINEAT
restore_i8(nvramaddress_t addr)
{
	return 0xFF;
}

/* сохранение по указанному индексу в FRAM одного байта */
void 
//NOINLINEAT
save_i8(nvramaddress_t addr, uint_fast8_t v)
{
}

/* выборка по указанному индексу из FRAM одного 16-битного слова */
uint_fast16_t 
//NOINLINEAT
restore_i16(nvramaddress_t addr)
{
	return 0xFFFF;
}

/* сохранение по указанному индексу в FRAM одного 16-битного слова */
void 
//NOINLINEAT
save_i16(nvramaddress_t addr, uint_fast16_t v)
{
}

/* выборка по указанному индексу из FRAM одного 24-битного слова */
uint_fast32_t
//NOINLINEAT
restore_i24(nvramaddress_t addr)
{
	return 0x00FFFFFF;
}

/* сохранение по указанному индексу в FRAM одного 32-битного слова */
void
//NOINLINEAT
save_i24(nvramaddress_t addr, uint_fast32_t v)
{
}

/* выборка по указанному индексу из FRAM одного 32-битного слова */
uint_fast32_t
//NOINLINEAT
restore_i32(nvramaddress_t addr)
{
	return 0xFFFFFFFF;
}

/* сохранение по указанному индексу в FRAM одного 32-битного слова */
void
//NOINLINEAT
save_i32(nvramaddress_t addr, uint_fast32_t v)
{
}

void nvram_write(nvramaddress_t addr, const uint8_t * data, unsigned len)
{

}

void nvram_read(nvramaddress_t addr, uint8_t * data, unsigned len)
{
	memset(data, 0xFF, len);
}

#endif

#endif /* defined (NVRAM_TYPE) && (NVRAM_TYPE != NVRAM_TYPE_NOTHING) */
