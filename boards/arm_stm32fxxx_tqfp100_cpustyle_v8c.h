/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// плата STM32F4DISCOVERY с процессором STM32F407VGT6

#ifndef ARM_STM32FXXX_TQFP64_CPUSTYLE_V8C_H_INCLUDED
#define ARM_STM32FXXX_TQFP64_CPUSTYLE_V8C_H_INCLUDED 1

//#define HARDWARE_ARM_USEUSART0 1		// US0: PA5/PA6 pins
//#define HARDWARE_ARM_USEUSART1 1		// US1: PA21/PA22 pins

#define WITHSPI16BIT	1		/* возможно использование 16-ти битных слов при обмене по SPI */
//#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
//#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
#define WITHSPISW 	1	/* Использование программного управления SPI. Нельзя убирать эту строку - требуется явное отключение из-за конфликта с I2C */

//#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
//#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */
//#define WITHCPUADCHW 	1	/* использование ADC */

//#define WITHUSBHW_DEVICE		USB_OTG_FS
//#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */
//#define WITHUSBDEV_VBUSSENSE	1	/* используется предопределенный вывод VBUS_SENSE */
//#define WITHUSBDEV_HSDESC	1	/* Требуется формировать дескрипторы как для HIGH SPEED */

#if WITHUSBHW
	#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
	#define WITHMODEM_CDC	1
	//#define WITHCAT_USART2		1
	//#define WITHUART2HW	1	/* Используется периферийный контроллер последовательного порта #2 */
	#define WITHDEBUG_USART2	1
	#define WITHNMEA_USART2		1	/* порт подключения GPS/GLONASS */
	//#define WITHUSBUAC		1	/* использовать виртуальную звуковую плату на USB соединении */

	#define WITHUSBCDC		1	/* ACM использовать виртуальный последовательный порт на USB соединении */
	#define WITHUSBHWCDC_N	2	/* количество виртуальных последовательных портов */
	//#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
	//#define WITHUSBRNDIS	1	/* RNDIS использовать Remote NDIS на USB соединении */
	//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */
	//#define WITHUSBHID	1	/* HID использовать Human Interface Device на USB соединении */

#else

	//#define WITHUART1HW	1	/* Используется периферийный контроллер последовательного порта #1 */
	//#define WITHUART2HW	1	/* Используется периферийный контроллер последовательного порта #2 */

	//#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
	#define WITHCAT_USART2		1
	#define WITHDEBUG_USART2	1
	#define WITHMODEM_USART2	1
	#define WITHNMEA_USART2		1

#endif

#define LS020_RS_INITIALIZE() \
	do { \
		arm_hardware_pioc_outputs2m(LS020_RS, LS020_RS); \
	} while (0)

#define LS020_RESET_INITIALIZE() \
	do { \
	arm_hardware_pioc_outputs2m(LS020_RESET, LS020_RESET); \
	} while (0)

#define LS020_RS_SET(v) do { \
		if ((v) != 0) LS020_RS_PORT_S(LS020_RS); \
		else  LS020_RS_PORT_C(LS020_RS); \
	} while (0)

#define LS020_RESET_SET(v) do { \
		if ((v) != 0) LS020_RESET_PORT_S(LS020_RESET); \
		else  LS020_RESET_PORT_C(LS020_RESET); \
	} while (0)

#if LCDMODE_SPI_NA
	// эти контроллеры требуют только RS

	#define LS020_RS_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RS			(1u << 2)			// PC2 RS signal

#elif LCDMODE_SPI_RN
	// эти контроллеры требуют только RESET

	#define LS020_RESET_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RESET			(1u << 5)			// PC5 RST signal

#elif LCDMODE_SPI_RA
	// Эти контроллеры требуют RESET и RS
	// LCDMODE_UC1608

	#define LS020_RS_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RS			(1u << 2)			// PC2 RS signal

	#define LS020_RESET_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RESET			(1u << 5)			// PC5 RST signal

