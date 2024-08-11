/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#ifndef FORMATS_H_INCLUDED
#define FORMATS_H_INCLUDED

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>	// format specifiers for printing

#include "hardware.h"
#include "serial.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

uint_fast8_t local_snprintf_P( char * __restrict buffer, uint_fast8_t count, const FLASHMEM char * __restrict format, ... ) __attribute__ ((__format__ (__printf__, 3, 4)));
uint_fast8_t local_vsnprintf_P( char * __restrict buffer, uint_fast8_t count, const FLASHMEM char * __restrict format, va_list ap ) __attribute__ ((__format__ (__printf__, 3, 0)));
// Отладочная печать
void debug_printf_P(const FLASHMEM char * __restrict format, ... ) __attribute__ ((__format__ (__printf__, 1, 2)));

char * safestrcpy(char * dst, size_t blen, const char * src);
void strtrim(char * s);

void printhex(uintptr_t offs, const void * buff, unsigned length);
void printhex32(uintptr_t voffs, const void * vbuff, unsigned length);
void printhex64(uintptr_t voffs, const void * vbuff, unsigned length);

// spool-based functions for debug
int dbg_puts_impl_P(const FLASHMEM char * s);
int dbg_puts_impl(const char * s);
int dbg_putchar(int c);
int dbg_writechar(int c);	/* вывод символа без преобразования '\n' в пару символов '\r' '\n' */
int dbg_getchar(char * r);
void dbg_flush(void); /* дождаться, пока будут переданы все символы, ы том числе и из FIFO */

#if WITHDEBUG
	#define TP() \
		do { \
			static const char this_file [] = __FILE__; \
			PRINTF(PSTR("At %d in %s.\n"), __LINE__, this_file); \
		} while(0)
#else /* WITHDEBUG */
	#define TP() do { } while(0)
#endif /* WITHDEBUG */

#if WITHDEBUG
#if LINUX_SUBSYSTEM
	#define PRINTF printf
#else
	#define PRINTF	debug_printf_P
#endif /* LINUX_SUBSYSTEM */
#else /* WITHDEBUG */
	#define PRINTF(...)	do {} while (0)
#endif /* WITHDEBUG */

#if WITHDEBUG && 1
	#define ASSERT(v) do { if ((v) == 0) { \
		PRINTF(PSTR("%s(%d): Assert '%s'\n"), __FILE__, __LINE__, (# v)); \
		for (;;) ; \
		} } while (0)
	#define ASSERT2(v, f, l) do { if ((v) == 0) { \
		PRINTF(PSTR("%s(%d): Assert '%s'\n"), (f), (l), (# v)); \
		for (;;) ; \
		} } while (0)
	#define ASSERT3(v, f, l, m) do { if ((v) == 0) { \
		PRINTF(PSTR("%s(%d): Assert '%s' (%s)\n"), (f), (l), (# v), (m)); \
		for (;;) ; \
		} } while (0)

	#define VERIFY(v) do { if ((v) == 0) { \
		PRINTF(PSTR("%s(%d): Verify '%s'\n"), __FILE__, __LINE__, (# v)); \
		for (;;) ; \
		} } while (0)

	#define VERIFY3(v, f, l, m) do { if ((v) == 0) { \
		PRINTF(PSTR("%s(%d): Verify '%s' (%s)\n"), f, l, (# v), (m)); \
		for (;;) ; \
		} } while (0)

#else /* WITHDEBUG */

	#define ASSERT(v) ((void) (0))
	#define ASSERT2(v, f, l) ((void) (0))
	#define ASSERT3(v, f, l, m) ((void) (0))
	#define VERIFY(v) ((void) (v))
	#define VERIFY3(v, f, l, m) ((void) (v))

#endif /* WITHDEBUG */

#if WITHDEBUG && WITHUART0HW && WITHDEBUG_UART0
	// Отладочные функции работают через USART0
	// Вызывается из user-mode программы при запрещённых прерываниях.
	#define HARDWARE_DEBUG_INITIALIZE() do { \
			hardware_uart0_initialize(1, DEBUGSPEED, 8, 0, 0); \
			hardware_uart0_set_speed(DEBUGSPEED); \
		} while (0)
	#define HARDWARE_DEBUG_SET_SPEED(baudrate) do { \
			hardware_uart0_set_speed(baudrate); \
		} while (0)
	#define HARDWARE_DEBUG_PUTCHAR(c) \
		(hardware_uart0_putchar(c))
	#define HARDWARE_DEBUG_GETCHAR(pc) \
		(hardware_uart0_getchar(pc))

	// вызывается из обработчика прерываний UART0
	// с принятым символом
	#define HARDWARE_UART0_ONRXCHAR(c) do { \
			(void) (c); \
			hardware_uart0_enablerx(1); \
		} while (0)
	// вызывается из обработчика прерываний UART0
	#define HARDWARE_UART0_ONOVERFLOW() do { \
		} while (0)
	// вызывается из обработчика прерываний UART0
	// по готовности передатчика
	#define HARDWARE_UART0_ONTXCHAR(ctx) do { \
			(void) ctx; \
			hardware_uart0_enabletx(0); \
		} while (0)
	// вызывается из обработчика прерываний UART0
	// по окончании передачи (сдвиговый регистр передатчика пуст)
	#define HARDWARE_UART0_ONTXDONE(ctx) do { \
			(void) ctx; \
			hardware_uart0_enabletx(0); \
		} while (0)

	// дождаться, пока будут переданы все символы, ы том числе и из FIFO
	#define HARDWARE_DEBUG_FLUSH() do { \
		hardware_uart0_flush(); \
	} while (0)

#endif /* WITHDEBUG && WITHUART0HW && WITHDEBUG_UART0 */

