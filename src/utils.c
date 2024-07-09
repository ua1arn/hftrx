/*
 * utils.c
 *
 *  Created on: Sep 30, 2023
 *      Author: Gena
 */

/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "utils.h"


// битовые маски, соответствующие биту в байте по его номеру.
const uint_fast8_t rbvalues [8] =
{
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
};


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

unsigned long ulmin(
	unsigned long a,
	unsigned long b)
{
	return a < b ? a : b;
}

unsigned long ulmax(
	unsigned long a,
	unsigned long b)
{
	return a > b ? a : b;
}

signed long slmin(
	signed long a,
	signed long b)
{
	return a < b ? a : b;
}

signed long slmax(
	signed long a,
	signed long b)
{
	return a > b ? a : b;
}

uint_fast32_t ulmin32(uint_fast32_t a, uint_fast32_t b)
{
	return a < b ? a : b;
}

uint_fast32_t ulmax32(uint_fast32_t a, uint_fast32_t b)
{
	return a > b ? a : b;
}

uint_fast16_t ulmin16(uint_fast16_t a, uint_fast16_t b)
{
	return a < b ? a : b;
}

uint_fast16_t ulmax16(uint_fast16_t a, uint_fast16_t b)
{
	return a > b ? a : b;
}

/* получить 16-бит значение */
uint_fast16_t
USBD_peek_u16(
	const uint8_t * buff
	)
{
	return
		((uint_fast16_t) buff [1] << 8) +
		((uint_fast16_t) buff [0] << 0);
}

/* получить 24-бит значение */
uint_fast32_t
USBD_peek_u24(
	const uint8_t * buff
	)
{
	return
		((uint_fast32_t) buff [2] << 16) +
		((uint_fast32_t) buff [1] << 8) +
		((uint_fast32_t) buff [0] << 0);
}

/* получить 32-бит значение */
/* Low endian memory layout */
uint_fast32_t
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
/* Low endian memory layout */
float
USBD_peek_IEEE_FLOAT(
	const uint8_t * buff
	)
{
	union
	{
		float f;
		int32_t i;
	} v;
	v.i =
		((uint_fast32_t) buff [3] << 24) +
		((uint_fast32_t) buff [2] << 16) +
		((uint_fast32_t) buff [1] << 8) +
		((uint_fast32_t) buff [0] << 0);
	return v.f;
}

/* записать в буфер для ответа 32-бит значение */
unsigned USBD_poke_u32(uint8_t * buff, uint_fast32_t v)
{
	buff [0] = LO_BYTE(v);
	buff [1] = HI_BYTE(v);
	buff [2] = HI_24BY(v);
	buff [3] = HI_32BY(v);

	return 4;
}

/* записать в буфер для ответа 32-бит значение */
unsigned USBD_poke_IEEE_FLOAT(uint8_t * buff, float f)
{
	union
	{
		float f;
		int32_t i;
	} v;
	v.f = f;
	buff [0] = LO_BYTE(v.i);
	buff [1] = HI_BYTE(v.i);
	buff [2] = HI_24BY(v.i);
	buff [3] = HI_32BY(v.i);

	return 4;
}

/* получить 16-бит значение */
/* Big endian memory layout */
uint_fast16_t
USBD_peek_u16_BE(
	const uint8_t * buff
	)
{
	return
		((uint_fast32_t) buff [0] << 8) +
		((uint_fast32_t) buff [1] << 0);
}

/* получить 32-бит значение */
/* Big endian memory layout */
uint_fast32_t
USBD_peek_u32_BE(
	const uint8_t * buff
	)
{
	return
		((uint_fast32_t) buff [0] << 24) +
		((uint_fast32_t) buff [1] << 16) +
		((uint_fast32_t) buff [2] << 8) +
		((uint_fast32_t) buff [3] << 0);
}

/* получить 64-бит значение */
/* Big endian memory layout */
uint_fast64_t
USBD_peek_u64_BE(
	const uint8_t * buff
	)
{
	return
		((uint_fast64_t) buff [0] << 56) +
		((uint_fast64_t) buff [1] << 48) +
		((uint_fast64_t) buff [2] << 40) +
		((uint_fast64_t) buff [3] << 32) +
		((uint_fast64_t) buff [4] << 24) +
		((uint_fast64_t) buff [5] << 16) +
		((uint_fast64_t) buff [6] << 8) +
		((uint_fast64_t) buff [7] << 0);
}

/* записать в буфер для ответа 32-бит значение */
/* Big endian memory layout */
unsigned USBD_poke_u32_BE(uint8_t * buff, uint_fast32_t v)
{
	buff [3] = LO_BYTE(v);
	buff [2] = HI_BYTE(v);
	buff [1] = HI_24BY(v);
	buff [0] = HI_32BY(v);

	return 4;
}

/* записать в буфер для ответа 64-бит значение */
/* Big endian memory layout */
unsigned USBD_poke_u64_BE(uint8_t * buff, uint_fast64_t v)
{
	buff [0] = (v >> 56) & 0xFF;
	buff [1] = (v >> 48) & 0xFF;
	buff [2] = (v >> 40) & 0xFF;
	buff [3] = (v >> 32) & 0xFF;
	buff [4] = (v >> 24) & 0xFF;
	buff [5] = (v >> 16) & 0xFF;
	buff [6] = (v >> 8) & 0xFF;
	buff [7] = (v >> 0) & 0xFF;

	return 8;
}

/* записать в буфер для ответа 24-бит значение */
unsigned USBD_poke_u24(uint8_t * buff, uint_fast32_t v)
{
	buff [0] = LO_BYTE(v);
	buff [1] = HI_BYTE(v);
	buff [2] = HI_24BY(v);

	return 3;
}

/* записать в буфер для ответа 16-бит значение */
unsigned USBD_poke_u16(uint8_t * buff, uint_fast16_t v)
{
	buff [0] = LO_BYTE(v);
	buff [1] = HI_BYTE(v);

	return 2;
}

/* записать в буфер для ответа 16-бит значение */
/* Big endian memory layout */
unsigned USBD_poke_u16_BE(uint8_t * buff, uint_fast16_t v)
{
	buff [1] = LO_BYTE(v);
	buff [0] = HI_BYTE(v);

	return 2;
}

/* записать в буфер для ответа 8-бит значение */
unsigned USBD_poke_u8(uint8_t * buff, uint_fast8_t v)
{
	buff [0] = v;

	return 1;
}

//////////////////////////////////////
/// Queue support
///


void uint8_queue_init(u8queue_t * q, uint8_t * buff, unsigned sz)
{
	q->qg = q->qp = 0;
	q->size = sz;
	q->buffer = buff;
}

uint_fast8_t uint8_queue_put(u8queue_t * q, uint_fast8_t c)
{
	unsigned qpt = q->qp;
	const unsigned next = (qpt + 1) % q->size;
	if (next != q->qg)
	{
		q->buffer [qpt] = c;
		q->qp = next;
		return 1;
	}
	return 0;
}

uint_fast8_t uint8_queue_get(u8queue_t * q, uint_fast8_t * pc)
{
	unsigned qgt = q->qg;
	if (q->qp != qgt)
	{
		* pc = q->buffer [qgt];
		q->qg = (qgt + 1) % q->size;
		return 1;
	}
	return 0;
}

uint_fast8_t uint8_queue_empty(const u8queue_t * q)
{
	return q->qp == q->qg;
}


