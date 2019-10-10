/* $Id$ */
/* board-specific CPU DIP28/TQFP32 attached signals */
// companion file for boards/atmega328_ctl_4z5ky_v1.h
//
/* Синтезатор 4Z5KY с двухстрочником http://www.cqham.ru/trx92_19.htm */
/* AD9951+AD9834 */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef ATMEGA328_CPU_4Z5KY_V1_H_INCLUDED
#define ATMEGA328_CPU_4Z5KY_V1_H_INCLUDED 1

//#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
//#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
#define WITHSPISW 	1	/* Использование программного управления SPI. */
//#define WITHTWIHW	1	/* используется аппаратный контроллер TWI (I2C) */
#undef WITHELKEY
#define WITHCPUADCHW 	1	/* использование ADC */

#define WITHUART1HW	1	/* Используется периферийный контроллер последовательного порта #1 */
//#define WITHUART2HW	1	/* Используется периферийный контроллер последовательного порта #2 */

//#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
#define WITHCAT_USART1		1
#define WITHDEBUG_USART1	1
#define WITHMODEM_USART1	1
#define WITHNMEA_USART1		1

#if LCDMODE_HD44780 && (LCDMODE_SPI == 0)

	// данные LCD
	#define LCD_DATA_PORT PORTB
	#define LCD_DATA_DIRECTION DDRB
	#define LCD_DATA_INPUT PINB

	#define LCD_DATAS_BIT_LOW PB0		// какой бит данных младший в слове считанном с порта
	#define LCD_DATAS_BITS (0x0f << LCD_DATAS_BIT_LOW)

	#define DISPLAY_BUS_DATA_GET() ((LCD_DATA_INPUT & LCD_DATAS_BITS) >> LCD_DATAS_BIT_LOW) /* получить данные с шины LCD */
	#define DISPLAY_BUS_DATA_SET(v) do { \
		LCD_DATA_PORT = \
			(LCD_DATA_PORT & ~ LCD_DATAS_BITS) | \
			(((v) << LCD_DATAS_BIT_LOW) & LCD_DATAS_BITS); \
		} while (0)

	// строб E LCD
	#define LCD_STROBE_PORT PORTB
	#define LCD_STROBE_DIRECTION DDRB
	#define LCD_STROBE_BIT (1U << PB4)

	// сигналы RS и WE LCD
	#define LCD_RS_WE_PORT PORTB
	#define LCD_RS_WE_DIRECTION DDRB
	#define ADDRES_BIT (1U << PB5)
	//#define WRITEE_BIT (1U << PB1)
	//#define WRITEE_BIT_ZERO (1U << PB1)		// для тестирования обмена с HD44780 всегда на записи

#elif LCDMODE_SPI_RA || LCDMODE_SPI_NA || LCDMODE_SPI_RN

	#define LS020_RESET_PORT PORTB
	#define LS020_RESET_DDR DDRB
	#define LS020_RST	(1U << PB0)			// D4 signal in HD44780 socket

	#define LS020_RS_PORT PORTB
	#define LS020_RS_DDR DDRB
	#define LS020_RS	(1U << PB1)			// D5 signal in HD44780 socket

	#define SPI2_TARGET_PORT PORTB
	#define SPI2_TARGET_DDR DDRB
	#define SPI2_TARGET_PIN PINB

	#define	SPI2_CS_BIT (1U << PB2)	// SS бит, через который идет выбор устройста SPI
	#define	SPI2_MOSI_BIT (1U << PB3)	// бит, через который идет вывод (или ввод в случае двунаправленного SPI).
	// PB4 - MISO - do not use
	#define	SPI2_SCLK_BIT (1U << PB5)	// бит, через который идет синхронизация SPI

	#define	SPI_CSEL2	3		// LCD

#endif

#if WITHENCODER

	#define ENCODER_TARGET_PORT PORTD
	#define ENCODER_DIRECTION_PORT DDRD
	#define ENCODER_INPUT_PORT PIND

	#define ENCODER_SHIFT PIND2
	#define ENCODER_BITS ((1U << PIND3) | (1U << PIND2))

#endif

	/* переключение бита управления несущей вызывается из прерываний -
	 всем остальным, модифицирующим биты на этом порту, выполнять под запретом прерываний.
		*/