#if WITHDEBUG && WITHUART1HW && WITHDEBUG_UART1
	// Отладочные функции работают через USART1
	// Вызывается из user-mode программы при запрещённых прерываниях.
	#define HARDWARE_DEBUG_INITIALIZE() do { \
			hardware_uart1_initialize(1, DEBUGSPEED, 8, 0, 0); \
			hardware_uart1_set_speed(DEBUGSPEED); \
		} while (0)
	#define HARDWARE_DEBUG_SET_SPEED(baudrate) do { \
			hardware_uart1_set_speed(baudrate); \
		} while (0)
	#define HARDWARE_DEBUG_PUTCHAR(c) \
		(hardware_uart1_putchar(c))
	#define HARDWARE_DEBUG_GETCHAR(pc) \
		(hardware_uart1_getchar(pc))

	// вызывается из обработчика прерываний UART1
	// с принятым символом
	#define HARDWARE_UART1_ONRXCHAR(c) do { \
			(void) (c); \
			hardware_uart1_enablerx(1); \
		} while (0)
	// вызывается из обработчика прерываний UART1
	#define HARDWARE_UART1_ONOVERFLOW() do { \
		} while (0)
	// вызывается из обработчика прерываний UART1
	// по готовности передатчика
	#define HARDWARE_UART1_ONTXCHAR(ctx) do { \
			(void) ctx; \
			hardware_uart1_enabletx(0); \
		} while (0)
	// вызывается из обработчика прерываний UART1
	// по окончании передачи (сдвиговый регистр передатчика пуст)
	#define HARDWARE_UART1_ONTXDONE(ctx) do { \
			(void) ctx; \
			hardware_uart1_enabletx(0); \
		} while (0)

	// дождаться, пока будут переданы все символы, ы том числе и из FIFO
	#define HARDWARE_DEBUG_FLUSH() do { \
		hardware_uart1_flush(); \
	} while (0)

#endif /* WITHDEBUG && WITHUART1HW && WITHDEBUG_UART1 */

#if WITHDEBUG && WITHUART2HW && WITHDEBUG_UART2
	// Отладочные функции работают через USART2
	// Вызывается из user-mode программы при запрещённых прерываниях.
	#define HARDWARE_DEBUG_INITIALIZE() do { \
			hardware_uart2_initialize(1, DEBUGSPEED, 8, 0, 0); \
			hardware_uart2_set_speed(DEBUGSPEED); \
		} while (0)
	#define HARDWARE_DEBUG_SET_SPEED(baudrate) do { \
			hardware_uart2_set_speed(baudrate); \
		} while (0)
	#define HARDWARE_DEBUG_PUTCHAR(c) \
		(hardware_uart2_putchar(c))
	#define HARDWARE_DEBUG_GETCHAR(pc) \
		(hardware_uart2_getchar(pc))

	// вызывается из обработчика прерываний USART2
	// с принятым символом
	#define HARDWARE_UART2_ONRXCHAR(c) do { \
			(void) (c); \
			hardware_uart2_enablerx(1); \
		} while (0)
	// вызывается из обработчика прерываний USART2
	#define HARDWARE_UART2_ONOVERFLOW() do { \
		} while (0)
	// вызывается из обработчика прерываний USART2
	// по готовности передатчика
	#define HARDWARE_UART2_ONTXCHAR(ctx) do { \
			(void) ctx; \
			hardware_uart2_enabletx(0); \
		} while (0)
	// вызывается из обработчика прерываний UART1
	// по окончании передачи (сдвиговый регистр передатчика пуст)
	#define HARDWARE_UART2_ONTXDONE(ctx) do { \
			(void) ctx; \
			hardware_uart2_enabletx(0); \
		} while (0)

	// дождаться, пока будут переданы все символы, ы том числе и из FIFO
	#define HARDWARE_DEBUG_FLUSH() do { \
		hardware_uart2_flush(); \
	} while (0)

#endif /* WITHDEBUG && WITHUART2HW && WITHDEBUG_UART2 */

#if WITHDEBUG && WITHUART3HW && WITHDEBUG_UART3
	// Отладочные функции работают через USART3
	// Вызывается из user-mode программы при запрещённых прерываниях.
	#define HARDWARE_DEBUG_INITIALIZE() do { \
			hardware_uart3_initialize(1, DEBUGSPEED, 8, 0, 0); \
			hardware_uart3_set_speed(DEBUGSPEED); \
		} while (0)
	#define HARDWARE_DEBUG_SET_SPEED(baudrate) do { \
			hardware_uart3_set_speed(baudrate); \
		} while (0)
	#define HARDWARE_DEBUG_PUTCHAR(c) \
		(hardware_uart3_putchar(c))
	#define HARDWARE_DEBUG_GETCHAR(pc) \
		(hardware_uart3_getchar(pc))

	// вызывается из обработчика прерываний USART3
	// с принятым символом
	#define HARDWARE_UART3_ONRXCHAR(c) do { \
			(void) (c); \
			hardware_uart3_enablerx(1); \
		} while (0)
	// вызывается из обработчика прерываний USART3
	#define HARDWARE_UART3_ONOVERFLOW() do { \
		} while (0)
	// вызывается из обработчика прерываний USART3
	// по готовности передатчика
	#define HARDWARE_UART3_ONTXCHAR(ctx) do { \
			(void) ctx; \
			hardware_uart3_enabletx(0); \
		} while (0)
	// вызывается из обработчика прерываний UART3
	// по окончании передачи (сдвиговый регистр передатчика пуст)
	#define HARDWARE_UART3_ONTXDONE(ctx) do { \
			(void) ctx; \
			hardware_uart3_enabletx(0); \
		} while (0)

	// дождаться, пока будут переданы все символы, ы том числе и из FIFO
	#define HARDWARE_DEBUG_FLUSH() do { \
		hardware_uart3_flush(); \
	} while (0)

#endif /* WITHDEBUG && WITHUART3HW && WITHDEBUG_UART3 */