#elif LCDMODE_HD44780 && (LCDMODE_SPI == 0)

	// E (enable) bit
	#define LCD_STROBE_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LCD_STROBE_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LCD_STROBE_BIT			(1u << 6)	// PC6

	// RS (address, register select) bit
	#define LCD_RS_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LCD_RS_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define ADDRES_BIT				(1u << 4)	// PC4 - bit in RS port

	// WE (write enable) bit
	#define LCD_WE_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LCD_WE_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define WRITEE_BIT				(1u << 5)	// PC5 - bit in
	
	// Выводы подключения ЖКИ индикатора WH2002 или аналогичного HD44780.
	#define LCD_DATA_INPUT			(GPIOF->IDR)
	#define LCD_DATAS_BITS			((1u << 3) | (1u << 2) | (1u << 1) | (1u << 0))	// PF3..PF0
	#define LCD_DATAS_BIT_LOW		0		// какой бит данных младший в слове считанном с порта

	#define DISPLAY_BUS_DATA_GET() ((LCD_DATA_INPUT & LCD_DATAS_BITS) >> LCD_DATAS_BIT_LOW) /* получить данные с шины LCD */
	#define DISPLAY_BUS_DATA_SET(v) do { /* выдача данных (не сдвинуьых) */ \
			const portholder_t t = (portholder_t) (v) << LCD_DATAS_BIT_LOW; \
			GPIOF->BSRR = BSRR_S(t & LCD_DATAS_BITS) | BSRR_C(~ t & LCD_DATAS_BITS); \
			__DSB(); \
		} while (0)


	/* инициализация управляющих выходов процессора для управления HD44780 - полный набор выходов */
	#define LCD_CONTROL_INITIALIZE() \
		do { \
			arm_hardware_piof_outputs2m(LCD_STROBE_BIT | WRITEE_BIT | ADDRES_BIT, 0); \
			arm_hardware_pioe_outputs((1U << 0), 0 * (1U << 0));		/* PE0 - enable backlight */ \
		} while (0)
	/* инициализация управляющих выходов процессора для управления HD44780 - WE=0 */
	#define LCD_CONTROL_INITIALIZE_WEEZERO() \
		do { \
			arm_hardware_piof_outputs2m(LCD_STROBE_BIT | WRITEE_BIT_ZERO | ADDRES_BIT, 0); \
		} while (0)
	/* инициализация управляющих выходов процессора для управления HD44780 - WE отсутствует - сигнал к индикатору заземлён */
	#define LCD_CONTROL_INITIALIZE_WEENONE() \
		do { \
			arm_hardware_piof_outputs2m(LCD_STROBE_BIT | ADDRES_BIT, 0); \
		} while (0)

	#define LCD_DATA_INITIALIZE_READ() \
		do { \
			arm_hardware_piof_inputs(LCD_DATAS_BITS);	/* переключить порт на чтение с выводов */ \
		} while (0)

	#define LCD_DATA_INITIALIZE_WRITE(v) \
		do { \
			arm_hardware_piof_outputs2m(LCD_DATAS_BITS, (v) << LCD_DATAS_BIT_LOW);	/* открыть выходы порта */ \
		} while (0)

#endif


#if WITHENCODER

	// Выводы подключения енкодера #1
	#define ENCODER_INPUT_PORT	(GPIOE->IDR) 
	#define ENCODER_BITA		(1u << 2)		// PE2
	#define ENCODER_BITB		(1u << 3)		// PE3
	#define ENCODER_BITS		(ENCODER_BITA | ENCODER_BITB)

	// Выводы подключения енкодера #2
	#define ENCODER2_INPUT_PORT	(GPIOH->IDR) 
	#define ENCODER2_BITA		(1u << 4)		// PE4
	#define ENCODER2_BITB		(1u << 5)		// PE5
	#define ENCODER2_BITS		(ENCODER2_BITA | ENCODER2_BITB)

	#define ENCODER_INITIALIZE() \
		do { \
			arm_hardware_pioe_inputs(ENCODER_BITS); \
			arm_hardware_pioe_updown(ENCODER_BITS, 0); \
			arm_hardware_pioe_onchangeinterrupt(ENCODER_BITS, ENCODER_BITS, ENCODER_BITS, ARM_OVERREALTIME_PRIORITY); \
			arm_hardware_pioe_inputs(ENCODER2_BITS); \
			arm_hardware_pioe_updown(ENCODER2_BITS, 0); \
			arm_hardware_pioe_onchangeinterrupt(0 * ENCODER2_BITS, ENCODER2_BITS, ENCODER2_BITS, ARM_OVERREALTIME_PRIORITY); \
		} while (0)