#if 0
	#define TXPATH_TARGET_PORT PORTD	// выходы процессора - управление трактом ппередачи и манипуляцией
	#define TXPATH_TARGET_DDR DDRD		// переключение на вывод - управление трактом передачи и манипуляцией

	#if ! ELKEY328
		// Управление передатчиком - сигналы TXPATH_ENABLE (PA11) и TXPATH_ENABLE_CW (PA10) - активны при нуле на выходе.
		#define TXPATH_BIT_ENABLE_SSB		(1U << PD5)
		#define TXPATH_BIT_ENABLE_CW	(1U << PD7)
		#define TXPATH_BITS_ENABLE	(TXPATH_BIT_ENABLE_SSB | TXPATH_BIT_ENABLE_CW)
		// Подготовленные управляющие слова
		#define TXGFV_RX		0
		#define TXGFV_TRANS		0			// переход между режимами приёма и передачи
		#define TXGFV_TX_SSB	TXPATH_BIT_ENABLE_SSB
		#define TXGFV_TX_CW		TXPATH_BIT_ENABLE_CW
		#define TXGFV_TX_AM		TXPATH_BIT_ENABLE_CW
		#define TXGFV_TX_NFM	TXPATH_BIT_ENABLE_CW
	#else
		// Управление передатчиком - единственный сигнал разрешения тракта
		#define TXPATH_BIT_GATE (1U << PD5)	// выходной сигнал из процессора - управление передатчиком.
		//#define TXPATH_BIT_GATE_RX TXPATH_BIT_GATE	// сигнал tx2 - управление передатчиком. При приёме активен
		#define TXPATH_BIT_GATE_RX 0	// сигнал tx2 - управление передатчиком. При приёме не активен
		// Подготовленные управляющие слова
		#define TXGFV_RX		TXPATH_BIT_GATE_RX
		#define TXGFV_TRANS		0			// переход между режимами приёма и передачи
		#define TXGFV_TX_SSB	TXPATH_BIT_GATE
		#define TXGFV_TX_CW		TXPATH_BIT_GATE
		#define TXGFV_TX_AM		TXPATH_BIT_GATE
		#define TXGFV_TX_NFM	TXPATH_BIT_GATE
	#endif
#else
	// Подготовленные управляющие слова (заглушки)
	#define TXGFV_RX		0
	#define TXGFV_TRANS		0			// переход между режимами приёма и передачи
	#define TXGFV_TX_SSB	0
	#define TXGFV_TX_CW		0
	#define TXGFV_TX_AM		0
	#define TXGFV_TX_NFM	0

#endif

#if 0
	// процессор формирует звук самоконтроля при передаче телеграфом
	// генерация сигнала самоконтроля на PD7(OC2)
	/* самоконтроль передачи телеграфа и озвучка нажатий */
	#define SIDETONE_TARGET_PORT	PORTD	/* Порт процессора, на котором находится вывод тона самоконтроля */
	#define SIDETONE_TARGET_DDR		DDRD
	#define SIDETONE_TARGET_BIT		(1U << PD6)
#endif

	#define TARGET_TWI_TWCK_PORT PORTC
	#define TARGET_TWI_TWCK_DDR DDRC
	#define TARGET_TWI_TWCK_PIN PINC
	#define TARGET_TWI_TWCK	(1U << PC5)

	#define TARGET_TWI_TWD_PORT PORTC
	#define TARGET_TWI_TWD_DDR DDRC
	#define TARGET_TWI_TWD_PIN PINC
	#define TARGET_TWI_TWD	(1U << PC4)

