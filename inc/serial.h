/*
 * serial.h
 *
 *  Created on: Sep 20, 2023
 *      Author: Gena
 */

#ifndef INC_SERIAL_H_
#define INC_SERIAL_H_

#include <stdint.h>

void serial_set_handler(uint_fast16_t int_id, void (* handler)(void));


void cat2_parsechar(uint_fast8_t c);				/* вызывается из обработчика прерываний */
void cat2_rxoverflow(void);							/* вызывается из обработчика прерываний */
void cat2_disconnect(void);							/* вызывается из обработчика прерываний произошёл разрыв связи при работе по USB CDC */
void cat2_sendchar(void * ctx);							/* вызывается из обработчика прерываний */

// Функции тестирования работы компорта по прерываниям
void cat3_parsechar(uint_fast8_t c);				/* вызывается из обработчика прерываний */
void cat3_rxoverflow(void);							/* вызывается из обработчика прерываний */
void cat3_disconnect(void);							/* вызывается из обработчика прерываний */
void cat3_sendchar(void * ctx);							/* вызывается из обработчика прерываний */

// Функции тестирования работы компорта по прерываниям
void cat7_parsechar(uint_fast8_t c);				/* вызывается из обработчика прерываний */
void cat7_rxoverflow(void);							/* вызывается из обработчика прерываний */
void cat7_disconnect(void);							/* вызывается из обработчика прерываний */
void cat7_sendchar(void * ctx);							/* вызывается из обработчика прерываний */

void modem_parsechar(uint_fast8_t c);				/* вызывается из обработчика прерываний */
void modem_rxoverflow(void);						/* вызывается из обработчика прерываний */
void modem_disconnect(void);						/* вызывается из обработчика прерываний */
void modem_sendchar(void * ctx);							/* вызывается из обработчика прерываний */

void nmea_format(const char * format, ...) __attribute__ ((__format__ (__printf__, 1, 2)));
int nmea_putc(int c);

void hardware_uart0_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate, uint_fast8_t bits, uint_fast8_t parity, uint_fast8_t odd);
void hardware_uart0_set_speed(uint_fast32_t baudrate);
void hardware_uart0_tx(void * ctx, uint_fast8_t c);	/* передача символа после прерывания о готовности передатчика */
void hardware_uart0_enabletx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
void hardware_uart0_enablerx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
uint_fast8_t hardware_uart0_putchar(uint_fast8_t c);/* передача символа если готов порт */
uint_fast8_t hardware_uart0_getchar(char * cp); /* приём символа, если готов порт */
void hardware_uart0_flush(void);	/* дождаться, когда буде все передано */

void hardware_uart1_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate, uint_fast8_t bits, uint_fast8_t parity, uint_fast8_t odd);
void hardware_uart1_set_speed(uint_fast32_t baudrate);
void hardware_uart1_tx(void * ctx, uint_fast8_t c);	/* передача символа после прерывания о готовности передатчика */
void hardware_uart1_enabletx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
void hardware_uart1_enablerx(uint_fast8_t state);	/* <i> из обработчика прерываний */
uint_fast8_t hardware_uart1_putchar(uint_fast8_t c);/* передача символа если готов порт */
uint_fast8_t hardware_uart1_getchar(char * cp); /* приём символа, если готов порт */
void hardware_uart1_flush(void);	/* дождаться, когда буде все передано */

void hardware_uart2_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate, uint_fast8_t bits, uint_fast8_t parity, uint_fast8_t odd);
void hardware_uart2_set_speed(uint_fast32_t baudrate);
void hardware_uart2_tx(void * ctx, uint_fast8_t c);	/* передача символа после прерывания о готовности передатчика */
void hardware_uart2_enabletx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
void hardware_uart2_enablerx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
uint_fast8_t hardware_uart2_putchar(uint_fast8_t c);/* передача символа если готов порт */
uint_fast8_t hardware_uart2_getchar(char * cp); /* приём символа, если готов порт */
void hardware_uart2_flush(void);	/* дождаться, когда буде все передано */

void hardware_uart3_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate, uint_fast8_t bits, uint_fast8_t parity, uint_fast8_t odd);
void hardware_uart3_set_speed(uint_fast32_t baudrate);
void hardware_uart3_tx(void * ctx, uint_fast8_t c);	/* передача символа после прерывания о готовности передатчика */
void hardware_uart3_enabletx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
void hardware_uart3_enablerx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
uint_fast8_t hardware_uart3_putchar(uint_fast8_t c);/* передача символа если готов порт */
uint_fast8_t hardware_uart3_getchar(char * cp); /* приём символа, если готов порт */
void hardware_uart3_flush(void);	/* дождаться, когда буде все передано */

