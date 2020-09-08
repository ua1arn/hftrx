/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>	// format specifiers for printing

#include "hardware.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

uint_fast8_t local_snprintf_P( char * __restrict buffer, uint_fast8_t count, const FLASHMEM char * __restrict format, ... );
// Отладочная печать
void debug_printf_P(const FLASHMEM char * __restrict format, ... );

char * safestrcpy(char * dst, size_t blen, const char * src);

void printhex(unsigned long voffs, const unsigned char * buff, unsigned length);


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

	#define PRINTF	debug_printf_P
	#define ASSERT(v) do { if ((v) == 0) { \
		PRINTF(PSTR("%s(%d): Assert '%s'\n"), __FILE__, __LINE__, (# v)); \
		for (;;) ; \
		} } while (0)

	#define VERIFY(v) do { if ((v) == 0) { \
		PRINTF(PSTR("%s(%d): Verify '%s'\n"), __FILE__, __LINE__, (# v)); \
		for (;;) ; \
		} } while (0)

	#define TRACE0(f)		do { PRINTF( PSTR(f)); } while (0)
	#define TRACE1(f,a1)		do { PRINTF( PSTR(f),(a1)); } while (0)
	#define TRACE2(f,a1,a2)		do { PRINTF( PSTR(f),(a1),(a2)); } while (0)
	#define TRACE3(f,a1,a2,a3)	do { PRINTF( PSTR(f),(a1),(a2),(a3)); } while (0)
	#define TRACE4(f,a1,a2,a3,a4)	do { PRINTF( PSTR(f),(a1),(a2),(a3),(a4)); } while (0)
	#define TRACE5(f,a1,a2,a3,a4,a5) do { PRINTF( PSTR(f),(a1),(a2),(a3),(a4),(a5)); } while (0)

#else /* WITHDEBUG */

	#define PRINTF(...)	do {} while (0)
	#define ASSERT(v) ((void) (0))
	#define VERIFY(v) ((void) (v))

	#define TRACE0(f)			do {} while (0)
	#define TRACE1(f,a1)			do {} while (0)
	#define TRACE2(f,a1,a2)			do {} while (0)
	#define TRACE3(f,a1,a2,a3)		do {} while (0)
	#define TRACE4(f,a1,a2,a3,a4)		do {} while (0)
	#define TRACE5(f,a1,a2,a3,a4,a5)		do {} while (0)

#endif /* WITHDEBUG */

// spool-based functions for debug
int dbg_puts_impl_P(const FLASHMEM char * s);
int dbg_puts_impl(const char * s);
int dbg_putchar(int c);
int dbg_getchar(char * r);



#if WITHDEBUG && WITHUART1HW && WITHDEBUG_USART1
	// Отладочные функции работают через USART1
	// Вызывается из user-mode программы при запрещённых прерываниях.
	#define HARDWARE_DEBUG_INITIALIZE() do { \
			hardware_uart1_initialize(1); \
		} while (0)
	#define HARDWARE_DEBUG_SET_SPEED(baudrate) do { \
			hardware_uart1_set_speed(baudrate); \
		} while (0)
	#define HARDWARE_DEBUG_PUTCHAR(c) \
		(hardware_usart1_putchar(c))
	#define HARDWARE_DEBUG_GETCHAR(pc) \
		(hardware_usart1_getchar(pc))

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
			hardware_uart1_enabletx(0); \
		} while (0)

#endif /* WITHDEBUG && WITHUART1HW && WITHDEBUG_USART1 */

// Для serial_irq_loopback_test
#if 0 && WITHDEBUG && WITHUART1HW && WITHDEBUG_USART1
	// Отладочные функции работают через USART1
	// Вызывается из user-mode программы при запрещённых прерываниях.
	#define HARDWARE_DEBUGSIRQ_INITIALIZE() do { \
			hardware_uart1_initialize(0); \
		} while (0)
	#define HARDWARE_DEBUGSIRQ_SET_SPEED(baudrate) do { \
			hardware_uart1_set_speed(baudrate); \
		} while (0)
	#define HARDWARE_DEBUGSIRQ_PUTCHAR(c) \
		(hardware_usart1_putchar(c))
	#define HARDWARE_DEBUGSIRQ_GETCHAR(pc) \
		(hardware_usart1_getchar(pc))
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_DEBUGSIRQ_TX(ctx, c) do { \
			hardware_uart1_tx((ctx), (c)); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_DEBUGSIRQ_ENABLETX(v) do { \
			hardware_uart1_enabletx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_DEBUGSIRQ_ENABLERX(v) do { \
			hardware_uart1_enablerx(v); \
		} while (0)

	// вызывается из обработчика прерываний UART1
	// с принятым символом
	#define HARDWARE_UART1_ONRXCHAR(c) do { \
			cat3_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний UART1
	#define HARDWARE_UART1_ONOVERFLOW() do { \
			cat3_rxoverflow(); \
		} while (0)
	// вызывается из обработчика прерываний UART1
	// по готовности передатчика
	#define HARDWARE_UART1_ONTXCHAR(ctx) do { \
			cat3_sendchar(ctx); \
		} while (0)

