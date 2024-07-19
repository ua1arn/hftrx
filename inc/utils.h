/* $Id: $ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include <stdint.h>



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* получить 32-бит значение */
/* Little endian memory layout */
uint_fast32_t
USBD_peek_u32(
	const uint8_t * buff
	);
/* получить 32-бит значение */
/* Little endian memory layout */
float
USBD_peek_IEEE_FLOAT(
	const uint8_t * buff
	);

/* записать в буфер для ответа 32-бит значение */
/* Little endian memory layout */
unsigned USBD_poke_u32(uint8_t * buff, uint_fast32_t v);
/* записать в буфер для ответа 32-бит значение */
/* Little endian memory layout */
unsigned USBD_poke_IEEE_FLOAT(uint8_t * buff, float v);

/* получить 24-бит значение */
/* Little endian memory layout */
uint_fast32_t
USBD_peek_u24(
	const uint8_t * buff
	);

/* записать в буфер для ответа 24-бит значение */
/* Little endian memory layout */
unsigned USBD_poke_u24(uint8_t * buff, uint_fast32_t v);

/* получить 16-бит значение */
/* Little endian memory layout */
uint_fast16_t
USBD_peek_u16(
	const uint8_t * buff
	);

/* записать в буфер для ответа 16-бит значение */
/* Little endian memory layout */
unsigned USBD_poke_u16(uint8_t * buff, uint_fast16_t v);

/* получить 8-бит значение */
uint_fast8_t
USBD_peek_u8(
	const uint8_t * buff
	);

/* записать в буфер для ответа 8-бит значение */
unsigned USBD_poke_u8(uint8_t * buff, uint_fast8_t v);

/* получить 32-бит значение */
/* Big endian memory layout */
uint_fast32_t
USBD_peek_u32_BE(
	const uint8_t * buff
	);

/* получить 16-бит значение */
/* Big endian memory layout */
uint_fast16_t
USBD_peek_u16_BE(
	const uint8_t * buff
	);

/* получить 64-бит значение */
/* Big endian memory layout */
uint_fast64_t
USBD_peek_u64_BE(
	const uint8_t * buff
	);

/* записать в буфер для ответа n-бит значение */
/* Big endian memory layout */
unsigned USBD_poke_u32_BE(uint8_t * buff, uint_fast32_t v);
unsigned USBD_poke_u64_BE(uint8_t * buff, uint_fast64_t v);
unsigned USBD_poke_u24_BE(uint8_t * buff, uint_fast32_t v);
unsigned USBD_poke_u16_BE(uint8_t * buff, uint_fast16_t v);

uint_fast32_t ulmin32(uint_fast32_t a, uint_fast32_t b);
uint_fast32_t ulmax32(uint_fast32_t a, uint_fast32_t b);
uint_fast16_t ulmin16(uint_fast16_t a, uint_fast16_t b);
uint_fast16_t ulmax16(uint_fast16_t a, uint_fast16_t b);
unsigned long ulmin(unsigned long a, unsigned long b);
unsigned long ulmax(unsigned long a, unsigned long b);
signed long slmin(signed long a, signed long b);
signed long slmax(signed long a, signed long b);

#define  HI_32BY(w)  (((w) >> 24) & 0xFF)   /* Extract 31..24 bits from unsigned word */
#define  HI_24BY(w)  (((w) >> 16) & 0xFF)   /* Extract 23..16 bits from unsigned word */
#define  HI_BYTE(w)  (((w) >> 8) & 0xFF)   /* Extract high-order byte from unsigned word */
#define  LO_BYTE(w)  ((w) & 0xFF)          /* Extract low-order byte from unsigned word */

/////////////////////
/// Queue support
///


typedef struct u8queue
{
	uint8_t * buffer;
	unsigned size;
	unsigned qg;
	unsigned qp;
} u8queue_t;

void uint8_queue_init(u8queue_t * q, uint8_t * buff, unsigned sz);
uint_fast8_t uint8_queue_put(u8queue_t * q, uint_fast8_t c);
uint_fast8_t uint8_queue_get(u8queue_t * q, uint_fast8_t * pc);
uint_fast8_t uint8_queue_empty(const u8queue_t * q);


/* поддержка побитового формирования значений для вывода в SPI устройство. */
typedef uint8_t rbtype_t;
extern const uint_fast8_t rbvalues [8];	// битовые маски, соответствующие биту в байте по его номеру.

#define RBBIT(bitpos, v) do { \
		if ((v) != 0) \
			rbbuff [(sizeof rbbuff / sizeof rbbuff [0]) - 1 - (bitpos) / 8] |= rbvalues [(bitpos) % 8]; \
	} while (0)

// Для ширины поля до 8 бит
#define RBVAL(rightbitpos, v, width)  do { \
		uint_fast8_t v2_507 = (v); \
		uint_fast8_t p_508 = (rightbitpos); \
		uint_fast8_t i_509; \
		for (i_509 = 0; i_509 < (width); ++ i_509, v2_507 >>= 1, ++ p_508) \
		{	RBBIT(p_508, v2_507 & 0x01); \
		}	\
	} while (0)

// Для ширины поля до 16 бит
#define RBVAL_W16(rightbitpos, v, width) do { \
		uint_fast16_t v2_515 = (v); \
		uint_fast8_t p_516 = (rightbitpos); \
		uint_fast8_t i; \
		for (i = 0; i < (width); ++ i, v2_515 >>= 1, ++ p_516) \
		{	RBBIT(p, v2_515 & 0x01); \
		}	\
	} while (0)

// For set values in 8-bit alligned places
#define RBVAL8(rightbitpos, v) do { \
		rbbuff [(sizeof rbbuff / sizeof rbbuff [0]) - 1 - (rightbitpos) / 8] = (v); \
	} while (0)

#define RBNULL(rightbitpos, width) do { \
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
//	IRQL_t irql
//	spi_operate_lock(& irql);
//	spi_select(target, CTLREG_SPIMODE);
//	spi_progval8_p1(target, rbbuff [0]);
//	spi_progval8_p2(target, rbbuff [1]);
//	spi_progval8_p2(target, rbbuff [2]);
//	spi_progval8_p2(target, rbbuff [3]);
// 	spi_complete(target);
//	spi_unselect(target);
//	spi_operate_unlock(irql);

/* Таблица разворота младших восьми бит */
//extern const FLASHMEM unsigned char revbittable [256];
uint_fast8_t revbits8(uint_fast8_t v);	// Функция разворота младших восьми бит

uint32_t ptr_hi32(uintptr_t v);
uint32_t ptr_lo32(uintptr_t v);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* INC_UTILS_H_ */