#if WITHDEBUG && WITHUART4HW && WITHDEBUG_UART4
	// Отладочные функции работают через USART4
	// Вызывается из user-mode программы при запрещённых прерываниях.
	#define HARDWARE_DEBUG_INITIALIZE() do { \
			hardware_uart4_initialize(1, DEBUGSPEED, 8, 0, 0); \
			hardware_uart4_set_speed(DEBUGSPEED); \
		} while (0)
	#define HARDWARE_DEBUG_SET_SPEED(baudrate) do { \
			hardware_uart4_set_speed(baudrate); \
		} while (0)
	#define HARDWARE_DEBUG_PUTCHAR(c) \
		(hardware_uart4_putchar(c))
	#define HARDWARE_DEBUG_GETCHAR(pc) \
		(hardware_uart4_getchar(pc))

	// вызывается из обработчика прерываний USART4
	// с принятым символом
	#define HARDWARE_UART4_ONRXCHAR(c) do { \
			(void) (c); \
			hardware_uart4_enablerx(1); \
		} while (0)
	// вызывается из обработчика прерываний USART4
	#define HARDWARE_UART4_ONOVERFLOW() do { \
		} while (0)
	// вызывается из обработчика прерываний USART4
	// по готовности передатчика
	#define HARDWARE_UART4_ONTXCHAR(ctx) do { \
			(void) ctx; \
			hardware_uart4_enabletx(0); \
		} while (0)
	// вызывается из обработчика прерываний UART4
	// по окончании передачи (сдвиговый регистр передатчика пуст)
	#define HARDWARE_UART4_ONTXDONE(ctx) do { \
			(void) ctx; \
			hardware_uart4_enabletx(0); \
		} while (0)

	// дождаться, пока будут переданы все символы, ы том числе и из FIFO
	#define HARDWARE_DEBUG_FLUSH() do { \
		hardware_uart4_flush(); \
	} while (0)

#endif /* WITHDEBUG && WITHUART4HW && WITHDEBUG_UART4 */

#if WITHDEBUG && WITHUART5HW && WITHDEBUG_UART5
	// Отладочные функции работают через USART5
	// Вызывается из user-mode программы при запрещённых прерываниях.
	#define HARDWARE_DEBUG_INITIALIZE() do { \
			hardware_uart5_initialize(1, DEBUGSPEED, 8, 0, 0); \
			hardware_uart5_set_speed(DEBUGSPEED); \
		} while (0)
	#define HARDWARE_DEBUG_SET_SPEED(baudrate) do { \
			hardware_uart5_set_speed(baudrate); \
		} while (0)
	#define HARDWARE_DEBUG_PUTCHAR(c) \
		(hardware_uart5_putchar(c))
	#define HARDWARE_DEBUG_GETCHAR(pc) \
		(hardware_uart5_getchar(pc))

	// вызывается из обработчика прерываний USART5
	// с принятым символом
	#define HARDWARE_UART5_ONRXCHAR(c) do { \
			(void) (c); \
			hardware_uart5_enablerx(1); \
		} while (0)
	// вызывается из обработчика прерываний USART5
	#define HARDWARE_UART5_ONOVERFLOW() do { \
		} while (0)
	// вызывается из обработчика прерываний USART5
	// по готовности передатчика
	#define HARDWARE_UART5_ONTXCHAR(ctx) do { \
			(void) ctx; \
			hardware_uart5_enabletx(0); \
		} while (0)
	// вызывается из обработчика прерываний UART5
	// по окончании передачи (сдвиговый регистр передатчика пуст)
	#define HARDWARE_UART5_ONTXDONE(ctx) do { \
			(void) ctx; \
			hardware_uart5_enabletx(0); \
		} while (0)

	// дождаться, пока будут переданы все символы, ы том числе и из FIFO
	#define HARDWARE_DEBUG_FLUSH() do { \
		hardware_uart5_flush(); \
	} while (0)

#endif /* WITHDEBUG && WITHUART5HW && WITHDEBUG_UART5 */

#if WITHDEBUG && WITHUART6HW && WITHDEBUG_UART6
	// Отладочные функции работают через USART6
	// Вызывается из user-mode программы при запрещённых прерываниях.
	#define HARDWARE_DEBUG_INITIALIZE() do { \
			hardware_uart6_initialize(1, DEBUGSPEED, 8, 0, 0); \
			hardware_uart6_set_speed(DEBUGSPEED); \
		} while (0)
	#define HARDWARE_DEBUG_SET_SPEED(baudrate) do { \
			hardware_uart6_set_speed(baudrate); \
		} while (0)
	#define HARDWARE_DEBUG_PUTCHAR(c) \
		(hardware_uart6_putchar(c))
	#define HARDWARE_DEBUG_GETCHAR(pc) \
		(hardware_uart6_getchar(pc))

	// вызывается из обработчика прерываний USART5
	// с принятым символом
	#define HARDWARE_UART6_ONRXCHAR(c) do { \
			(void) (c); \
			hardware_uart6_enablerx(1); \
		} while (0)
	// вызывается из обработчика прерываний USART5
	#define HARDWARE_UART6_ONOVERFLOW() do { \
		} while (0)
	// вызывается из обработчика прерываний USART5
	// по готовности передатчика
	#define HARDWARE_UART6_ONTXCHAR(ctx) do { \
			(void) ctx; \
			hardware_uart6_enabletx(0); \
		} while (0)
	// вызывается из обработчика прерываний UART5
	// по окончании передачи (сдвиговый регистр передатчика пуст)
	#define HARDWARE_UART6_ONTXDONE(ctx) do { \
			(void) ctx; \
			hardware_uart6_enabletx(0); \
		} while (0)

	// дождаться, пока будут переданы все символы, ы том числе и из FIFO
	#define HARDWARE_DEBUG_FLUSH() do { \
		hardware_uart6_flush(); \
	} while (0)

#endif /* WITHDEBUG && WITHUART6HW && WITHDEBUG_UART6 */