#endif

/* Распределение битов в ARM контроллерах */


#if WITHCAT || WITHNMEA
	// CAT control lines
	/* манипуляция от порта RS-232 */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
		} while (0)

	/* переход на передачу от порта RS-232 */
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
		} while (0)

	/* сигнал PPS от GPS/GLONASS/GALILEO модуля */
	#define NMEA_INITIALIZE() \
		do { \
		} while (0)
#endif

#if WITHTX


	// txpath outputs

	//#define TXPATH_TARGET_DDR_S			GPIOF->PIO_OER	// was DDRA
	//#define TXPATH_TARGET_DDR_C		GPIOD->PIO_ODR	// was DDRA
	#define TXPATH_TARGET_PORT_S(v)		do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TXPATH_TARGET_PORT_C(v)		do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
	#if 1

		// Управление передатчиком - сигналы TXPATH_ENABLE (PC12) и TXPATH_ENABLE_CW (PC13) - активны при нуле на выходе.
		#define TXPATH_BIT_ENABLE_SSB		0//(1u << 12)
		#define TXPATH_BIT_ENABLE_CW		0//(1u << 13)
		#define TXPATH_BITS_ENABLE	(TXPATH_BIT_ENABLE_SSB | TXPATH_BIT_ENABLE_CW)

		// Подготовленные управляющие слова
		#define TXGFV_RX		0
		#define TXGFV_TRANS		0			// переход между режимами приёма и передачи
		#define TXGFV_TX_SSB	TXPATH_BIT_ENABLE_SSB
		#define TXGFV_TX_CW		TXPATH_BIT_ENABLE_CW
		#define TXGFV_TX_AM		TXPATH_BIT_ENABLE_CW
		#define TXGFV_TX_NFM	TXPATH_BIT_ENABLE_CW

		// Управление передатчиком - сигналы TXPATH_ENABLE (PA11) и TXPATH_ENABLE_CW (PA10) - активны при нуле на выходе.
		// Установка начального стстояния выходов
		#define TXPATH_INITIALIZE() \
			do { \
				arm_hardware_piob_opendrain(TXPATH_BITS_ENABLE, TXPATH_BITS_ENABLE); \
			} while (0)
	#else
		// Управление передатчиком - единственный сигнал разрешения тракта
		#define TXPATH_BIT_GATE (1u << 12)	// выходной сигнал из процессора - управление передатчиком.
		//#define TXPATH_BIT_GATE_RX TXPATH_BIT_GATE	// сигнал tx2 - управление передатчиком. При приёме активен
		#define TXPATH_BIT_GATE_RX 0	// сигнал tx2 - управление передатчиком. При приёме не активен

		// Подготовленные управляющие слова
		#define TXGFV_RX		TXPATH_BIT_GATE_RX // TXPATH_BIT_GATE для страктов с общими каскадами на приём и передачу
		#define TXGFV_TRANS		0			// переход между режимами приёма и передачи
		#define TXGFV_TX_SSB	TXPATH_BIT_GATE
		#define TXGFV_TX_CW		TXPATH_BIT_GATE
		#define TXGFV_TX_AM		TXPATH_BIT_GATE
		#define TXGFV_TX_NFM	TXPATH_BIT_GATE

		// Управление передатчиком - сигнал TXPATH_BIT_GATE
		// Установка начального стстояния выходов
		#define TXPATH_INITIALIZE() \
			do { \
				arm_hardware_piob_outputs2m(TXPATH_BIT_GATE, 0); \
			} while (0)

	#endif


	// PTT input

	#define PTT_TARGET_PIN				(GPIOB->IDR) // was PINA 
	#define PTT_BIT_PTT					(1u << 11)	// PB11

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() \
		do { \
			arm_hardware_piob_inputs(PTT_BIT_PTT); \
			arm_hardware_piob_updown(PTT_BIT_PTT, 0); \
		} while (0)

#endif /* WITHTX */