void hardware_uart4_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate, uint_fast8_t bits, uint_fast8_t parity, uint_fast8_t odd);
void hardware_uart4_set_speed(uint_fast32_t baudrate);
void hardware_uart4_tx(void * ctx, uint_fast8_t c);	/* передача символа после прерывания о готовности передатчика */
void hardware_uart4_enabletx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
void hardware_uart4_enablerx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
uint_fast8_t hardware_uart4_putchar(uint_fast8_t c);/* передача символа если готов порт */
uint_fast8_t hardware_uart4_getchar(char * cp); /* приём символа, если готов порт */
void hardware_uart4_flush(void);	/* дождаться, когда буде все передано */

void hardware_uart5_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate, uint_fast8_t bits, uint_fast8_t parity, uint_fast8_t odd);
void hardware_uart5_set_speed(uint_fast32_t baudrate);
void hardware_uart5_tx(void * ctx, uint_fast8_t c);	/* передача символа после прерывания о готовности передатчика */
void hardware_uart5_enabletx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
void hardware_uart5_enablerx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
uint_fast8_t hardware_uart5_putchar(uint_fast8_t c);/* передача символа если готов порт */
uint_fast8_t hardware_uart5_getchar(char * cp); /* приём символа, если готов порт */
void hardware_uart5_flush(void);	/* дождаться, когда буде все передано */

void hardware_uart6_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate, uint_fast8_t bits, uint_fast8_t parity, uint_fast8_t odd);
void hardware_uart6_set_speed(uint_fast32_t baudrate);
void hardware_uart6_tx(void * ctx, uint_fast8_t c);	/* передача символа после прерывания о готовности передатчика */
void hardware_uart6_enabletx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
void hardware_uart6_enablerx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
uint_fast8_t hardware_uart6_putchar(uint_fast8_t c);/* передача символа если готов порт */
uint_fast8_t hardware_uart6_getchar(char * cp); /* приём символа, если готов порт */
void hardware_uart6_flush(void);	/* дождаться, когда буде все передано */

void hardware_uart7_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate, uint_fast8_t bits, uint_fast8_t parity, uint_fast8_t odd);
void hardware_uart7_set_speed(uint_fast32_t baudrate);
void hardware_uart7_tx(void * ctx, uint_fast8_t c);	/* передача символа после прерывания о готовности передатчика */
void hardware_uart7_enabletx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
void hardware_uart7_enablerx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
uint_fast8_t hardware_uart7_putchar(uint_fast8_t c);/* передача символа если готов порт */
uint_fast8_t hardware_uart7_getchar(char * cp); /* приём символа, если готов порт */
void hardware_uart7_flush(void);	/* дождаться, когда буде все передано */

void usbd_cdc_tx(void * ctx, uint_fast8_t c);			/* передача символа после прерывания о готовности передатчика - вызывается из HARDWARE_CDC_ONTXCHAR */
void usbd_cdc_enabletx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
void usbd_cdc_enablerx(uint_fast8_t state);	/* вызывается из обработчика прерываний */

void usbd_cdc_send(const void * buff, size_t length);	/* временное решение для передачи */
uint_fast8_t usbd_cdc_ready(void);	/* временное решение для передачи */

void btspp_tx(void * ctx, uint_fast8_t c);			/* передача символа после прерывания о готовности передатчика - вызывается из HARDWARE_CDC_ONTXCHAR */
void btspp_enabletx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
void btspp_enablerx(uint_fast8_t state);	/* вызывается из обработчика прерываний */

void btspp_send(const void * buff, size_t length);	/* временное решение для передачи */
uint_fast8_t btspp_ready(void);	/* временное решение для передачи */


#if defined(STM32F401xC)


#elif CPUSTYLE_STM32F4XX

#elif CPUSTYLE_STM32F7XX

	#define BOARD_USART1_FREQ (stm32f7xx_get_usart1_freq())
	#define BOARD_USART2_FREQ 	(stm32f7xx_get_apb1_freq())
	#define BOARD_USART3_FREQ 	(stm32f7xx_get_usart3_freq())

#elif CPUSTYLE_STM32H7XX

	// See Table 8. Register boundary addresses
	#define BOARD_USART1_FREQ 	(stm32h7xx_get_usart1_6_freq())
	#define BOARD_USART2_FREQ 	(stm32h7xx_get_usart2_to_8_freq())
	#define BOARD_USART3_FREQ 	(stm32h7xx_get_usart2_to_8_freq())

#elif CPUSTYLE_STM32MP1

	//#define BOARD_USART1_FREQ 	(stm32mp1_uart1_get_freq())
	//#define BOARD_USART2_FREQ 	(stm32mp1_uart2_4_get_freq())
	//#define BOARD_USART3_FREQ 	(stm32mp1_uart3_5_get_freq())
	//#define BOARD_UART4_FREQ 	(stm32mp1_uart2_4_get_freq())
	//#define BOARD_UART5_FREQ 	(stm32mp1_uart3_5_get_freq())
	//#define BOARD_USART6_FREQ 	(stm32mp1_usart6_get_freq())
	//#define BOARD_UART7_FREQ 	(stm32mp1_uart7_8_get_freq())
	//#define BOARD_UART8_FREQ 	(stm32mp1_uart7_8_get_freq())

#endif

#endif /* INC_SERIAL_H_ */