#endif /* WITHDEBUG && WITHUART1HW && WITHDEBUG_USART1 */

#if WITHDEBUG && WITHUART2HW && WITHDEBUG_USART2
	// Отладочные функции работают через USART2
	// Вызывается из user-mode программы при запрещённых прерываниях.
	#define HARDWARE_DEBUG_INITIALIZE() do { \
			hardware_uart2_initialize(1); \
		} while (0)
	#define HARDWARE_DEBUG_SET_SPEED(baudrate) do { \
			hardware_uart2_set_speed(baudrate); \
		} while (0)
	#define HARDWARE_DEBUG_PUTCHAR(c) \
		(hardware_usart2_putchar(c))
	#define HARDWARE_DEBUG_GETCHAR(pc) \
		(hardware_usart2_getchar(pc))

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
			hardware_uart2_enabletx(0); \
		} while (0)

#endif /* WITHDEBUG && WITHUART2HW && WITHDEBUG_USART2 */

#if WITHMODEM && WITHUART1HW && WITHMODEM_USART1
	// Модемные функции работают через USART1
	// Вызывается из user-mode программы
	#define HARDWARE_MODEM_INITIALIZE() do { \
			hardware_uart1_initialize(0); \
		} while (0)
	// Вызывается из user-mode программы
	#define HARDWARE_MODEM_SET_SPEED(baudrate) do { \
			hardware_uart1_set_speed(baudrate); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_MODEM_ENABLETX(v) do { \
			hardware_uart1_enabletx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_MODEM_ENABLERX(v) do { \
			hardware_uart1_enablerx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_MODEM_TX(ctx, c) do { \
			hardware_uart1_tx((ctx), (c)); \
		} while (0)

	// вызывается из обработчика прерываний UART1
	// с принятым символом
	#define HARDWARE_UART1_ONRXCHAR(c) do { \
			modem_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний UART1
	#define HARDWARE_UART1_ONOVERFLOW() do { \
			modem_rxoverflow(); \
		} while (0)
	// вызывается из обработчика прерываний UART1
	// по готовности передатчика
	#define HARDWARE_UART1_ONTXCHAR(ctx) do { \
			modem_sendchar(ctx); \
		} while (0)

#endif /* WITHMODEM && WITHUART1HW && WITHMODEM_USART1 */

#if WITHMODEM && WITHUART2HW && WITHMODEM_USART2
	// Модемные функции работают через USART2
	// Вызывается из user-mode программы
	#define HARDWARE_MODEM_INITIALIZE() do { \
			hardware_uart2_initialize(0); \
		} while (0)
	// Вызывается из user-mode программы
	#define HARDWARE_MODEM_SET_SPEED(baudrate) do { \
			hardware_uart2_set_speed(baudrate); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_MODEM_ENABLETX(v) do { \
			hardware_uart2_enabletx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_MODEM_ENABLERX(v) do { \
			hardware_uart2_enablerx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_MODEM_TX(ctx, c) do { \
			hardware_uart2_tx((ctx), (c)); \
		} while (0)

	// вызывается из обработчика прерываний UART2
	// с принятым символом
	#define HARDWARE_UART2_ONRXCHAR(c) do { \
			modem_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний UART2
	#define HARDWARE_UART2_ONOVERFLOW() do { \
			modem_rxoverflow(); \
		} while (0)
	// вызывается из обработчика прерываний UART2
	// по готовности передатчика
	#define HARDWARE_UART2_ONTXCHAR(ctx) do { \
			modem_sendchar(ctx); \
		} while (0)

#endif /* WITHMODEM && WITHUART2HW && WITHMODEM_USART2 */