#if WITHTX
	// PTT

	#define PTT_TARGET_PORT PORTD	
	#define PTT_TARGET_DDR DDRD
	#define PTT_TARGET_PIN PIND

	#define PTT_BIT_PTT (1U << PD7)		

	#if 0 && (WITHCAT || WITHNMEA)
		// Сигналы переключения на передачу и манипуляции через CAT интерфейс.
		#define FROMCAT_TARGET_PIN_RTS		PIND
		#define FROMCAT_TARGET_DDR_RTS		DDRD
		#define FROMCAT_BIT_RTS			(1U << PD4)	/* сигнал RTS от FT232RL	*/
		/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
		#define FROMCAT_TARGET_PIN_DTR		PINB
		#define FROMCAT_TARGET_DDR_DTR		DDRB
		#define FROMCAT_BIT_DTR			(1U << PB3)	/* сигнал DTR от FT232RL	*/
		// разрешение прерываний по изменению состояния на входе манипуляции по CAT
		// #2 at ATMega328 - PORT D
		#define DTRPCMSK_PORT	PCMSK0			// PCMSK0 - PB, PCMSK1 - PC, PCMSK2 - PD
		#define DTRPCICR_BIT	(1U << PCIE0)	// PCIE0 - PB, PCIE1 - PC, PCIE2 - PD
		#define DTRPCIVECT PCINT0_vect			// вектор обработчика
	#endif /* WITHCAT */

	#if 0 && WITHELKEY
		#define ELKEY_TARGET_PORT PORTD		// входы процессора - сигналы электронного ключа
		#define ELKEY_TARGET_DDR DDRD		// переключение на ввод - сигналы электронного ключа
		#define ELKEY_TARGET_PIN PIND		// ввод - сигналы электронного ключа

		#define ELKEY_BIT_LEFT (1U << PD0)
		#define ELKEY_BIT_RIGHT (1U << PD1)
		// разрешение прерываний по нажатиям на контакты ключа
		// Микропроцессорах наподобии ATMega644, ATMega328
		// #2 at ATMega328 - PORT D
		#define PCMSK_PORT	PCMSK2			// PCMSK0 - PB, PCMSK1 - PC, PCMSK2 - PD
		#define PCICR_BIT	(1U << PCIE2)	// PCIE0 - PB, PCIE1 - PC, PCIE2 - PD
		#define PCIVECT PCINT2_vect			// вектор обработчика
	#endif