#if WITHCAT && WITHUART0HW && WITHCAT_UART0
	// CAT функции работают через UART0
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_INITIALIZE() do { \
			hardware_uart0_initialize(0, DEBUGSPEED, 8, 0, 0); \
		} while (0)
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_SET_SPEED(baudrate) do { \
			hardware_uart0_set_speed(baudrate); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_CAT_ENABLETX(v) do { \
			hardware_uart0_enabletx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_CAT_ENABLERX(v) do { \
			hardware_uart0_enablerx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_CAT_TX(ctx, c) do { \
			hardware_uart0_tx((ctx), (c)); \
		} while (0)
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния PTT
	#if FROMCAT_BIT_RTS != 0
		#define HARDWARE_CAT_GET_RTS() ((FROMCAT_TARGET_PIN_RTS & FROMCAT_BIT_RTS) == 0)
	#else /* FROMCAT_BIT_RTS != 0 */
		#define HARDWARE_CAT_GET_RTS() (0)
	#endif /* FROMCAT_BIT_RTS != 0 */
	// Вызывается из обработчика перерываний (или из user-mode программы) для получения состояния нажатия ключа
	#if FROMCAT_BIT_DTR != 0
		#define HARDWARE_CAT_GET_DTR() ((FROMCAT_TARGET_PIN_DTR & FROMCAT_BIT_DTR) == 0)
	#else /* FROMCAT_BIT_DTR != 0 */
		#define HARDWARE_CAT_GET_DTR() (0)
	#endif /* FROMCAT_BIT_DTR != 0 */

	/* заглушка для второго канала CAT */
	#define HARDWARE_CAT2_GET_RTS() (0)
	#define HARDWARE_CAT2_GET_DTR() (0)

	// вызывается из обработчика прерываний UART0
	// с принятым символом
	#define HARDWARE_UART0_ONRXCHAR(c) do { \
			cat2_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний UART0
	#define HARDWARE_UART0_ONOVERFLOW() do { \
			cat2_rxoverflow(); \
		} while (0)
	// вызывается из обработчика прерываний UART0
	// по готовности передатчика
	#define HARDWARE_UART0_ONTXCHAR(ctx) do { \
			cat2_sendchar(ctx); \
		} while (0)
	// вызывается из обработчика прерываний UART0
	// по окончании передачи (сдвиговый регистр передатчика пуст)
	#define HARDWARE_UART0_ONTXDONE(ctx) do { \
			cat2_txdone(ctx); \
		} while (0)

#endif /* WITHCAT && WITHUART0HW && WITHCAT_UART0 */

#if WITHCAT && WITHUART1HW && WITHCAT_UART1
	// CAT функции работают через USART1
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_INITIALIZE() do { \
			hardware_uart1_initialize(0, DEBUGSPEED, 8, 0, 0); \
		} while (0)
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_SET_SPEED(baudrate) do { \
			hardware_uart1_set_speed(baudrate); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_CAT_ENABLETX(v) do { \
			hardware_uart1_enabletx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_CAT_ENABLERX(v) do { \
			hardware_uart1_enablerx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_CAT_TX(ctx, c) do { \
			hardware_uart1_tx((ctx), (c)); \
		} while (0)
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния PTT
	#if FROMCAT_BIT_RTS != 0
		#define HARDWARE_CAT_GET_RTS() ((FROMCAT_TARGET_PIN_RTS & FROMCAT_BIT_RTS) == 0)
	#else /* FROMCAT_BIT_RTS != 0 */
		#define HARDWARE_CAT_GET_RTS() (0)
	#endif /* FROMCAT_BIT_RTS != 0 */
	// Вызывается из обработчика перерываний (или из user-mode программы) для получения состояния нажатия ключа
	#if FROMCAT_BIT_DTR != 0
		#define HARDWARE_CAT_GET_DTR() ((FROMCAT_TARGET_PIN_DTR & FROMCAT_BIT_DTR) == 0)
	#else /* FROMCAT_BIT_DTR != 0 */
		#define HARDWARE_CAT_GET_DTR() (0)
	#endif /* FROMCAT_BIT_DTR != 0 */

	/* заглушка для второго канала CAT */
	#define HARDWARE_CAT2_GET_RTS() (0)
	#define HARDWARE_CAT2_GET_DTR() (0)

	// вызывается из обработчика прерываний UART1
	// с принятым символом
	#define HARDWARE_UART1_ONRXCHAR(c) do { \
			cat2_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний UART1
	#define HARDWARE_UART1_ONOVERFLOW() do { \
			cat2_rxoverflow(); \
		} while (0)
	// вызывается из обработчика прерываний UART1
	// по готовности передатчика
	#define HARDWARE_UART1_ONTXCHAR(ctx) do { \
			cat2_sendchar(ctx); \
		} while (0)
	// вызывается из обработчика прерываний UART1
	// по окончании передачи (сдвиговый регистр передатчика пуст)
	#define HARDWARE_UART1_ONTXDONE(ctx) do { \
			cat2_txdone(ctx); \
		} while (0)

#endif /* WITHCAT && WITHUART1HW && WITHCAT_UART1 */

#if WITHCAT && WITHUART2HW && WITHCAT_UART2
	// CAT функции работают через USART2
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_INITIALIZE() do { \
			hardware_uart2_initialize(0, DEBUGSPEED, 8, 0, 0); \
		} while (0)
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_SET_SPEED(baudrate) do { \
			hardware_uart2_set_speed(baudrate); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_CAT_ENABLETX(v) do { \
			hardware_uart2_enabletx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_CAT_ENABLERX(v) do { \
			hardware_uart2_enablerx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_CAT_TX(ctx, c) do { \
			hardware_uart2_tx((ctx), (c)); \
		} while (0)
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния PTT
	#if FROMCAT_BIT_RTS != 0
		#define HARDWARE_CAT_GET_RTS() ((FROMCAT_TARGET_PIN_RTS & FROMCAT_BIT_RTS) == 0)
	#else /* FROMCAT_BIT_RTS != 0 */
		#define HARDWARE_CAT_GET_RTS() (0)
	#endif /* FROMCAT_BIT_RTS != 0 */
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния RTS
	#if FROMCAT_BIT_DTR != 0
		#define HARDWARE_CAT_GET_DTR() ((FROMCAT_TARGET_PIN_DTR & FROMCAT_BIT_DTR) == 0)
	#else /* FROMCAT_BIT_DTR != 0 */
		#define HARDWARE_CAT_GET_DTR() (0)
	#endif /* FROMCAT_BIT_DTR != 0 */

	// вызывается из обработчика прерываний UART2
	// с принятым символом
	#define HARDWARE_UART2_ONRXCHAR(c) do { \
			cat2_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний UART2
	#define HARDWARE_UART2_ONOVERFLOW() do { \
			cat2_rxoverflow(); \
		} while (0)
	// вызывается из обработчика прерываний UART2
	// по готовности передатчика
	#define HARDWARE_UART2_ONTXCHAR(ctx) do { \
			cat2_sendchar(ctx); \
		} while (0)
	// вызывается из обработчика прерываний UART1
	// по окончании передачи (сдвиговый регистр передатчика пуст)
	#define HARDWARE_UART2_ONTXDONE(ctx) do { \
			cat2_txdone(ctx); \
		} while (0)