#if WITHCAT && WITHUART1HW && WITHCAT_USART1
	// CAT функции работают через USART1
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_INITIALIZE() do { \
			hardware_uart1_initialize(0); \
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

#endif /* WITHCAT && WITHUART1HW && WITHCAT_USART1 */

#if WITHCAT && WITHUART2HW && WITHCAT_USART2
	// CAT функции работают через USART2
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_INITIALIZE() do { \
			hardware_uart2_initialize(0); \
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

#endif /* WITHCAT && WITHUART2HW && WITHCAT_USART2 */

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
	#define HARDWARE_CDC_ONRXCHAR(c) do { \
			cat2_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	#define HARDWARE_CDC_ONOVERFLOW() do { \
			cat2_rxoverflow(); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// произошёл разрыв связи при работе по USB CDC
	#define HARDWARE_CDC_ONDISCONNECT() do { \
			cat2_disconnect(); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// по готовности передатчика
	#define HARDWARE_CDC_ONTXCHAR(ctx) do { \
			cat2_sendchar(ctx); \
		} while (0)

#endif /* WITHCAT && WITHUSBCDCACM && WITHCAT_CDC */


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
	#define HARDWARE_CDC_ONRXCHAR(c) do { \
			modem_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	#define HARDWARE_CDC_ONOVERFLOW() do { \
			modem_rxoverflow(); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// произошёл разрыв связи при работе по USB CDC
	#define HARDWARE_CDC_ONDISCONNECT() do { \
			modem_disconnect(); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// по готовности передатчика
	#define HARDWARE_CDC_ONTXCHAR(ctx) do { \
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
	#define HARDWARE_CDC_ONRXCHAR(c) do { \
			debugusb_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	#define HARDWARE_CDC_ONOVERFLOW() do { \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// произошёл разрыв связи при работе по USB CDC
	#define HARDWARE_CDC_ONDISCONNECT() do { \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// по готовности передатчика
	#define HARDWARE_CDC_ONTXCHAR(ctx) do { \
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

#if WITHNMEA && WITHUART1HW && WITHNMEA_USART1
	// Модемные функции работают через USART1
	// Вызывается из user-mode программы
	#define HARDWARE_NMEA_INITIALIZE() do { \
			hardware_uart1_initialize(0); \
		} while (0)
	// Вызывается из user-mode программы
	#define HARDWARE_NMEA_SET_SPEED(baudrate) do { \
			hardware_uart1_set_speed(baudrate); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_NMEA_ENABLETX(v) do { \
			hardware_uart1_enabletx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_NMEA_ENABLERX(v) do { \
			hardware_uart1_enablerx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_NMEA_TX(ctx, c) do { \
			hardware_uart1_tx((ctx), (c)); \
		} while (0)

	// вызывается из обработчика прерываний UART2
	// с принятым символом
	#define HARDWARE_UART1_ONRXCHAR(c) do { \
			nmea_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний UART2
	#define HARDWARE_UART1_ONOVERFLOW() do { \
			nmea_rxoverflow(); \
		} while (0)
	// вызывается из обработчика прерываний UART2
	// по готовности передатчика
	#define HARDWARE_UART1_ONTXCHAR(ctx) do { \
			nmea_sendchar(ctx); \
		} while (0)

#endif /* WITHNMEA && WITHUART1HW && WITHMODEM_USART1 */

#if WITHNMEA && WITHUART2HW && WITHNMEA_USART2
	// Модемные функции работают через USART2
	// Вызывается из user-mode программы
	#define HARDWARE_NMEA_INITIALIZE() do { \
			hardware_uart2_initialize(0); \
		} while (0)
	// Вызывается из user-mode программы
	#define HARDWARE_NMEA_SET_SPEED(baudrate) do { \
			hardware_uart2_set_speed(baudrate); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_NMEA_ENABLETX(v) do { \
			hardware_uart2_enabletx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_NMEA_ENABLERX(v) do { \
			hardware_uart2_enablerx(v); \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_NMEA_TX(ctx, c) do { \
			hardware_uart2_tx((ctx), (c)); \
		} while (0)

	// вызывается из обработчика прерываний UART2
	// с принятым символом
	#define HARDWARE_UART2_ONRXCHAR(c) do { \
			nmea_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний UART2
	#define HARDWARE_UART2_ONOVERFLOW() do { \
			nmea_rxoverflow(); \
		} while (0)
	// вызывается из обработчика прерываний UART2
	// по готовности передатчика
	#define HARDWARE_UART2_ONTXCHAR(ctx) do { \
			nmea_sendchar(ctx); \
		} while (0)

#endif /* WITHNMEA && WITHUART2HW && WITHMODEM_USART2 */

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
		// вызывается из обработчика прерываний UART1
		#define HARDWARE_UART2_ONOVERFLOW() do { \
				hardware_uart2_enablerx(1); \
			} while (0)
	#endif /* ! defined (HARDWARE_UART2_ONOVERFLOW) */

	#if ! defined (HARDWARE_UART2_ONTXCHAR)
		// вызывается из обработчика прерываний UART1
		// по готовности передатчика
		#define HARDWARE_UART2_ONTXCHAR(ctx) do { \
				hardware_uart2_enabletx(0); \
			} while (0)
	#endif /* ! defined (HARDWARE_UART2_ONTXCHAR) */

#endif /* WITHUART2HW */

#if WITHUSBCDCACM
	// Заглушки, если есть виртуальный последовательный порт, но нигде не используется.
	#if ! defined (HARDWARE_CDC_ONRXCHAR)
		// вызывается из обработчика прерываний CDC
		// с принятым символом
		#define HARDWARE_CDC_ONRXCHAR(c) do { \
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
	#if ! defined (HARDWARE_CDC_ONOVERFLOW)
		// вызывается из обработчика прерываний CDC
		#define HARDWARE_CDC_ONOVERFLOW() do { \
				usbd_cdc_enablerx(1); \
			} while (0)
	#endif /* ! defined (HARDWARE_CDC_ONOVERFLOW) */

	#if ! defined (HARDWARE_CDC_ONTXCHAR)
		// вызывается из обработчика прерываний CDC
		// по готовности передатчика
		#define HARDWARE_CDC_ONTXCHAR(ctx) do { \
				usbd_cdc_enabletx(0); \
			} while (0)
	#endif /* ! defined (HARDWARE_CDC_ONTXCHAR) */

#endif /* WITHUSBCDCACM */

#ifdef __cplusplus
}
#endif /* __cplusplus */
