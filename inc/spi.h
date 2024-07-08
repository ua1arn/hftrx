/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#ifndef SPIFUNCS_H_INCLUDED
#define SPIFUNCS_H_INCLUDED

#include "hardware.h"

#include <stdint.h>

#include "gpio.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void spi_initialize(void);	// отдельно инициализация SPI


/* Управление SPI. Так как некоторые периферийные устройства не могут работать с 8-битовыми блоками
   на шине, в таких случаях формирование делается программно - аппаратный SPI при этом отключается.

   Так как переключение в нужный режим SPI производится после активизации CS, для такого применения не годятся
   режимы SPI с "0" уровнем SCLK в неактивном состоянии.
   */

typedef enum
{
	SPIC_MODE0,
	SPIC_MODE1,
	SPIC_MODE2,
	SPIC_MODE3,
	//
	SPIC_MODES_COUNT
} spi_modes_t;

#if WITHSPISW
	#if CPUSTYLE_XC7Z || CPUSTYLE_XCZU
		#define SCLK_NPULSE() do { 							\
			SPI_SCLK_C(); hardware_spi_io_delay(); 			\
			SPI_SCLK_S(); hardware_spi_io_delay(); 			\
		} while (0)

	#define SDO_SET(val) do { 								\
			if ((val) != 0) 								\
				{ SPI_MOSI_S(); hardware_spi_io_delay();  } \
			else 											\
				{ SPI_MOSI_C(); hardware_spi_io_delay();  } \
		} while (0)


	#elif CPUSTYLE_ARM || CPUSTYLE_ATXMEGA
		// при программной реализации SPI
		// поддерживается режим SPI MODE 3
		#define SCLK_SET() do { \
				{ SPI_TARGET_SCLK_PORT_S(SPI_SCLK_BIT); hardware_spi_io_delay(); } \
			} while (0)

		#define SCLK_CLR() do { \
				{ SPI_TARGET_SCLK_PORT_C(SPI_SCLK_BIT); hardware_spi_io_delay();  } \
			} while (0)

		#define SCLK_NPULSE() do { SCLK_CLR(); SCLK_SET(); } while (0)
			
		#define SDO_SET(val) do { \
				if ((val) != 0) \
					{ SPI_TARGET_MOSI_PORT_S(SPI_MOSI_BIT); hardware_spi_io_delay();  } \
				else \
					{ SPI_TARGET_MOSI_PORT_C(SPI_MOSI_BIT); hardware_spi_io_delay();  } \
			} while (0)

	#elif CPUSTYLE_ATMEGA

		#if WITHSPISPLIT	/* для двух разных потребителей формируются отдельные сигналы MOSI, SCK, CS */
			// SPI0
			// при программной реализации SPI
			// поддерживается режим SPI MODE 3
			// Формирование строба SPI0
			#define SCLK0_SET() do { SPI0_TARGET_SCLK_PORT |= SPI0_SCLK_BIT; } while (0)
			#define SCLK0_CLR() do { SPI0_TARGET_SCLK_PORT &= ~SPI0_SCLK_BIT; } while (0)

			#if CPUSTYLE_ATMEGA_XXX4
				// поддержка быстрого формирования строба
				#define SCLK0_TGL() do { SPI0_TARGET_SCLK_PIN = SPI0_SCLK_BIT; } while (0)

				#define SCLK0_NPULSE() do { SCLK0_TGL(); SCLK0_TGL(); } while (0)
			#else
				#define SCLK0_NPULSE() do { SCLK0_CLR(); SCLK0_SET(); } while (0)
			#endif
			// выдача данных на MOSI0
			#define SDO0_SET(val) do { \
				if ((val) != 0)  \
				  SPI0_TARGET_MOSI_PORT |= SPI0_MOSI_BIT; \
				else \
				  SPI0_TARGET_MOSI_PORT &= ~SPI0_MOSI_BIT; \
				} while (0)
			// SPI1
			// при программной реализации SPI
			// поддерживается режим SPI MODE 3
			// Формирование строба SPI1
			#define SCLK1_SET() do { SPI1_TARGET_SCLK_PORT |= SPI1_SCLK_BIT; } while (0)
			#define SCLK1_CLR() do { SPI1_TARGET_SCLK_PORT &= ~SPI1_SCLK_BIT; } while (0)

			#if CPUSTYLE_ATMEGA_XXX4
				// поддержка быстрого формирования строба
				#define SCLK1_TGL() do { SPI1_TARGET_SCLK_PIN = SPI1_SCLK_BIT; } while (0)

				#define SCLK1_NPULSE() do { SCLK1_TGL(); SCLK1_TGL(); } while (0)
			#else
				#define SCLK1_NPULSE() do { SCLK1_CLR(); SCLK1_SET(); } while (0)
			#endif
			// выдача данных на MOSI1
			#define SDO1_SET(val) do { \
				if ((val) != 0)  \
				  SPI1_TARGET_MOSI_PORT |= SPI1_MOSI_BIT; \
				else \
				  SPI1_TARGET_MOSI_PORT &= ~SPI1_MOSI_BIT; \
				} while (0)
			// SPI2
			// при программной реализации SPI
			// поддерживается режим SPI MODE 3
			// Формирование строба SPI1
			#define SCLK2_SET() do { SPI2_TARGET_SCLK_PORT |= SPI2_SCLK_BIT; } while (0)
			#define SCLK2_CLR() do { SPI2_TARGET_SCLK_PORT &= ~SPI2_SCLK_BIT; } while (0)

			#if CPUSTYLE_ATMEGA_XXX4
				// поддержка быстрого формирования строба
				#define SCLK2_TGL() do { SPI2_TARGET_SCLK_PIN = SPI2_SCLK_BIT; } while (0)

				#define SCLK2_NPULSE() do { SCLK2_TGL(); SCLK2_TGL(); } while (0)
			#else
				#define SCLK2_NPULSE() do { SCLK2_CLR(); SCLK2_SET(); } while (0)
			#endif
			// выдача данных на MOSI1
			#define SDO2_SET(val) do { \
				if ((val) != 0)  \
				  SPI2_TARGET_MOSI_PORT |= SPI2_MOSI_BIT; \
				else \
				  SPI2_TARGET_MOSI_PORT &= ~SPI2_MOSI_BIT; \
				} while (0)


		#else /* WITHSPISPLIT */
			// при программной реализации SPI
			// поддерживается режим SPI MODE 3

			// Формирование строба SPI
			#define SCLK_SET() do { SPI_TARGET_SCLK_PORT |= SPI_SCLK_BIT; } while (0)
			#define SCLK_CLR() do { SPI_TARGET_SCLK_PORT &= ~SPI_SCLK_BIT; } while (0)

			#if CPUSTYLE_ATMEGA_XXX4
				// поддержка быстрого формирования строба
				#define SCLK_TGL() do { SPI_TARGET_SCLK_PIN = SPI_SCLK_BIT; } while (0)

				#define SCLK_NPULSE() do { SCLK_TGL(); SCLK_TGL(); } while (0)
			#else
				#define SCLK_NPULSE() do { SCLK_CLR(); SCLK_SET(); } while (0)
			#endif
			// выдача данных на MOSI
			#define SDO_SET(val) do { \
				if ((val) != 0)  \
				  SPI_TARGET_MOSI_PORT |= SPI_MOSI_BIT; \
				else \
				  SPI_TARGET_MOSI_PORT &= ~SPI_MOSI_BIT; \
				} while (0)
		#endif /* WITHSPISPLIT */

	#else

		#error Missing CPUSTYLE_xxx

	#endif

	#if WITHSPISPLIT	/* для двух разных потребителей формируются отдельные сигналы MOSI, SCK, CS */
		//интерфейс с платой - выдача одного бита на последовательный канал
		#if defined (SPI_CSEL2)
			#define prog_bit(target, bitv) \
				do { \
					switch (target) \
					{ \
					case SPI_CSEL0: \
						SDO0_SET(bitv);	/* запись бита информации */ \
						SCLK0_NPULSE();	/* latch to chips */ \
						break; \
					case SPI_CSEL1: \
						SDO1_SET(bitv);	/* запись бита информации */ \
						SCLK1_NPULSE();	/* latch to chips */ \
						break; \
					case SPI_CSEL2: \
						SDO2_SET(bitv);	/* запись бита информации */ \
						SCLK2_NPULSE();	/* latch to chips */ \
						break; \
					} \
				} while (0)
		#else
			#define prog_bit(target, bitv) \
				do { \
					switch (target) \
					{ \
					case SPI_CSEL0: \
						SDO0_SET(bitv);	/* запись бита информации */ \
						SCLK0_NPULSE();	/* latch to chips */ \
						break; \
					case SPI_CSEL1: \
						SDO1_SET(bitv);	/* запись бита информации */ \
						SCLK1_NPULSE();	/* latch to chips */ \
						break; \
					} \
				} while (0)
		#endif
		// ---------------- end of optimizations
	#else /* WITHSPISPLIT */
		//интерфейс с платой - выдача одного бита на последовательный канал
		#define prog_bit(target, bitv) \
			do { \
				(void) (target); \
				SDO_SET(bitv);	/* запись бита информации */ \
				SCLK_NPULSE();	/* latch to chips */ \
			} while (0)
		// ---------------- end of optimizations
	#endif /* WITHSPISPLIT */

	void prog_val_impl(
		spitarget_t target,	/* addressing to chip */
		uint_fast8_t value,
		uint_fast8_t n				/* number of bits to send */
		);
	void NOINLINEAT prog_bit_impl(
		spitarget_t target,	/* addressing to chip */
		uint_fast8_t value
		);
	void NOINLINEAT prog_val8_impl(
		spitarget_t target,	/* addressing to chip */
		uint_fast8_t value
		);

	/* выдача указанного количества битов с любым содержимым.
	   Используется для выравнивания общего количества битов на значение, кратное 8. */
	void prog_dummy_impl(
		spitarget_t target,	/* addressing to chip */
		uint_fast8_t n				/* number of bits to send */
		);

	void prog_phbits_impl(
		spitarget_t target,	/* addressing to chip */
		const phase_t * val,
		uint_fast8_t i,				/* left of 1-st bit to set */
		uint_fast8_t n				/* number of bits to send */
		);


	// Эти три функции должны использоваться везде, где надо работать с SPI.
	#define prog_val(target, value, n) do { (void) target; prog_val_impl((target), (value), (n)); } while (0)
	#define prog_val8(target, value) do { (void) target; prog_val8_impl((target), (value)); } while (0)
	#define prog_phbits(target, val, i, n) do { (void) target; prog_phbits_impl((target), (val), (i), (n)); } while (0)

	/* выдача указанного количества битов с любым содержимым.
	   Используется для выравнивания общего количества битов на значение, кратное 8. */
	#define prog_dummy(target, n) do { prog_val_impl((target), 0x00, n); } while (0)	