#endif /* WITHCAT && WITHUART2HW && WITHCAT_UART2 */

#if WITHCAT && WITHUART4HW && WITHCAT_UART4
	// CAT функции работают через UART4
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_INITIALIZE() do { \
			hardware_uart4_initialize(0, DEBUGSPEED, 8, 0, 0); \
		} while (0)
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_SET_SPEED(baudrate) do { \
			hardware_uart4_set_speed(baudrate); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_CAT_ENABLETX(v) do { \
			hardware_uart4_enabletx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_CAT_ENABLERX(v) do { \
			hardware_uart4_enablerx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_CAT_TX(ctx, c) do { \
			hardware_uart4_tx((ctx), (c)); \
		} while (0)
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния PTT
	#if FROMCAT_BIT_RTS != 0
		#define HARDWARE_CAT_GET_RTS() ((FROMCAT_TARGET_PIN_RTS & FROMCAT_BIT_RTS) == 0)
	#else /* FROMCAT_BIT_RTS != 0 */
		#define HARDWARE_CAT_GET_RTS() (0)
	#endif /* FROMCAT_BIT_RTS != 0 */
	// Вызывается из обработчика перерываний (или из user-mode программы) для получения состояния нажатия ключа
	#if FROMCAT_BIT_DTR != 0
		#define HARDWARE_CAT_GET_DTR() ((FROMCAT_TARGET_PIN_DTR & FROMCAT_BIT_DTR) == 0)
	#else /* FROMCAT_BIT_DTR != 0 */
		#define HARDWARE_CAT_GET_DTR() (0)
	#endif /* FROMCAT_BIT_DTR != 0 */

	/* заглушка для второго канала CAT */
	#define HARDWARE_CAT2_GET_RTS() (0)
	#define HARDWARE_CAT2_GET_DTR() (0)

	// вызывается из обработчика прерываний UART4
	// с принятым символом
	#define HARDWARE_UART4_ONRXCHAR(c) do { \
			cat2_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний UART4
	#define HARDWARE_UART4_ONOVERFLOW() do { \
			cat2_rxoverflow(); \
		} while (0)
	// вызывается из обработчика прерываний UART4
	// по готовности передатчика
	#define HARDWARE_UART4_ONTXCHAR(ctx) do { \
			cat2_sendchar(ctx); \
		} while (0)
	// вызывается из обработчика прерываний UART4
	// по окончании передачи (сдвиговый регистр передатчика пуст)
	#define HARDWARE_UART4_ONTXDONE(ctx) do { \
			cat2_txdone(ctx); \
		} while (0)

#endif /* WITHCAT && WITHUART4HW && WITHCAT_UART4 */

#if WITHCAT && WITHUART5HW && WITHCAT_UART5
	// CAT функции работают через UART5
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_INITIALIZE() do { \
			hardware_uart5_initialize(0, DEBUGSPEED, 8, 0, 0); \
		} while (0)
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_SET_SPEED(baudrate) do { \
			hardware_uart5_set_speed(baudrate); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_CAT_ENABLETX(v) do { \
			hardware_uart5_enabletx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_CAT_ENABLERX(v) do { \
			hardware_uart5_enablerx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_CAT_TX(ctx, c) do { \
			hardware_uart5_tx((ctx), (c)); \
		} while (0)
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния PTT
	#if FROMCAT_BIT_RTS != 0
		#define HARDWARE_CAT_GET_RTS() ((FROMCAT_TARGET_PIN_RTS & FROMCAT_BIT_RTS) == 0)
	#else /* FROMCAT_BIT_RTS != 0 */
		#define HARDWARE_CAT_GET_RTS() (0)
	#endif /* FROMCAT_BIT_RTS != 0 */
	// Вызывается из обработчика перерываний (или из user-mode программы) для получения состояния нажатия ключа
	#if FROMCAT_BIT_DTR != 0
		#define HARDWARE_CAT_GET_DTR() ((FROMCAT_TARGET_PIN_DTR & FROMCAT_BIT_DTR) == 0)
	#else /* FROMCAT_BIT_DTR != 0 */
		#define HARDWARE_CAT_GET_DTR() (0)
	#endif /* FROMCAT_BIT_DTR != 0 */

	/* заглушка для второго канала CAT */
	#define HARDWARE_CAT2_GET_RTS() (0)
	#define HARDWARE_CAT2_GET_DTR() (0)

	// вызывается из обработчика прерываний UART5
	// с принятым символом
	#define HARDWARE_UART5_ONRXCHAR(c) do { \
			cat2_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний UART5
	#define HARDWARE_UART5_ONOVERFLOW() do { \
			cat2_rxoverflow(); \
		} while (0)
	// вызывается из обработчика прерываний UART5
	// по готовности передатчика
	#define HARDWARE_UART5_ONTXCHAR(ctx) do { \
			cat2_sendchar(ctx); \
		} while (0)
	// вызывается из обработчика прерываний UART5
	// по окончании передачи (сдвиговый регистр передатчика пуст)
	#define HARDWARE_UART5_ONTXDONE(ctx) do { \
			cat2_txdone(ctx); \
		} while (0)

#endif /* WITHCAT && WITHUART5HW && WITHCAT_UART5 */

#if WITHLWIP && WITHUART7HW && WITHSLIP_UART7
	// CAT7 функции работают через UART7
	// Вызывается из user-mode программы
	#define HARDWARE_SLIP_INITIALIZE() do { \
			hardware_uart7_initialize(0, DEBUGSPEED, 8, 0, 0); \
		} while (0)
	// Вызывается из user-mode программы
	#define HARDWARE_SLIP_SET_SPEED(baudrate) do { \
			hardware_uart7_set_speed(baudrate); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_SLIP_ENABLETX(v) do { \
			hardware_uart7_enabletx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_SLIP_ENABLERX(v) do { \
			hardware_uart7_enablerx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_SLIP_TX(ctx, c) do { \
			hardware_uart7_tx((ctx), (c)); \
		} while (0)
	// вызывается из обработчика прерываний UART7
	// с принятым символом
	#define HARDWARE_UART7_ONRXCHAR(c) do { \
			slip_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний UART7
	#define HARDWARE_UART7_ONOVERFLOW() do { \
			slip_rxoverflow(); \
		} while (0)
	// вызывается из обработчика прерываний UART7
	// по готовности передатчика
	#define HARDWARE_UART7_ONTXCHAR(ctx) do { \
			slip_sendchar(ctx); \
		} while (0)
	// вызывается из обработчика прерываний UART1
	// по окончании передачи (сдвиговый регистр передатчика пуст)
	#define HARDWARE_UART7_ONTXDONE(ctx) do { \
			slip_txdone(ctx); \
		} while (0)