#if WITHELKEY
	// Electronic key inputs
	//#define ELKEY_TARGET_PORT			GPIOF->PIO_ODSR
	#define ELKEY_TARGET_PIN			(GPIOB->IDR)	// was PINA 
	#define ELKEY_BIT_LEFT				(1u << 4)	// PB4
	#define ELKEY_BIT_RIGHT				(1u << 5)	// PB5

	#define ELKEY_INITIALIZE() \
		do { \
			arm_hardware_piob_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
			arm_hardware_piob_updown(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, 0); \
		} while (0)
#endif

#define HARDWARE_USART2_INITIALIZE() do { \
		arm_hardware_pioa_altfn2((1uL << 2) | (1uL << 3), AF_USART2); /* PA2: TX DATA line (2 MHz), PA3: RX data line */ \
		arm_hardware_pioa_updown((1uL << 3), 0);	/* PA3: pull-up RX data */ \
	} while (0)

//#define SPI_IOUPDATE_PORT_C(v)	do { GPIOC->BSRR = BSRR_C(v); } while (0)
//#define SPI_IOUPDATE_PORT_S(v)	do { GPIOC->BSRR = BSRR_S(v); } while (0)
//#define SPI_IOUPDATE_BIT		0	//GPIO_ODR_ODR5

//#define SPI_BIDIRECTIONAL 1	// ввод данных по SPI идет через тот же самый pin, что и вывод

#if 1
	// Набор определений для работы без внешнего дешифратора
	#define SPI_ADDRESS_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define SPI_ADDRESS_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	

	#define SPI_CSEL0	((1uL << 3))	//(GPIO_ODR_ODR0)	/* PE3 */
	#define SPI_CSEL1	0	//(GPIO_ODR_ODR0)	/* LED */
	#define SPI_CSEL2	0	//(GPIO_ODR_ODR0)	/* LED */
	#define SPI_CSEL3	0	//(GPIO_ODR_ODR0)	/* LED */ 
	#define SPI_CSEL4	((1u << 2))	/* PE2 - display */
	#define SPI_CSEL5	0	//(GPIO_ODR_ODR16)	/*  */
	#define SPI_CSEL6	0	//(GPIO_ODR_ODR17)	/* */
	#define SPI_CSEL7	0	//(GPIO_ODR_ODR18) 	/*  */
	// Здесь должны быть перечислены все биты формирования CS  устройстве.
	#define SPI_ALLCS_BITS (SPI_CSEL0 | SPI_CSEL1 | SPI_CSEL2 | SPI_CSEL3 | SPI_CSEL4 | SPI_CSEL5 | SPI_CSEL6 | SPI_CSEL7)
	#define SPI_ALLCS_BITSNEG 0		// Выходы, активные при "1"

#else
	// Есть внешний дешифратор на шине адреса SPI

	// биты вывода адреса чипселект дешифратора
	#define SPI_ADDRESS_PORT_S(v)	do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define SPI_ADDRESS_PORT_C(v)	do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	//#define SPI_ADDRESS_DDR_S			(GPIOC->PIO_OER)	// was DDRA
	//#define SPI_ADDRESS_DDR_C			(GPIOC->PIO_ODR)	// was DDRA

	#define SPI_A0 ((1u << 10))			// биты адреса для дешифратора SPI
	#define SPI_A1 ((1u << 11))
	#define SPI_A2 ((1u << 12))


	#define SPI_NAEN_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define SPI_NAEN_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)
	//#define SPI_NAEN_DDR_S			(GPIOA->PIO_OER)	// was DDRA
	//#define SPI_NAEN_DDR_C			(GPIOA->PIO_ODR)	// was DDRA

	#define SPI_NAEN_BIT (1u << 15)		// PA15 used
	#define SPI_ALLCS_BITS	0		// требуется для указания того, что работа с прямым выбором CS (без дешифратора) не требуется
#endif
// Набор определений для работы без внешнего дешифратора
#define SPI_ALLCS_PORT_S(v)	do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_ALLCS_PORT_C(v)	do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)

/* инициализация лиий выбора периферийных микросхем */
#define SPI_ALLCS_INITIALIZE() \
	do { \
		arm_hardware_piob_outputs2m(SPI_ALLCS_BITS, SPI_ALLCS_BITS); \
	} while (0)