#endif /* WITHSPISW */


// Работа совместно с фоновым обменом SPI по прерываниям
// Assert CS, send and then read  bytes via SPI, and deassert CS
// При приеме на сигнале MOSI должно обеспечиваться состояние логической "1" для корректной работы SD CARD
void prog_spi_io(
	spitarget_t target, spi_speeds_t spispeedindex, spi_modes_t spimode,
	const uint8_t * txbuff1, unsigned int txsize1,
	const uint8_t * txbuff2, unsigned int txsize2,
	uint8_t * rxbuff, unsigned int rxsize
	);

// Работа совместно с фоновым обменом SPI по прерываниям
// Assert CS, send and then read  bytes via SPI, and deassert CS
// Выдача и прием ответных байтов
void prog_spi_exchange(
	spitarget_t target, spi_speeds_t spispeedindex, spi_modes_t spimode,
	const uint8_t * txbuff,
	uint8_t * rxbuff,
	unsigned int size
	);

// Работа совместно с фоновым обменом SPI по прерываниям
// Assert CS, send and then read  bytes via SPI, and deassert CS
// Выдача и прием ответных байтов
void prog_spi_exchange32(
	spitarget_t target, spi_speeds_t spispeedindex, spi_modes_t spimode,
	const uint32_t * txbuff,
	uint32_t * rxbuff,
	unsigned int size
	);