#endif /* WITHLWIP && WITHUART7HW && WITHSLIP_UART7 */

#if WITHLWIP && WITHUART5HW && WITHSLIP_UART5
	// CAT7 функции работают через UART5
	// Вызывается из user-mode программы
	#define HARDWARE_SLIP_INITIALIZE() do { \
			hardware_uart5_initialize(0, DEBUGSPEED, 8, 0, 0); \
		} while (0)
	// Вызывается из user-mode программы
	#define HARDWARE_SLIP_SET_SPEED(baudrate) do { \
			hardware_uart5_set_speed(baudrate); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_SLIP_ENABLETX(v) do { \
			hardware_uart5_enabletx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_SLIP_ENABLERX(v) do { \
			hardware_uart5_enablerx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_SLIP_TX(ctx, c) do { \
			hardware_uart5_tx((ctx), (c)); \
		} while (0)
	// вызывается из обработчика прерываний UART7
	// с принятым символом
	#define HARDWARE_UART5_ONRXCHAR(c) do { \
			slip_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний UART7
	#define HARDWARE_UART5_ONOVERFLOW() do { \
			slip_rxoverflow(); \
		} while (0)
	// вызывается из обработчика прерываний UART7
	// по готовности передатчика
	#define HARDWARE_UART5_ONTXCHAR(ctx) do { \
			slip_sendchar(ctx); \
		} while (0)
	// вызывается из обработчика прерываний UART1
	// по окончании передачи (сдвиговый регистр передатчика пуст)
	#define HARDWARE_UART5_ONTXDONE(ctx) do { \
			slip_txdone(ctx); \
		} while (0)

#endif /* WITHLWIP && WITHUART5HW && WITHSLIP_UART5 */

#if WITHCAT && WITHUSBCDCACM && WITHCAT_CDC
	// CAT функции работают через виртуальный USB последовательный порт
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_INITIALIZE() do { \
		} while (0)
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_SET_SPEED(baudrate) do { \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_CAT_ENABLETX(v) do { \
			usbd_cdc_enabletx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_CAT_ENABLERX(v) do { \
			usbd_cdc_enablerx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_CAT_TX(ctx, c) do { \
			usbd_cdc_tx((ctx), (c)); \
		} while (0)
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния RTS
	#define HARDWARE_CAT_GET_RTS() (usbd_cdc1_getrts())
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния DTR
	#define HARDWARE_CAT_GET_DTR() (usbd_cdc1_getdtr())
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния RTS
	#define HARDWARE_CAT2_GET_RTS() (usbd_cdc2_getrts())
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния DTR
	#define HARDWARE_CAT2_GET_DTR() (usbd_cdc2_getdtr())

	// вызывается из обработчика прерываний CDC
	// с принятым символом
	#define HARDWARE_CDC_ONRXCHAR(offset, c) do { \
			cat2_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// произошёл разрыв связи при работе по USB CDC
	#define HARDWARE_CDC_ONDISCONNECT() do { \
			cat2_disconnect(); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// по готовности передатчика
	#define HARDWARE_CDC_ONTXCHAR(offset, ctx) do { \
			cat2_sendchar(ctx); \
		} while (0)

#endif /* WITHCAT && WITHUSBCDCACM && WITHCAT_CDC */

#if WITHCAT && WITHUSEUSBBT && WITHCAT_BT
	// CAT функции работают через виртуальный BT последовательный порт SPP
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_INITIALIZE() do { \
		} while (0)
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_SET_SPEED(baudrate) do { \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_CAT_ENABLETX(v) do { \
			btspp_enabletx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_CAT_ENABLERX(v) do { \
			btspp_enablerx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_CAT_TX(ctx, c) do { \
			btspp_tx((ctx), (c)); \
		} while (0)
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния RTS
	#define HARDWARE_CAT_GET_RTS() (bt_spp_getrts())
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния DTR
	#define HARDWARE_CAT_GET_DTR() (bt_spp_getdtr())
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния RTS
	#define HARDWARE_CAT2_GET_RTS() 0//(bt_spp2_getrts())
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния DTR
	#define HARDWARE_CAT2_GET_DTR() 0//(bt_spp2_getdtr())

	// вызывается из обработчика прерываний CDC
	// с принятым символом
	#define HARDWARE_BTSPP_ONRXCHAR(offset, c) do { \
			cat2_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// произошёл разрыв связи при работе по USB CDC
	#define HARDWARE_BTSPP_ONDISCONNECT() do { \
			cat2_disconnect(); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// по готовности передатчика
	#define HARDWARE_BTSPP_ONTXCHAR(offset, ctx) do { \
			cat2_sendchar(ctx); \
		} while (0)

#endif /* WITHCAT && WITHUSEUSBBT && WITHCAT_BT */


#if WITHCAT && WITHLWIP && WITHCAT_LWIP
	// CAT функции работают через виртуальный BT последовательный порт SPP
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_INITIALIZE() do { \
		} while (0)
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_SET_SPEED(baudrate) do { \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов HARDWARE_CAT_TX
	#define HARDWARE_CAT_ENABLETX(v) do { \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов HARDWARE_LWIP_ONRXCHAR
	#define HARDWARE_CAT_ENABLERX(v) do { \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_CAT_TX(ctx, c) do { \
		} while (0)
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния RTS
	#define HARDWARE_CAT_GET_RTS() (0)
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния DTR
	#define HARDWARE_CAT_GET_DTR() (0)
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния RTS
	#define HARDWARE_CAT2_GET_RTS() 0//(bt_spp2_getrts())
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния DTR
	#define HARDWARE_CAT2_GET_DTR() 0//(bt_spp2_getdtr())

	// вызывается из обработчика прерываний CDC
	// с принятым символом
	#define HARDWARE_LWIP_ONRXCHAR(offset, c) do { \
			cat2_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// произошёл разрыв связи при работе по USB CDC
	#define HARDWARE_LWIP_ONDISCONNECT() do { \
			cat2_disconnect(); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// по готовности передатчика
	#define HARDWARE_LWIP_ONTXCHAR(offset, ctx) do { \
			cat2_sendchar(ctx); \
		} while (0)

#endif /* WITHCAT && WITHUSEUSBBT && WITHCAT_BT */

#if WITHLWIP && WITHUSBCDCACM && WITHSLIP_CDC
	// CAT функции работают через виртуальный USB последовательный порт
	// Вызывается из user-mode программы
	#define HARDWARE_SLIP_INITIALIZE() do { \
		} while (0)
	// Вызывается из user-mode программы
	#define HARDWARE_SLIP_SET_SPEED(baudrate) do { \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_SLIP_ENABLETX(v) do { \
			usbd_cdc_enabletx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_SLIP_ENABLERX(v) do { \
			usbd_cdc_enablerx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_SLIP_TX(ctx, c) do { \
			usbd_cdc_tx((ctx), (c)); \
		} while (0)

	// вызывается из обработчика прерываний CDC
	// с принятым символом
	#define HARDWARE_CDC_ONRXCHAR(c) do { \
			slip_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	#define HARDWARE_CDC_ONOVERFLOW() do { \
			slip_rxoverflow(); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// произошёл разрыв связи при работе по USB CDC
	#define HARDWARE_CDC_ONDISCONNECT() do { \
			slip_disconnect(); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// по готовности передатчика
	#define HARDWARE_CDC_ONTXCHAR(offset, ctx) do { \
			slip_sendchar(ctx); \
		} while (0)

#endif /* WITHLWIP && WITHUSBCDCACM && WITHSLIP_CDC */


#if WITHMODEM && WITHUSBCDCACM && WITHMODEM_CDC
	// Модемные функции работают через виртуальный USB последовательный порт
	// Вызывается из user-mode программы
	#define HARDWARE_MODEM_INITIALIZE() do { \
		} while (0)
	// Вызывается из user-mode программы
	#define HARDWARE_MODEM_SET_SPEED(baudrate) do { \
		} while (0)
	// вызывается из state machie протокола MODEM или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_MODEM_ENABLETX(v) do { \
			usbd_cdc_enabletx(v); \
		} while (0)
	// вызывается из state machie протокола MODEM или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_MODEM_ENABLERX(v) do { \
			usbd_cdc_enablerx(v); \
		} while (0)
	// вызывается из state machie протокола MODEM или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_MODEM_TX(ctx, c) do { \
			usbd_cdc_tx((ctx), (c)); \
		} while (0)

	// вызывается из обработчика прерываний CDC
	// с принятым символом
	#define HARDWARE_CDC_ONRXCHAR(offset, c) do { \
			modem_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// произошёл разрыв связи при работе по USB CDC
	#define HARDWARE_CDC_ONDISCONNECT() do { \
			modem_disconnect(); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// по готовности передатчика
	#define HARDWARE_CDC_ONTXCHAR(offset, ctx) do { \
			modem_sendchar(ctx); \
		} while (0)