/* инициализация сигналов управлдения дешифратором CS */
#define SPI_ADDRESS_AEN_INITIALIZE() \
	do { \
	} while (0)
/* инициализация сигналов управлдения дешифратором CS */
#define SPI_ADDRESS_NAEN_INITIALIZE() \
	do { \
	} while (0)
/* инициализация сигнала IOUPDATE на DDS */
/* начальное состояние = 1 */
#define SPI_IOUPDATE_INITIALIZE() \
	do { \
	} while (0)
/* инициализация сигнала IOUPDATE на DDS */
/* начальное состояние = 0 */
#define SPI_IORESET_INITIALIZE() \
	do { \
	} while (0)

// SPI control pins
// SPI1 hardware used.

// MOSI & SCK port
#define SPI_TARGET_SCLK_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_TARGET_SCLK_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_TARGET_MOSI_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_TARGET_MOSI_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)

#define	SPI_SCLK_BIT			(1U << 5)	// * PA5 бит, через который идет синхронизация SPI
#define	SPI_MOSI_BIT			(1U << 7)	// * PA7 бит, через который идет вывод (или ввод в случае двунаправленного SPI).

#define SPI_TARGET_MISO_PIN		(GPIOA->IDR)		// was PINA 
#define	SPI_MISO_BIT			(1U << 6)	// * PA6 бит, через который идет ввод с SPI.

	#define SPIIO_INITIALIZE() do { \
			arm_hardware_pioa_outputs(SPI_SCLK_BIT, SPI_SCLK_BIT); \
			arm_hardware_pioa_outputs(SPI_MOSI_BIT, SPI_MOSI_BIT); \
			arm_hardware_pioa_inputs(SPI_MISO_BIT); \
		} while (0)
	#define HARDWARE_SPI_CONNECT() do { \
			arm_hardware_pioa_altfn20(SPI_MOSI_BIT | SPI_MISO_BIT, AF_SPI1); /* В этих процессорах и входы и выходы перекдючаются на ALT FN */ \
			arm_hardware_pioa_altfn20(SPI_SCLK_BIT, AF_SPI1); /* В этих процессорах и входы и выходы перекдючаются на ALT FN */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT() do { \
			arm_hardware_pioa_outputs(SPI_SCLK_BIT, SPI_SCLK_BIT); \
			arm_hardware_pioa_outputs(SPI_MOSI_BIT, SPI_MOSI_BIT); \
			arm_hardware_pioa_inputs(SPI_MISO_BIT); \
		} while (0)
	#define HARDWARE_SPI_CONNECT_MOSI() do { \
			arm_hardware_pioa_altfn20(SPI_MOSI_BIT, AF_SPI1);	/* PIO disable for MOSI bit (SD CARD read support) */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
			arm_hardware_pioa_outputs(SPI_MOSI_BIT, SPI_MOSI_BIT);	/* PIO enable for MOSI bit (SD CARD read support)  */ \
		} while (0)

//#define SIDETONE_TARGET_BIT		(1u << 8)	// output TIM4_CH3 (PB8, base mapping)

#if 1 // WITHTWISW
//#if CPUSTYLE_ATMEGA
	//#define TARGET_TWI_PORT PORTC
	//#define TARGET_TWI_DDR DDRC
	//#define TARGET_TWI_TWCK	(1U << PC0)
	//#define TARGET_TWI_TWD	(1U << PC1)
//#elif CPUSTYLE_ARM
	#define TARGET_TWI_PORT_C(v) do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_TWI_PORT_S(v) do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_TWI_PIN		GPIOB->IDR)
	#define TARGET_TWI_TWCK		(1u << 6)		// PB6
	#define TARGET_TWI_TWD		(1u << 7)		// PB7