#endif /* WITHTX */


	#define HARDWARE_SPI_CONNECT() do { \
		} while (0)

	#define HARDWARE_SPI_DISCONNECT() do { \
			SPCR = 0x00; \
		} while (0)
	//#define SPI_IOUPDATE_BIT (1U << PB3)	// вместо порта используется выход дешифратора CS интерфейса SPI

	// PC3: IOUPDATE
	#define SPI_IOUPDATE_PORT PORTC
	#define SPI_IOUPDATE_DDR DDRC
	#define SPI_IOUPDATE_BIT (1U << PC3)	// IOUPDATE

	#define WITHSPISPLIT	1	/* для двух разных потребителей формируются отдельные сигналы MOSI, SCK, CS */
	#define SPI_ALLCS_BITS	0		// требуется для указания того, что работа с прямым выбором CS (без дешифратора) не требуется
	
	// AD9951: PC0: RESET
	#define	TARGET_DDSRESET_PORT	PORTC
	#define	TARGET_DDSRESET_DIR	DDRC
	#define	TARGET_DDSRESET_BIT	(1U << PC0)

	#define	TARGET_DDSRESET_PORT_C(v) do { TARGET_DDSRESET_PORT &= ~ (v); } while (0)
	#define	TARGET_DDSRESET_PORT_S(v) do { TARGET_DDSRESET_PORT |= (v); } while (0)

	#define TARGET_DDSRESET_INITIALIZE() do { \
		TARGET_DDSRESET_DIR |= TARGET_DDSRESET_BIT; \
	} while (0)

	// AD9951: PC1: SCLK
	#define SPI0_TARGET_SCLK_PORT PORTC
	#define SPI0_TARGET_SCLK_DDR DDRC
	#define SPI0_TARGET_SCLK_PIN PINC
	#define	SPI0_SCLK_BIT (1U << PC1)	// бит, через который идет синхронизация SPI

	// AD9951: PB3: SCLK
	#define SPI0_TARGET_MOSI_PORT PORTB
	#define SPI0_TARGET_MOSI_DDR DDRB
	#define SPI0_TARGET_MOSI_PIN PINB
	#define	SPI0_MOSI_BIT (1U << PB3)	// бит, через который идет вывод (или ввод в случае двунаправленного SPI).

	// AD9951: PC0: RESET
	#define SPI0_TARGET_CS_PORT PORTC
	#define SPI0_TARGET_CS_DDR DDRC
	#define SPI0_TARGET_CS_PIN PINC
	#define	SPI0_CS_BIT 0//(1U << PC3)	// бит, через который идет выбор устройста SPI

	#define	SPI_CSEL0	0

	// AD9834: PD4: SCLK
	#define SPI1_TARGET_SCLK_PORT PORTD
	#define SPI1_TARGET_SCLK_DDR DDRD
	#define SPI1_TARGET_SCLK_PIN PIND
	#define	SPI1_SCLK_BIT (1U << PD4)	// бит, через который идет синхронизация SPI

	// AD9834: PB2: MOSI
	#define SPI1_TARGET_MOSI_PORT PORTB
	#define SPI1_TARGET_MOSI_DDR DDRB
	#define SPI1_TARGET_MOSI_PIN PINB
	#define	SPI1_MOSI_BIT (1U << PB2)	// бит, через который идет вывод (или ввод в случае двунаправленного SPI).

	// AD9834: PC2: CS (FSYNC)
	#define SPI1_TARGET_CS_PORT PORTC
	#define SPI1_TARGET_CS_DDR DDRC
	#define SPI1_TARGET_CS_PIN PINC
	#define	SPI1_CS_BIT (1U << PC2)	// бит, через который идет выбор устройста SPI

	#define	SPI_CSEL1	1

	// CONTROL REGISTER: PD5: SCLK (74HC595, pin 11)
	#define SPI2_TARGET_SCLK_PORT PORTD
	#define SPI2_TARGET_SCLK_DDR DDRD
	#define SPI2_TARGET_SCLK_PIN PIND
	#define	SPI2_SCLK_BIT (1U << PD5)	// бит, через который идет синхронизация SPI

	// CONTROL REGISTER: PB1: MOSI
	#define SPI2_TARGET_MOSI_PORT PORTB
	#define SPI2_TARGET_MOSI_DDR DDRB
	#define SPI2_TARGET_MOSI_PIN PINB
	#define	SPI2_MOSI_BIT (1U << PB1)	// бит, через который идет вывод (или ввод в случае двунаправленного SPI).

	// CONTROL REGISTER: PD6: CS (74HC595, pin 12)
	#define SPI2_TARGET_CS_PORT PORTD
	#define SPI2_TARGET_CS_DDR DDRD
	#define SPI2_TARGET_CS_PIN PIND
	#define	SPI2_CS_BIT (1U << PD6)	// бит, через который идет выбор устройста SPI

	#define SPI_CSEL2	2

	#define SPIIO_INITIALIZE() do { \
		/* для двух разных потребителей формируются отдельные сигналы MOSI, SCK, CS */ \
		SPI0_TARGET_SCLK_PORT |= (SPI0_SCLK_BIT);	/* порт программируется на вывод по всем выводам */ \
		SPI0_TARGET_SCLK_DDR |= (SPI0_SCLK_BIT);	/* порт программируется на вывод по всем выводам */ \
		SPI0_TARGET_MOSI_PORT |= (SPI0_MOSI_BIT);	/* порт программируется на вывод по всем выводам */ \
		SPI0_TARGET_MOSI_DDR |= (SPI0_MOSI_BIT);	/* порт программируется на вывод по всем выводам */ \
		\
		SPI1_TARGET_SCLK_PORT |= (SPI1_SCLK_BIT);	/* порт программируется на вывод по всем выводам */ \
		SPI1_TARGET_SCLK_DDR |= (SPI1_SCLK_BIT);	/* порт программируется на вывод по всем выводам */ \
		SPI1_TARGET_MOSI_PORT |= (SPI1_MOSI_BIT);	/* порт программируется на вывод по всем выводам */ \
		SPI1_TARGET_MOSI_DDR |= (SPI1_MOSI_BIT);	/* порт программируется на вывод по всем выводам */ \
		\
		SPI2_TARGET_SCLK_PORT |= (SPI2_SCLK_BIT);	/* порт программируется на вывод по всем выводам */ \
		SPI2_TARGET_SCLK_DDR |= (SPI2_SCLK_BIT);	/* порт программируется на вывод по всем выводам */ \
		SPI2_TARGET_MOSI_PORT |= (SPI2_MOSI_BIT);	/* порт программируется на вывод по всем выводам */ \
		SPI2_TARGET_MOSI_DDR |= (SPI2_MOSI_BIT);	/* порт программируется на вывод по всем выводам */ \
		} while (0)

	#define SPI_ALLCS_INITIALIZE() do { \
			/* для двух разных потребителей формируются отдельные сигналы MOSI, SCK, CS */ \
			SPI0_TARGET_CS_PORT |= (SPI0_CS_BIT);	/* порт программируется на вывод по всем выводам */ \
			SPI0_TARGET_CS_DDR |= (SPI0_CS_BIT);	/* порт программируется на вывод по всем выводам */ \
			\
			SPI1_TARGET_CS_PORT |= (SPI1_CS_BIT);	/* порт программируется на вывод по всем выводам */ \
			SPI1_TARGET_CS_DDR |= (SPI1_CS_BIT);	/* порт программируется на вывод по всем выводам */ \
			\
			SPI2_TARGET_CS_PORT |= (SPI2_CS_BIT);	/* порт программируется на вывод по всем выводам */ \
			SPI2_TARGET_CS_DDR |= (SPI2_CS_BIT);	/* порт программируется на вывод по всем выводам */ \
		} while (0)