#endif /* WITHMODEM && WITHUSBCDCACM && WITHCAT_CDC */

#if WITHDEBUG && WITHUSBCDCACM && WITHDEBUG_CDC
	// Модемные функции работают через виртуальный USB последовательный порт
	// Вызывается из user-mode программы при запрещённых прерываниях.
	#define HARDWARE_DEBUG_INITIALIZE() do { \
		debugusb_initialize(); \
		usbd_cdc_enablerx(1); \
		} while (0)
	// Вызывается из user-mode программы
	#define HARDWARE_DEBUG_SET_SPEED(baudrate) do { \
		} while (0)
	// вызывается из state machie протокола MODEM или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_DEBUG_ENABLETX(v) do { \
			usbd_cdc_enabletx(v); \
		} while (0)
	// вызывается из state machie протокола MODEM или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_DEBUG_ENABLERX(v) do { \
			usbd_cdc_enablerx(v); \
		} while (0)
	// вызывается из state machie протокола MODEM или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_DEBUG_TX(ctx, c) do { \
			usbd_cdc_tx((ctx), (c)); \
		} while (0)
	#define HARDWARE_DEBUG_PUTCHAR(c) \
		(debugusb_putchar(c))
	#define HARDWARE_DEBUG_GETCHAR(pc) \
		(debugusb_getchar(pc))

	// вызывается из обработчика прерываний CDC
	// с принятым символом
	#define HARDWARE_CDC_ONRXCHAR(offset, c) do { \
			debugusb_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// произошёл разрыв связи при работе по USB CDC
	#define HARDWARE_CDC_ONDISCONNECT() do { \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// по готовности передатчика
	#define HARDWARE_CDC_ONTXCHAR(offset, ctx) do { \
			debugusb_sendchar(ctx); \
		} while (0)

#endif /* WITHDEBUG && WITHUSBCDCACM && WITHDEBUG_CDC */

#if WITHUART1HW
	// Заглушки, если есть последовательный порт #1, но нигде не используется.
	#if ! defined (HARDWARE_UART1_ONRXCHAR)
		// вызывается из обработчика прерываний CDC
		// с принятым символом
		#define HARDWARE_UART1_ONRXCHAR(c) do { \
				(void) (c); \
				hardware_uart1_enablerx(1); \
			} while (0)
	#endif /* ! defined (HARDWARE_UART1_ONRXCHAR) */

	#if ! defined (HARDWARE_UART1_ONOVERFLOW)
		// вызывается из обработчика прерываний UART1
		#define HARDWARE_UART1_ONOVERFLOW() do { \
				hardware_uart1_enablerx(1); \
			} while (0)
	#endif /* ! defined (HARDWARE_UART1_ONOVERFLOW) */

	#if ! defined (HARDWARE_UART1_ONTXCHAR)
		// вызывается из обработчика прерываний UART1
		// по готовности передатчика
		#define HARDWARE_UART1_ONTXCHAR(ctx) do { \
				hardware_uart1_enabletx(0); \
			} while (0)
	#endif /* ! defined (HARDWARE_UART1_ONTXCHAR) */

#endif /* WITHUART1HW */

