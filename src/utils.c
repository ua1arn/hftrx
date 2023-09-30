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

/* записать в буфер для ответа 32-бит значение */
unsigned USBD_poke_u32(uint8_t * buff, uint_fast32_t v)
{
	buff [0] = LO_BYTE(v);
	buff [1] = HI_BYTE(v);
	buff [2] = HI_24BY(v);
	buff [3] = HI_32BY(v);

	return 4;
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