#if KEYBOARD_USE_ADC == 0
	#define KBD_TARGET_PORT PORTC
	#define KBD_TARGET_DDR DDRC
	#define KBD_TARGET_PIN PINC

	#define KBD_MASK 0xcf	// все используемые биты
#endif

	/* программирование выводов, управляющих I2C */
	#define TWISOFT_INITIALIZE() do { \
		HARDWARE_INPUT_INITIALIZE(TARGET_TWI_TWCK_PORT, TARGET_TWI_TWCK_DDR, TARGET_TWI_TWCK, TARGET_TWI_TWCK); \
		HARDWARE_INPUT_INITIALIZE(TARGET_TWI_TWD_PORT, TARGET_TWI_TWD_DDR, TARGET_TWI_TWD, TARGET_TWI_TWD); \
		} while (0)

	// Инициализация битов портов ввода-вывода для аппаратной реализации I2S
	// присоединение выводов к периферийному устройству
	// На ATMEGA не требуется
		#define	TWIHARD_INITIALIZE() do { \
		} while (0) 

	#define CS4272_RESET_INITIALIZE() do { \
			HARDWARE_OUTPUT_INITIALIZE(TARGET_CS4272_RESET_PORT, TARGET_CS4272_RESET_DDR, TARGET_CS4272_RESET_BIT, TARGET_CS4272_RESET_BIT); \
		} while (0)

	#define LS020_RS_INITIALIZE() do { \
			HARDWARE_OUTPUT_INITIALIZE(LS020_RS_PORT, LS020_RS_DDR, LS020_RS, LS020_RS); \
		} while(0)

	#define LS020_RESET_INITIALIZE() do { \
			HARDWARE_OUTPUT_INITIALIZE(LS020_RESET_PORT, LS020_RESET_DDR, LS020_RST, LS020_RST); \
		} while(0)

	#define LS020_RS_SET(v) do { \
			if ((v) != 0) LS020_RS_PORT_S(LS020_RS); \
			else  LS020_RS_PORT_C(LS020_RS); \
		} while (0)

	#define LS020_RST_SET(v) do { \
			if ((v) != 0) LS020_RST_PORT_S(LS020_RST); \
			else  LS020_RST_PORT_C(LS020_RST); \
		} while (0)

	#define LS020_RESET_PORT_S(b)	do { LS020_RESET_PORT |= (b); } while (0)
	#define LS020_RESET_PORT_C(b)	do { LS020_RESET_PORT &= ~ (b); } while (0)
	#define LS020_RS_PORT_S(b)		do { LS020_RS_PORT |= (b); } while (0)
	#define LS020_RS_PORT_C(b)		do { LS020_RS_PORT &= ~ (b); } while (0)

	#if KEYBOARD_USE_ADC
		#define HARDWARE_KBD_INITIALIZE() do { \
			} while (0)
	#else
		#define HARDWARE_KBD_INITIALIZE() do { \
				KBD_TARGET_PORT |= KBD_MASK;	/* tie up inputs */ \
				KBD_TARGET_DDR &= ~ KBD_MASK;	/* define these bits as inputs - enable pull-up */ \
			} while (0)
	#endif

#if SPI_BIDIRECTIONAL && CPUSTYLE_ATMEGA

	#define SPIIO_MOSI_TO_INPUT() do { \
		SPI_TARGET_MOSI_DDR &= ~ SPI_MOSI_BIT;	/* переключить порт на чтение с выводов */ \
		} while (0)
	#define SPIIO_MOSI_TO_OUTPUT() do { \
		SPI_TARGET_MOSI_DDR |= SPI_MOSI_BIT;	/* открыть выходы порта */ \
		} while (0)

