/* $Id: $ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include <stdint.h>

/* получить 32-бит значение */
uint_fast32_t
/* Low endian memory layout */
USBD_peek_u32(
	const uint8_t * buff
	);

/* записать в буфер для ответа 32-бит значение */
/* Low endian memory layout */
unsigned USBD_poke_u32(uint8_t * buff, uint_fast32_t v);

/* получить 24-бит значение */
/* Low endian memory layout */
uint_fast32_t
USBD_peek_u24(
	const uint8_t * buff
	);

/* записать в буфер для ответа 24-бит значение */
/* Low endian memory layout */
unsigned USBD_poke_u24(uint8_t * buff, uint_fast32_t v);

/* получить 16-бит значение */
/* Low endian memory layout */
uint_fast16_t
USBD_peek_u16(
	const uint8_t * buff
	);

/* записать в буфер для ответа 16-бит значение */
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

#endif /* INC_UTILS_H_ */