#if WITHUART2HW
	// Заглушки, если есть последовательный порт #2, но нигде не используется.
	#if ! defined (HARDWARE_UART2_ONRXCHAR)
		// вызывается из обработчика прерываний CDC
		// с принятым символом
		#define HARDWARE_UART2_ONRXCHAR(c) do { \
				(void) (c); \
				hardware_uart2_enablerx(1); \
			} while (0)
	#endif /* ! defined (HARDWARE_UART2_ONRXCHAR) */

	#if ! defined (HARDWARE_UART2_ONOVERFLOW)
		// вызывается из обработчика прерываний UART2
		#define HARDWARE_UART2_ONOVERFLOW() do { \
				hardware_uart2_enablerx(1); \
			} while (0)
	#endif /* ! defined (HARDWARE_UART2_ONOVERFLOW) */

	#if ! defined (HARDWARE_UART2_ONTXCHAR)
		// вызывается из обработчика прерываний UART2
		// по готовности передатчика
		#define HARDWARE_UART2_ONTXCHAR(ctx) do { \
				hardware_uart2_enabletx(0); \
			} while (0)
	#endif /* ! defined (HARDWARE_UART2_ONTXCHAR) */

#endif /* WITHUART2HW */

#if WITHUART3HW
	// Заглушки, если есть последовательный порт #3, но нигде не используется.
	#if ! defined (HARDWARE_UART3_ONRXCHAR)
		// вызывается из обработчика прерываний CDC
		// с принятым символом
		#define HARDWARE_UART3_ONRXCHAR(c) do { \
				(void) (c); \
				hardware_uart3_enablerx(1); \
			} while (0)
	#endif /* ! defined (HARDWARE_UART3_ONRXCHAR) */

	#if ! defined (HARDWARE_UART3_ONOVERFLOW)
		// вызывается из обработчика прерываний UART3
		#define HARDWARE_UART3_ONOVERFLOW() do { \
				hardware_uart3_enablerx(1); \
			} while (0)
	#endif /* ! defined (HARDWARE_UART3_ONOVERFLOW) */

	#if ! defined (HARDWARE_UART3_ONTXCHAR)
		// вызывается из обработчика прерываний UART3
		// по готовности передатчика
		#define HARDWARE_UART3_ONTXCHAR(ctx) do { \
				hardware_uart3_enabletx(0); \
			} while (0)
	#endif /* ! defined (HARDWARE_UART3_ONTXCHAR) */

#endif /* WITHUART3HW */

#if WITHUART4HW
	// Заглушки, если есть последовательный порт #4, но нигде не используется.
	#if ! defined (HARDWARE_UART4_ONRXCHAR)
		// вызывается из обработчика прерываний CDC
		// с принятым символом
		#define HARDWARE_UART4_ONRXCHAR(c) do { \
				(void) (c); \
				hardware_uart4_enablerx(1); \
			} while (0)
	#endif /* ! defined (HARDWARE_UART4_ONRXCHAR) */

	#if ! defined (HARDWARE_UART4_ONOVERFLOW)
		// вызывается из обработчика прерываний UART4
		#define HARDWARE_UART4_ONOVERFLOW() do { \
				hardware_uart4_enablerx(1); \
			} while (0)
	#endif /* ! defined (HARDWARE_UART4_ONOVERFLOW) */

	#if ! defined (HARDWARE_UART4_ONTXCHAR)
		// вызывается из обработчика прерываний UART4
		// по готовности передатчика
		#define HARDWARE_UART4_ONTXCHAR(ctx) do { \
				hardware_uart4_enabletx(0); \
			} while (0)
	#endif /* ! defined (HARDWARE_UART4_ONTXCHAR) */

#endif /* WITHUART4HW */

#if WITHUART5HW
	// Заглушки, если есть последовательный порт #5, но нигде не используется.
	#if ! defined (HARDWARE_UART5_ONRXCHAR)
		// вызывается из обработчика прерываний CDC
		// с принятым символом
		#define HARDWARE_UART5_ONRXCHAR(c) do { \
				(void) (c); \
				hardware_uart5_enablerx(1); \
			} while (0)
	#endif /* ! defined (HARDWARE_UART5_ONRXCHAR) */

	#if ! defined (HARDWARE_UART5_ONOVERFLOW)
		// вызывается из обработчика прерываний UART5
		#define HARDWARE_UART5_ONOVERFLOW() do { \
				hardware_uart5_enablerx(1); \
			} while (0)
	#endif /* ! defined (HARDWARE_UART5_ONOVERFLOW) */

	#if ! defined (HARDWARE_UART5_ONTXCHAR)
		// вызывается из обработчика прерываний UART5
		// по готовности передатчика
		#define HARDWARE_UART5_ONTXCHAR(ctx) do { \
				hardware_uart5_enabletx(0); \
			} while (0)
	#endif /* ! defined (HARDWARE_UART5_ONTXCHAR) */

#endif /* WITHUART5HW */

#if WITHUSBCDCACM
	// Заглушки, если есть виртуальный последовательный порт, но нигде не используется.
	#if ! defined (HARDWARE_CDC_ONRXCHAR)
		// вызывается из обработчика прерываний CDC
		// с принятым символом
		#define HARDWARE_CDC_ONRXCHAR(offset, c) do { \
				(void) (c); \
				usbd_cdc_enablerx(1); \
			} while (0)
	#endif /* ! defined (HARDWARE_CDC_ONRXCHAR) */
	#if ! defined (HARDWARE_CDC_ONDISCONNECT)
		// вызывается из обработчика прерываний CDC
		// произошёл разрыв связи при работе по USB CDC
		#define HARDWARE_CDC_ONDISCONNECT() do { \
			} while (0)
	#endif /* ! defined (HARDWARE_CDC_ONDISCONNECT) */
	#if ! defined (HARDWARE_CDC_ONTXCHAR)
		// вызывается из обработчика прерываний CDC
		// по готовности передатчика
		#define HARDWARE_CDC_ONTXCHAR(offset, ctx) do { \
				usbd_cdc_enabletx(0); \
			} while (0)
	#endif /* ! defined (HARDWARE_CDC_ONTXCHAR) */

#endif /* WITHUSBCDCACM */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FORMATS_H_INCLUDED */