#endif /* SPI_BIDIRECTIONAL && CPUSTYLE_ATMEGA */


#if CPUSTYLE_ATMEGA328

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() do { \
		PTT_TARGET_PORT |= PTT_BIT_PTT;	/* включть pull-up на этот вход. */ \
		PTT_TARGET_DDR &= ~ PTT_BIT_PTT;	/* бит входа PTT */ \
	} while (0)

	#define ELKEY_INITIALIZE() do { \
		ELKEY_TARGET_DDR &= ~ ELKEY_BIT_LEFT;	/* бит входа левого контакта манипулятора */ \
		ELKEY_TARGET_PORT |= ELKEY_BIT_LEFT; \
		ELKEY_TARGET_DDR &= ~ ELKEY_BIT_RIGHT;	/* бит входа правого контакта манипулятора */ \
		ELKEY_TARGET_PORT |= ELKEY_BIT_RIGHT; \
		/* PCMSK_PORT |= (ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); */ /* разрешение прерываний по нажатиям на контакты ключа */ \
		/* PCICR |= (PCICR_BIT); */ \
	} while (0)

	#define NMEA_INITIALIZE() do { \
		FROMCAT_TARGET_PORT_DTR |= FROMCAT_BIT_DTR;		/* enable pull-up */ \
		FROMCAT_TARGET_DDR_DTR &= ~ FROMCAT_BIT_DTR;	/* бит входа DTR */ \
		/* DTRPCMSK_PORT |= (FROMCAT_BIT_DTR); */ /* разрешение прерываний по изменению состояния DTR */ \
		/* PCICR |= (DTRPCICR_BIT); */ /* разрешение прерываний по изменению состояния DTR */ \
	} while (0)

	#define FROMCAT_DTR_INITIALIZE() do { \
		FROMCAT_TARGET_PORT_DTR |= FROMCAT_BIT_DTR;		/* enable pull-up */ \
		FROMCAT_TARGET_DDR_DTR &= ~ FROMCAT_BIT_DTR;	/* бит входа DTR */ \
		/* DTRPCMSK_PORT |= (FROMCAT_BIT_DTR); */ /* разрешение прерываний по изменению состояния DTR */ \
		/* PCICR |= (DTRPCICR_BIT); */ /* разрешение прерываний по изменению состояния DTR */ \
	} while (0)

	// FROMCAT_RTS_INITIALIZE variations
	#if FROMCAT_BIT_RTS != 0
		#define FROMCAT_RTS_INITIALIZE() do { \
			FROMCAT_TARGET_PORT_RTS |= FROMCAT_BIT_RTS;		/* enable pull-up */ \
			FROMCAT_TARGET_DDR_RTS &= ~ FROMCAT_BIT_RTS;	/* бит входа RTS */ \
		} while (0)
	#else /* FROMCAT_BIT_RTS != 0 */
		#define FROMCAT_RTS_INITIALIZE() do { \
		} while (0)
	#endif /* FROMCAT_BIT_RTS != 0 */

	#define ENCODER_INITIALIZE() do { \
		EICRA |= 0x05; \
		EIMSK |= 0x03; \
		EIFR |= 0x03; \
		ENCODER_TARGET_PORT |= ENCODER_BITS; /* PULL-UP */  \
		ENCODER_DIRECTION_PORT &= ~ ENCODER_BITS; \
	} while (0)