void hardware_spi_slave_initialize(void);		
void hardware_spi_slave_enable(uint_fast8_t spimode);	
void hardware_spi_slave_callback(uint8_t * buff, uint_fast8_t len);


void hardware_spi_master_initialize(void);		/* инициализация и перевод в состояние "отключено" */
void hardware_spi_master_setfreq(spi_speeds_t spispeedindex, int_fast32_t spispeed);

void board_reload_fir_artix7_p1(spitarget_t target, uint_fast8_t v1, uint_fast32_t v2);
void board_reload_fir_artix7_p2(spitarget_t target, uint_fast8_t v1, uint_fast32_t v2);

void board_fpga_fir_coef_p1(int_fast32_t v);	// Передача одного (первого) 32-битного значения и формирование строба.
void board_fpga_fir_coef_p2(int_fast32_t v);	// Передача одного (последующего) 32-битного значения и формирование строба.
void board_fpga_fir_complete(void);
void board_fpga_fir_connect(IRQL_t * oldIrql);
void board_fpga_fir_disconnect(IRQL_t irql);

// Serial flash (boot memory) interface
void spidf_initialize(void);

uint_fast8_t dataflash_read_status(void);
int timed_dataflash_read_status(void);
int largetimed_dataflash_read_status(void);	// infinity waiting
int testchipDATAFLASH(void);
int prepareDATAFLASH(void);
int sectoreraseDATAFLASH(unsigned long flashoffset);
int writeDATAFLASH(unsigned long flashoffset, const uint8_t * data, unsigned long len);
int verifyDATAFLASH(unsigned long flashoffset, const uint8_t * data, unsigned long len);
int readDATAFLASH(unsigned long flashoffset, uint8_t * data, unsigned long len);
int fullEraseDATAFLASH(void);
void hangoffDATAFLASH(void);	// Отключить процессор от SERIAL FLASH
unsigned long sectorsizeDATAFLASH(void);
unsigned long chipsizeDATAFLASH(void);
extern char nameDATAFLASH [];

void bootloader_readimage(unsigned long flashoffset, uint8_t * dest, unsigned Len);
void bootloader_verifyimage(unsigned long flashoffset, const uint8_t * dest, unsigned Len);
void bootloader_chiperase(void);

void nand_initialize(void);
void nand_tests(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // SPIFUNCS_H_INCLUDED
