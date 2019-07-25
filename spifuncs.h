/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#ifndef SPIFUNCS_H_INCLUDED
#define SPIFUNCS_H_INCLUDED

#include <stdint.h>

#include "hardware.h"
#include "pio.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void spi_initialize(void);	// отдельно инициализация SPI

#if WITHSPISW
	#if CPUSTYLE_ARM || CPUSTYLE_ATXMEGA
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

// Эти три функции должны использоваться везде, где надо работать с SPI.
#define prog_select(target) do { prog_select_impl(target); } while (0)
#define prog_unselect(target) do { (void) (target); prog_unselect_impl(); } while (0)
#define prog_read_byte(target, v)  ((void) (target), prog_spi_read_byte_impl(v))


void prog_pulse_ioupdate(void);

void prog_select_impl(
	spitarget_t target	/* SHIFTED addressing to chip (on ATMEGA - may be bit mask) */
	);
void prog_unselect_impl(void);

uint_fast8_t prog_spi_read_byte_impl(uint_fast8_t bytetosend);

// Send a frame of bytes via SPI
void prog_spi_send_frame(
	spitarget_t target,
	const uint8_t * buff, 
	unsigned int size
	);

// Read a frame of bytes via SPI
// Приём блока
// На сигнале MOSI при этом должно обеспачиваться состояние логической "1" для корректной работы SD CARD
void prog_spi_read_frame(
	spitarget_t target,
	uint8_t * buff, 
	unsigned int size
	);


#if SPI_BIDIRECTIONAL

	void prog_spi_to_read_impl(void);
	void prog_spi_to_write_impl(void);

	//void (prog_spi_to_read)(spitarget_t target);
	//void (prog_spi_to_write)(spitarget_t target);

	#define spi_to_read(target) do { prog_spi_to_read_impl(); } while (0)
	#define spi_to_write(target) do { prog_spi_to_write_impl(); } while (0)


#else
	#define spi_to_read(target) do { } while (0)
	#define spi_to_write(target) do { } while (0)
#endif


// 8-ми битные функции обмена по SPI
#if WITHSPIHW && ! WITHSPISW
	/* только аппаратный SPI */
	#define spi_select(target, spimode) \
		do { hardware_spi_connect(SPIC_SPEEDFAST, (spimode)); prog_select(target); } while (0)	// начало выдачи информации по SPI
	#define spi_select2(target, spimode, speedcode) \
		do { hardware_spi_connect((speedcode), (spimode)); prog_select(target); } while (0)	// начало выдачи информации по SPI
	#define spi_unselect(target) \
		do { prog_unselect(target); hardware_spi_disconnect(); } while (0)	// заверщение выдачи информации по SPI  - поднять чипселект
	#define spi_complete(target) \
		((void) (target), hardware_spi_complete_b8())		// ожидание выдачи последнего байта в последовательности
	#define spi_progval8(target, v) \
		((void) (target), hardware_spi_b8(v))	// выдача байта и дождаться его передачи
	#define spi_progval8_p1(target, v) \
		do { (void) (target); hardware_spi_b8_p1(v); } while (0)	// выдача первого байта в последовательности
	#define spi_progval8_p2(target, v) \
		do { (void) (target); hardware_spi_b8_p2(v); } while (0)	// выдача средних байтов в последовательности
	#define spi_progval8_p3(target, v) \
		((void) (target), hardware_spi_b8_p2(v), hardware_spi_complete_b8())	// выдача средних байтов в последовательности
	#define spi_read_byte(target, v) \
			((void) (target), hardware_spi_b8(v))
	#if WITHSPIHWDMA
		#define spi_send_frame(target, buff, count) \
			do { (void) (target); hardware_spi_master_send_frame((buff), (count)); } while (0)	// Передача блока
		#define spi_read_frame(target, buff, count) \
			do { (void) (target); hardware_spi_master_read_frame((buff), (count)); } while (0)	// Приём блока
	#else /* WITHSPIHWDMA */
		#define spi_send_frame(target, buff, count) \
			do { prog_spi_send_frame((target), (buff), (count)); } while (0)	// Передача блока
		#define spi_read_frame(target, buff, count) \
			do { prog_spi_read_frame((target), (buff), (count)); } while (0)	// Приём блока
	#endif /* WITHSPIHWDMA */