#elif CPUSTYLE_ATMEGA_XXX4

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() do { \
		PTT_TARGET_PORT |= PTT_BIT_PTT;	/* включть pull-up на этот вход. */ \
		PTT_TARGET_DDR &= ~ PTT_BIT_PTT;	/* бит входа PTT */ \
	} while (0)

	#define ELKEY_INITIALIZE() do { \
		ELKEY_TARGET_DDR &= ~ ELKEY_BIT_LEFT;	/* бит входа левого контакта манипулятора */ \
		ELKEY_TARGET_PORT |= ELKEY_BIT_LEFT; \
		ELKEY_TARGET_DDR &= ~ ELKEY_BIT_RIGHT;	/* бит входа правого контакта манипулятора */ \
		ELKEY_TARGET_PORT |= ELKEY_BIT_RIGHT; \
		/* PCMSK_PORT |= (ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); */ /* разрешение прерываний по нажатиям на контакты ключа */ \
		/* PCICR |= (PCICR_BIT); */ \
	} while (0)

	#define NMEA_INITIALIZE() do { \
		FROMCAT_TARGET_PORT_DTR |= FROMCAT_BIT_DTR;		/* enable pull-up */ \
		FROMCAT_TARGET_DDR_DTR &= ~ FROMCAT_BIT_DTR;	/* бит входа DTR */ \
		/* DTRPCMSK_PORT |= (FROMCAT_BIT_DTR); */ /* разрешение прерываний по изменению состояния DTR */ \
		/* PCICR |= (DTRPCICR_BIT); */ /* разрешение прерываний по изменению состояния DTR */ \
	} while (0)

	#define FROMCAT_DTR_INITIALIZE() do { \
		FROMCAT_TARGET_PORT_DTR |= FROMCAT_BIT_DTR;		/* enable pull-up */ \
		FROMCAT_TARGET_DDR_DTR &= ~ FROMCAT_BIT_DTR;	/* бит входа DTR */ \
		/* DTRPCMSK_PORT |= (FROMCAT_BIT_DTR); */ /* разрешение прерываний по изменению состояния DTR */ \
		/* PCICR |= (DTRPCICR_BIT); */ /* разрешение прерываний по изменению состояния DTR */ \
	} while (0)

	// FROMCAT_RTS_INITIALIZE variations
	#if FROMCAT_BIT_RTS != 0
		#define FROMCAT_RTS_INITIALIZE() do { \
			FROMCAT_TARGET_PORT_RTS |= FROMCAT_BIT_RTS;		/* enable pull-up */ \
			FROMCAT_TARGET_DDR_RTS &= ~ FROMCAT_BIT_RTS;	/* бит входа RTS */ \
		} while (0)
	#else /* FROMCAT_BIT_RTS != 0 */
		#define FROMCAT_RTS_INITIALIZE() do { \
		} while (0)
	#endif /* FROMCAT_BIT_RTS != 0 */

	#define ENCODER_INITIALIZE() do { \
		EICRA |= 0x05; \
		EIMSK |= 0x03; \
		EIFR |= 0x03; \
		ENCODER_TARGET_PORT |= ENCODER_BITS; /* PULL-UP */  \
		ENCODER_DIRECTION_PORT &= ~ ENCODER_BITS; \
	} while (0)

#elif CPUSTYLE_ATMEGA32

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() do { \
		PTT_TARGET_PORT |= PTT_BIT_PTT;	/* включть pull-up на этот вход. */ \
		PTT_TARGET_DDR &= ~ PTT_BIT_PTT;	/* бит входа PTT */ \
	} while (0)

	#define ELKEY_INITIALIZE() do { \
		ELKEY_TARGET_DDR &= ~ ELKEY_BIT_LEFT;	/* бит входа левого контакта манипулятора */ \
		ELKEY_TARGET_PORT |= ELKEY_BIT_LEFT; \
		ELKEY_TARGET_DDR &= ~ ELKEY_BIT_RIGHT;	/* бит входа правого контакта манипулятора */ \
		ELKEY_TARGET_PORT |= ELKEY_BIT_RIGHT; \
	} while (0)

	#define NMEA_INITIALIZE() do { \
		FROMCAT_TARGET_PORT_DTR |= FROMCAT_BIT_DTR;		/* enable pull-up */ \
		FROMCAT_TARGET_DDR_DTR &= ~ FROMCAT_BIT_DTR;	/* бит входа DTR */ \
		/* DTRPCMSK_PORT |= (FROMCAT_BIT_DTR); */ /* разрешение прерываний по изменению состояния DTR */ \
		/* PCICR |= (DTRPCICR_BIT); */ /* разрешение прерываний по изменению состояния DTR */ \
	} while (0)

	#define FROMCAT_DTR_INITIALIZE() do { \
		FROMCAT_TARGET_PORT_DTR |= FROMCAT_BIT_DTR;		/* enable pull-up */ \
		FROMCAT_TARGET_DDR_DTR &= ~ FROMCAT_BIT_DTR;	/* бит входа DTR */ \
		/* DTRPCMSK_PORT |= (FROMCAT_BIT_DTR); */ /* разрешение прерываний по изменению состояния DTR */ \
		/* PCICR |= (DTRPCICR_BIT); */ /* разрешение прерываний по изменению состояния DTR */ \
	} while (0)

	// FROMCAT_RTS_INITIALIZE variations
	#if FROMCAT_BIT_RTS != 0
		#define FROMCAT_RTS_INITIALIZE() do { \
			FROMCAT_TARGET_PORT_RTS |= FROMCAT_BIT_RTS;		/* enable pull-up */ \
			FROMCAT_TARGET_DDR_RTS &= ~ FROMCAT_BIT_RTS;	/* бит входа RTS */ \
		} while (0)
	#else /* FROMCAT_BIT_RTS != 0 */
		#define FROMCAT_RTS_INITIALIZE() do { \
		} while (0)
	#endif /* FROMCAT_BIT_RTS != 0 */

	#define ENCODER_INITIALIZE() do { \
		GICR |= 0xC0; \
		MCUCR = 0x05; \
		MCUCSR = 0x00; \
		GIFR = 0xC0; \
		ENCODER_TARGET_PORT |= ENCODER_BITS; /* PULL-UP */  \
		ENCODER_DIRECTION_PORT &= ~ ENCODER_BITS; \
	} while (0)