//#endif
	// Инициализация битов портов ввода-вывода для программной реализации I2C
	#define	TWISOFT_INITIALIZE() do { \
			enum { WORKMASK = TARGET_TWI_TWCK | TARGET_TWI_TWD };		/* битовая маска, определяет каким выводом шевелить */ \
			arm_hardware_piob_opendrain(WORKMASK, WORKMASK); \
		} while (0) 
	// Инициализация битов портов ввода-вывода для аппаратной реализации I2C
	// присоединение выводов к периферийному устройству
	#if CPUSTYLE_STM32F1XX

		#define	TWIHARD_INITIALIZE() do { \
				enum { WORKMASK = TARGET_TWI_TWCK | TARGET_TWI_TWD };		/* битовая маска, определяет каким выводом шевелить */ \
				arm_hardware_piob_periphopendrain_altfn2(TARGET_TWI_TWCK | TARGET_TWI_TWD, 255);	/* AF=4 */ \
			} while (0) 

	#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

		#define	TWIHARD_INITIALIZE() do { \
				enum { WORKMASK = TARGET_TWI_TWCK | TARGET_TWI_TWD };		/* битовая маска, определяет каким выводом шевелить */ \
				arm_hardware_piob_periphopendrain_altfn2(TARGET_TWI_TWCK | TARGET_TWI_TWD, AF_I2C1);	/* AF=4 */ \
			} while (0) 

	#endif
#endif

#if WITHCPUADCHW
	#define HARDWARE_ADC_INITIALIZE(ainmask) do { \
			arm_hardware_pioa_analoginput(((ainmask) >> 0) & 0xff);	/* ADC12_IN0..ADC12_IN7 */ \
			arm_hardware_piob_analoginput(((ainmask) >> 8) & 0x03);	/* ADC12_IN8..ADC12_IN0 */ \
			arm_hardware_pioc_analoginput(((ainmask) >> 10) & 0x3f);	/* ADC12_IN10..ADC12_IN15 */ \
		} while (0)
#endif /* WITHCPUADCHW */

#if WITHUSBHW
	/**USB_OTG_FS GPIO Configuration    
	PA9     ------> USB_OTG_FS_VBUS
	PA11     ------> USB_OTG_FS_DM
	PA12     ------> USB_OTG_FS_DP 
	*/
	#define	USBD_FS_INITIALIZE() do { \
		arm_hardware_pioa_altfn50((1U << 11) | (1U << 12), AF_OTGFS);			/* PA11, PA12 - USB_OTG_FS	*/ \
		arm_hardware_pioa_inputs(1U << 9);		/* PA9 - USB_OTG_FS_VBUS */ \
		arm_hardware_pioa_updownoff((1U << 9) | (1U << 11) | (1U << 12)); \
		} while (0)

	/**USB_OTG_HS GPIO Configuration    
	PB13     ------> USB_OTG_HS_VBUS
	PB14     ------> USB_OTG_HS_DM
	PB15     ------> USB_OTG_HS_DP 
	*/
	#define	USBD_HS_FS_INITIALIZE() do { \
		arm_hardware_piob_altfn50((1U << 14) | (1U << 15), AF_OTGHS_FS);			/* PB14, PB15 - USB_OTG_HS	*/ \
		arm_hardware_piob_inputs(1U << 13);		/* PB13 - USB_OTG_HS_VBUS */ \
		arm_hardware_piob_updownoff((1U << 13) | (1U << 14) | (1U << 15)); \
		} while (0)

	#define	USBD_HS_ULPI_INITIALIZE() do { \
		} while (0)
#endif /* WITHUSBHW */

	// PD12 - green, PD13 - orange, PD14 - red, PD15 - blue (LED4, LED3, LED5, LED6)
	#define BOARD_BLINK_BITS (1uL << 12)

	#define BOARD_BLINK_INITIALIZE() do { \
			arm_hardware_piod_outputs(BOARD_BLINK_BITS, 1 * BOARD_BLINK_BITS); \
		} while (0)
	#define BOARD_BLINK_SETSTATE(state) do { \
			if (state) \
				(GPIOD)->BSRR = BSRR_S(BOARD_BLINK_BITS); \
			else \
				(GPIOD)->BSRR = BSRR_C(BOARD_BLINK_BITS); \
		} while (0)

	/* макроопределение, которое должно включить в себя все инициализации */
	#define	HARDWARE_INITIALIZE() do { \
		BOARD_BLINK_INITIALIZE(); \
		} while (0)

#endif /* ARM_STM32FXXX_TQFP64_CPUSTYLE_V8C_H_INCLUDED */