#elif WITHSPIHW
	/* аппаратный и программный SPI - следующая передача может оказаться программной - потому отсоединяемся */
	#define spi_select(target, spimode) \
		do { hardware_spi_connect(SPIC_SPEEDFAST, (spimode)); prog_select(target); } while (0)	// начало выдачи информации по SPI
	#define spi_select2(target, spimode, speedcode) \
		do { hardware_spi_connect((speedcode), (spimode)); prog_select(target); } while (0)	// начало выдачи информации по SPI
	#define spi_unselect(target) \
		do { prog_unselect(target); hardware_spi_disconnect(); } while (0)	// заверщение выдачи информации по SPI  - поднять чипселект
	#define spi_complete(target) \
		((void) (target), hardware_spi_complete_b8())		// ожидание выдачи последнего байта в последовательности
	#define spi_progval8(target, v) \
		((void) (target), hardware_spi_b8(v))	// выдача байта и дождаться его передачи
	#define spi_progval8_p1(target, v) \
		do { (void) (target); hardware_spi_b8_p1(v); } while (0)	// выдача первого байта в последовательности
	#define spi_progval8_p2(target, v) \
		do { (void) (target); hardware_spi_b8_p2(v); } while (0)	// выдача средних байтов в последовательности
	#define spi_progval8_p3(target, v) \
		((void) (target), hardware_spi_b8_p2(v), hardware_spi_complete_b8())	// выдача средних байтов в последовательности
	#define spi_read_byte(target, v) \
			((void) (target), hardware_spi_b8(v))
	#if WITHSPIHWDMA
		#define spi_send_frame(target, buff, count) \
			do { (void) (target); hardware_spi_master_send_frame((buff), (count)); } while (0)	// Передача блока
		#define spi_read_frame(target, buff, count) \
			do { (void) (target); hardware_spi_master_read_frame((buff), (count)); } while (0)	// Приём блока
	#else /* WITHSPIHWDMA */
		#define spi_send_frame(target, buff, count) \
			do { prog_spi_send_frame((target), (buff), (count)); } while (0)	// Передача блока
		#define spi_read_frame(target, buff, count) \
			do { prog_spi_read_frame((target), (buff), (count)); } while (0)	// Приём блока
	#endif /* WITHSPIHWDMA */
#elif WITHSPISW
	/* только программный SPI */
	#define spi_select(target, spimode) \
		do { prog_select(target); } while (0)	// начало выдачи информации по SPI
	#define spi_select2(target, spimode, speedcode) \
		do { prog_select(target); } while (0)	// начало выдачи информации по SPI
	#define spi_unselect(target) \
		do { prog_unselect(target); } while (0)	// заверщение выдачи информации по SPI
	#define spi_complete(target) \
		do { (void) (target); } while (0)
	#define spi_progval8(target, v) \
			prog_read_byte(target, v)
	#define spi_progval8_p1(target, v) \
		do { prog_val8((target), (v)); } while (0)
	#define spi_progval8_p2(target, v) \
		do { prog_val8((target), (v)); } while (0)
	#define spi_progval8_p3(target, v) \
			prog_read_byte(target, v)
	#define spi_read_byte(target, v) \
			prog_read_byte(target, v)
	#define spi_send_frame(target, buff, count) \
		do { prog_spi_send_frame((target), (buff), (count)); } while (0)	// Передача блока
	#define spi_read_frame(target, buff, count) \
		do { prog_spi_read_frame((target), (buff), (count)); } while (0)	// Приём блока
#endif /* WITHSPIHW */

/* поддержка побитового формирования значений для вывода в SPI устройство. */
typedef uint8_t rbtype_t;
extern const uint_fast8_t rbvalues [8];	// битовые маски, соответствующие биту в байте по его номеру.

#define RBBIT(bitpos, v) \
	do { \
		if ((v) != 0) \
			rbbuff [(sizeof rbbuff / sizeof rbbuff [0]) - 1 - (bitpos) / 8] |= rbvalues [(bitpos) % 8]; \
	} while (0)

// Для ширины поля до 8 бит
#define RBVAL(rightbitpos, v, width) \
	do { \
		uint_fast8_t v2 = v; \
		uint_fast8_t p = rightbitpos; \
		uint_fast8_t i; \
		for (i = 0; i < (width); ++ i, v2 >>= 1, ++ p) \
		{	RBBIT(p, v2 & 0x01); \
		}	\
	} while (0)

// Для ширины поля до 16 бит
#define RBVAL_W16(rightbitpos, v, width) \
	do { \
		uint_fast16_t v2 = v; \
		uint_fast8_t p = rightbitpos; \
		uint_fast8_t i; \
		for (i = 0; i < (width); ++ i, v2 >>= 1, ++ p) \
		{	RBBIT(p, v2 & 0x01); \
		}	\
	} while (0)

