/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if WITHUART6HW

#include "serial.h"
#include "formats.h"	// for debug prints
#include "board.h"
#include "gpio.h"
#include <string.h>
#include <math.h>
#include "clocks.h"

#if CPUSTYLE_STM32H7XX
	typedef USART_TypeDef UART_t;
	#undef UARTBASENAME
	#define UARTBASENAME(port) UARTBASEconcat(USART, port)
#endif

#define thisPORT 6

#endif /* WITHUART6HW */
