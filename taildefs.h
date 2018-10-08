/* $Id$ */
#ifndef TAILDEFS_H_INCLUDED
#define TAILDEFS_H_INCLUDED

// В зависимости от типа платы - какие микросхемы применены в синтезаторе

#ifndef PRODUCT_H_INCLUDED
	#error Do not include this file directly.
#endif


enum
{
#if (SPISPEED400k) || defined (SPISPEED100k)
	SPIC_SPEED100k,		// 100 kHz for MICROCHIP MCP3204/MCP3208
	SPIC_SPEED400k,
	SPIC_SPEED10M,	/* 10 MHz для ILI9341 */
	SPIC_SPEED25M,	/* 25 MHz  */
#endif /* (SPISPEED400k) || defined (SPISPEED100k) */
	SPIC_SPEEDFAST,
#if WITHFPGAWAIT_AS || WITHFPGALOAD_PS || WITHDSPEXTFIR
	SPIC_SPEEDUFAST,	// Скорость для загрузки FPGA
#endif /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS || WITHDSPEXTFIR */
#if WITHUSESDCARD
	SPIC_SPEEDSDCARD,	// Переключаемая скоростть - 400 кГц или требуемая для SD карты
#endif /* WITHUSESDCARD */
#if LCDMODE_UC1608
	SPIC_SPEEDUC1608,	// SPISPEED for UC1608 should be less then 7.1 MHz
#endif /* LCDMODE_UC1608 */
	//
	SPIC_SPEEDS_COUNT
} spi_speeds_t;

#define MULTIVFO ((HYBRID_NVFOS > 1) && (LO1MODE_HYBRID || LO1MODE_FIXSCALE))

#define LO1DIVIDEVCO (HYBRID_OCTAVE_80_160 || HYBRID_OCTAVE_128_256 || FIXSCALE_48M0_X1_DIV256)

#if defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570)
	#define LO1MODE_DIRECT	1
	#define SYNTH_R1 1
	#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	#undef DDS1_CLK_MUL
	#define DDS1_CLK_MUL	1
	#undef DDS1_CLK_DIV
	#define DDS1_CLK_DIV	1
#endif

#if defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI5351A)
	#define LO1MODE_DIRECT	1
	#define SYNTH_R1 1
	#define PLL1_FRACTIONAL_LENGTH	10	/* Si5351a: lower 10 bits is a fractional part */
	#undef DDS1_CLK_MUL
	#define DDS1_CLK_MUL	1
	#undef DDS1_CLK_DIV
	#define DDS1_CLK_DIV	1
	#undef DDS2_CLK_MUL
	#define DDS2_CLK_MUL	1
	#undef DDS2_CLK_DIV
	#define DDS2_CLK_DIV	1
#endif

#if defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_NONE)
	#define	PLL1_FRACTIONAL_LENGTH 0
	#define SYNTH_R1	1
	#undef DDS1_CLK_MUL
	#define DDS1_CLK_MUL	1
	#undef DDS1_CLK_DIV
	#define DDS1_CLK_DIV	1
#endif


#if WITHDEBUG && WITHUART1HW && WITHDEBUG_USART1
	// Отладочные функции работают через USART1
	// Вызывается из user-mode программы при запрещённых прерываниях. 
	#define HARDWARE_DEBUG_INITIALIZE() do { \
			hardware_uart1_initialize(); \
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
			hardware_uart1_initialize(); \
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
			hardware_uart2_initialize(); \
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
			hardware_uart1_initialize(); \
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
			hardware_uart2_initialize(); \
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
			hardware_uart1_initialize(); \
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
			hardware_uart2_initialize(); \
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

#if WITHCAT && WITHUSBCDC && WITHCAT_CDC
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
	#define HARDWARE_CAT_GET_RTS() (usbd_cdc_getrts())
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния DTR
	#define HARDWARE_CAT_GET_DTR() (usbd_cdc_getdtr())

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

#endif /* WITHCAT && WITHUSBCDC && WITHCAT_CDC */


#if WITHMODEM && WITHUSBCDC && WITHMODEM_CDC
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

#endif /* WITHMODEM && WITHUSBCDC && WITHCAT_CDC */

#if WITHDEBUG && WITHUSBCDC && WITHDEBUG_CDC
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

#endif /* WITHDEBUG && WITHUSBCDC && WITHDEBUG_CDC */

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

#if WITHUSBCDC
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

#endif /* WITHUSBCDC */


#if defined (SPI_A2) && defined (SPI_A1) && defined (SPI_A0)
	// Три вывода формируют адрес
	// значения параметра target для выбора получателей на шите SPI
	#define SPI_CSEL0 (0 * SPI_A2 | 0 * SPI_A1 | 0 * SPI_A0)
	#define SPI_CSEL1 (0 * SPI_A2 | 0 * SPI_A1 | 1 * SPI_A0)
	#define SPI_CSEL2 (0 * SPI_A2 | 1 * SPI_A1 | 0 * SPI_A0)
	#define SPI_CSEL3 (0 * SPI_A2 | 1 * SPI_A1 | 1 * SPI_A0)
	#define SPI_CSEL4 (1 * SPI_A2 | 0 * SPI_A1 | 0 * SPI_A0)
	#define SPI_CSEL5 (1 * SPI_A2 | 0 * SPI_A1 | 1 * SPI_A0)
	#define SPI_CSEL6 (1 * SPI_A2 | 1 * SPI_A1 | 0 * SPI_A0)
	#define SPI_CSEL7 (1 * SPI_A2 | 1 * SPI_A1 | 1 * SPI_A0)

	#define SPI_ADDRESS_BITS (SPI_A2 | SPI_A1 | SPI_A0)	// маска, включающая все адресные биты

#elif defined (SPI_A1) && defined (SPI_A0)
	// Два вывода формируют адрес
	// значения параметра target для выбора получателей на шите SPI
	#define SPI_CSEL0 (0 * SPI_A1 | 0 * SPI_A0)
	#define SPI_CSEL1 (0 * SPI_A1 | 1 * SPI_A0)
	#define SPI_CSEL2 (1 * SPI_A1 | 0 * SPI_A0)
	#define SPI_CSEL3 (1 * SPI_A1 | 1 * SPI_A0)

	#define SPI_ADDRESS_BITS (SPI_A1 | SPI_A0)	// маска, включающая все адресные биты


#elif defined (SPI_A0)
	// Один вывод формируют адрес
	// значения параметра target для выбора получателей на шите SPI
	#define SPI_CSEL0 (0 * SPI_A0)
	#define SPI_CSEL1 (1 * SPI_A0)

	#define SPI_ADDRESS_BITS (SPI_A1 | SPI_A0)	// маска, включающая все адресные биты

#else
	// нет дешифратора адреса - прямое управление сигналами CS имеющихся SPI устройств.
#endif


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

	#define TRACE0(f)		debug_printf_P( PSTR(f))
	#define TRACE1(f,a1)		debug_printf_P( PSTR(f),(a1))
	#define TRACE2(f,a1,a2)		debug_printf_P( PSTR(f),(a1),(a2))
	#define TRACE3(f,a1,a2,a3)	debug_printf_P( PSTR(f),(a1),(a2),(a3))
	#define TRACE4(f,a1,a2,a3,a4)	debug_printf_P( PSTR(f),(a1),(a2),(a3),(a4))
	#define TRACE5(f,a1,a2,a3,a4,a5) debug_printf_P( PSTR(f),(a1),(a2),(a3),(a4),(a5))

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

#endif /* TAILDEFS_H_INCLUDED */