// For set values in 8-bit alligned places
#define RBVAL8(rightbitpos, v) \
	do { \
		rbbuff [(sizeof rbbuff / sizeof rbbuff [0]) - 1 - (rightbitpos) / 8] = (v); \
	} while (0)

#define RBNULL(rightbitpos, width) \
	do { \
		(void) (rightbitpos); \
		(void) (width); \
	} while (0)

// 	Example of usage:
//
//	rbtype_t rbbuff [4] = { 0 };
//
//	RBVAL(0, bit0val, 1);
//	RBVAL(1, bit123val, 3);
//	RBVAL(4, bit12val, 2);
//
//	spi_select(target, CTLREG_SPIMODE);
//	spi_progval8_p1(target, rbbuff [0]);
//	spi_progval8_p2(target, rbbuff [1]);
//	spi_progval8_p2(target, rbbuff [2]);
//	spi_progval8_p2(target, rbbuff [3]);
// 	spi_complete(target);
//	spi_unselect(target);

/* Таблица разворота младших восьми бит */
//extern const FLASHMEM unsigned char revbittable [256];
uint_fast8_t revbits8(uint_fast8_t v);	// Функция разворота младших восьми бит


void hardware_spi_slave_initialize(void);		
void hardware_spi_slave_enable(uint_fast8_t spimode);	
void hardware_spi_slave_callback(uint8_t * buff, uint_fast8_t len);

// --- dsp

void hardware_spi_master_initialize(void);		/* инициализация и перевод в состояние "отключено" */
void hardware_spi_master_setfreq(uint_fast8_t spispeedindex, int_fast32_t spispeed);
void hardware_spi_connect(uint_fast8_t spispeedindex, uint_fast8_t spimode);	/* управление состоянием - подключено */
void hardware_spi_connect_b16(uint_fast8_t spispeedindex, uint_fast8_t spimode);	/* управление состоянием - подключено - работа в режиме 16-ти битных слов. */
void hardware_spi_disconnect(void);	/* управление состоянием - отключено */

portholder_t hardware_spi_b16(portholder_t v);	/* передача 16-ти бит, возврат считанного */
portholder_t hardware_spi_b8(portholder_t v);	/* передача 8-ти бит, возврат считанного */

/* группа функций для использования в групповых передачах по SPI. Количество бит определяется типом spi_connect */
void hardware_spi_b16_p1(portholder_t v);	/* передача первого слова в последовательности */
void hardware_spi_b16_p2(portholder_t v);	/* дождаться готовности, передача слова */
void hardware_spi_b8_p1(portholder_t v);	/* передача первого байта в последовательности */
void hardware_spi_b8_p2(portholder_t v);	/* дождаться готовности, передача байта */

portholder_t hardware_spi_complete_b8(void);	/* дождаться готовности передача 8-ти бит */
portholder_t hardware_spi_complete_b16(void);	/* дождаться готовности передача 16-ти бит*/

// Read a frame of bytes via SPI
// На сигнале MOSI при это должно обеспачиваться состояние логической "1" для корректной работы SD CARD
void hardware_spi_master_read_frame(uint8_t * pBuffer, uint_fast32_t size);
void hardware_spi_master_read_frame_16b(uint16_t * pBuffer, uint_fast32_t size);
// Send a frame of bytes via SPI
void hardware_spi_master_send_frame(const uint8_t * pBuffer, uint_fast32_t size);
void hardware_spi_master_send_frame_16b(const uint16_t * pBuffer, uint_fast32_t size);

// Serial flash (boot memory) interface
void spidf_initialize(void);
void spidf_uninitialize(void);
void spidf_select(spitarget_t target, uint_fast8_t mode);
void spidf_unselect(spitarget_t target);
void spidf_to_read(spitarget_t target);
void spidf_to_write(spitarget_t target);
void spidf_progval8_p1(spitarget_t target, uint_fast8_t sendval);
void spidf_progval8_p2(spitarget_t target, uint_fast8_t sendval);
uint_fast8_t spidf_complete(spitarget_t target);
uint_fast8_t spidf_progval8(spitarget_t target, uint_fast8_t sendval);
uint_fast8_t spidf_read_byte(spitarget_t target, uint_fast8_t sendval);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // SPIFUNCS_H_INCLUDED