#endif /* CPUSTYLE_ATMEGA_XXX4 */

#if CPUSTYLE_ATMEGA || CPUSTYLE_ATXMEGA

	// Variations of SPI_ADDRESS_AEN_INITIALIZE
	// Variations of SPI_ADDRESS_NAEN_INITIALIZE
	#if SPI_AEN_BIT != 0

		#define SPI_ADDRESS_AEN_INITIALIZE() do { \
			/* программирование сигналов адреса SPI */ \
			SPI_ADDRESS_DDR |= SPI_ADDRESS_BITS; \
			/* программирование SS SPI */ \
			SPI_AEN_PORT &= ~ (SPI_AEN_BIT); \
			SPI_AEN_DDR |= (SPI_AEN_BIT); \
			} while (0)

		#define SPI_ADDRESS_NAEN_INITIALIZE() do { \
			} while (0)

	#elif SPI_NAEN_BIT != 0

		#define SPI_ADDRESS_NAEN_INITIALIZE() do { \
			/* программирование сигналов адреса SPI */ \
			SPI_ADDRESS_DDR |= SPI_ADDRESS_BITS; \
			/* программирование SS SPI */ \
			SPI_NAEN_PORT &= ~ (SPI_NAEN_BIT); \
			SPI_NAEN_DDR |= (SPI_NAEN_BIT); \
			} while (0)

		#define SPI_ADDRESS_AEN_INITIALIZE() do { \
			} while (0)

	#else /* SPI_AEN_BIT != 0 */

		#define SPI_ADDRESS_NAEN_INITIALIZE() do { \
			} while (0)

		#define SPI_ADDRESS_AEN_INITIALIZE() do { \
			} while (0)

	#endif /* SPI_AEN_BIT != 0 */

	// Variations of SPI_IOUPDATE_INITIALIZE
	#if SPI_IOUPDATE_BIT != 0

		#define SPI_IOUPDATE_INITIALIZE() do { \
			SPI_IOUPDATE_PORT |= SPI_IOUPDATE_BIT;	/* Неактивное состояние - "1" */ \
			SPI_IOUPDATE_DDR |= SPI_IOUPDATE_BIT; \
			} while (0)

	#else /* SPI_IOUPDATE_BIT != 0 */

		#define SPI_IOUPDATE_INITIALIZE() do { \
			} while (0)

	#endif /* SPI_IOUPDATE_BIT != 0 */

	// Variations of SPI_IORESET_INITIALIZE
	#if SPI_IORESET_BIT != 0

		// просто сбрасываем. Управления через этот вывод не идёт.
		// В новых платах этого бита нет.

		#define SPI_IORESET_INITIALIZE() do { \
			SPI_IORESET_PORT &= ~ SPI_IORESET_BIT; \
			SPI_IORESET_DDR |= SPI_IORESET_BIT; \
			} while (0)

	#else /* SPI_IOUPDATE_BIT != 0 */

		#define SPI_IORESET_INITIALIZE() do { \
			} while (0)

	#endif /* SPI_IOUPDATE_BIT != 0 */

#endif /* CPUSTYLE_ATMEGA || CPUSTYLE_ATXMEGA */

#endif /* ATMEGA328_CPU_4Z5KY_V1_H_INCLUDED */
