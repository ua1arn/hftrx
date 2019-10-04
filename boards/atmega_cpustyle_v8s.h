/* $Id$ */
/* board-specific CPU DIP40/TQFP44 attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef ATMEGA_CPUSTYLE_V8S_H_INCLUDED
#define ATMEGA_CPUSTYLE_V8S_H_INCLUDED 1

#define WITHSPIHW 	1	/* Использование аппаратного контроллера SPI */
//#define WITHSPISW 	1	/* Использование программного контроллера SPI */

#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
//#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */
#define WITHCPUADCHW 	1	/* использование ADC */

#define WITHUART1HW	1	/* Используется периферийный контроллер последовательного порта #1 */
//#define WITHUART2HW	1	/* Используется периферийный контроллер последовательного порта #2 */

//#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
#define WITHCAT_USART1		1
#define WITHDEBUG_USART1	1
#define WITHMODEM_USART1	1
#define WITHNMEA_USART1		1

// Аппаратура контроллера в корпусе DIP40/TQFP44 с общими выводами для HD44780 и I2C (не одновременно)
// "простой синтезатор на AD9835"

#if LCDMODE_SPI_RN
	#define LS020_RESET_PORT PORTC
	#define LS020_RESET_DDR DDRC
	#define LS020_RST	(1U << PC2)			// Pin 21 - D6 signal in HD44780 socket

#elif LCDMODE_SPI_RA
	#define LS020_RESET_PORT PORTC
	#define LS020_RESET_DDR DDRC
	#define LS020_RST	(1U << PC2)			// Pin 21 - D6 signal in HD44780 socket

	#define LS020_RS_PORT PORTC
	#define LS020_RS_DDR DDRC
	#define LS020_RS	(1U << PC3)			// Pin 22 - D7 signal in HD44780 socket

#elif LCDMODE_SPI_NA
	#define LS020_RS_PORT PORTC
	#define LS020_RS_DDR DDRC
	#define LS020_RS	(1U << PC3)			// Pin 22 - D7 signal in HD44780 socket

#elif LCDMODE_HD44780 && (LCDMODE_SPI == 0)
	// HD44780 контроллеры

	#if WITHTWIHW
		//#error WITHTWIHW not supported with HD44780 LCD interface
	#endif
	#undef WITHTWIHW

	// данные LCD
	#define LCD_DATA_PORT PORTC
	#define LCD_DATA_DIRECTION DDRC
	#define LCD_DATA_INPUT PINC
	// строб E LCD
	#define LCD_STROBE_PORT PORTC
	#define LCD_STROBE_DIRECTION DDRC
	// сигналы RS и WE LCD
	#define LCD_RS_WE_PORT PORTB
	#define LCD_RS_WE_DIRECTION DDRB

	// Биты, используемые при работе с LCD
	#define LCD_STROBE_BIT (1U << PC4)

	#define LCD_DATAS_BIT_LOW PC0		// какой бит данных младший в слове считанном с порта
	#define LCD_DATAS_BITS (0x0f << LCD_DATAS_BIT_LOW)

	#define DISPLAY_BUS_DATA_GET() ((LCD_DATA_INPUT & LCD_DATAS_BITS) >> LCD_DATAS_BIT_LOW) /* получить данные с шины LCD */
	#define DISPLAY_BUS_DATA_SET(v) do { \
		LCD_DATA_PORT = \
			(LCD_DATA_PORT & ~ LCD_DATAS_BITS) | \
			(((v) << LCD_DATAS_BIT_LOW) & LCD_DATAS_BITS); \
		} while (0)

	#define WRITEE_BIT (1U << PB1)
	//#define WRITEE_BIT_ZERO (1U << PB1)		// для тестирования обмена с HD44780 всегда на записи
	#define ADDRES_BIT (1U << PB0)

#endif

#if LCDMODE_UC1608
	#define UC1608_CSP_PORT PORTC
	#define UC1608_CSP_DDR DDRC
	#define UC1608_CSP 	(1U << PC4)			// Pin 23 - E signal on HD44780 socket - positive chipselect
	#define SPI_CSEL255	255					// по этому чипселекту выбираем положительным сигналом

#endif


#if WITHENCODER

	#define ENCODER_TARGET_PORT PORTD
	#define ENCODER_DIRECTION_PORT DDRD
	#define ENCODER_INPUT_PORT PIND

	#define ENCODER_SHIFT PIND2
	#define ENCODER_BITS ((1U << PIND3) | (1U << PIND2))
#endif

#if WITHTX
	/* переключение бита управления несущей вызывается из прерываний -
	 всем остальным, модифицирующим биты на этом порту, выполнять под запретом прерываний.
		*/

	#define TXPATH_TARGET_PORT PORTD	// выходы процессора - управление трактом ппередачи и манипуляцией
	#define TXPATH_TARGET_DDR DDRD		// переключение на вывод - управление трактом передачи и манипуляцией

	#if 1
		// Управление передатчиком - сигналы TXPATH_ENABLE (PD5) и TXPATH_ENABLE_CW (PD6) - активны при нуле на выходе.
		#define TXPATH_BIT_ENABLE_SSB	(1U << PD5)
		#define TXPATH_BIT_ENABLE_CW	(1U << PD6)
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
		#define TXPATH_BIT_GATE (1U << PD6)	// выходной сигнал из процессора - управление передатчиком.
		//#define TXPATH_BIT_GATE_RX TXPATH_BIT_GATE	// сигнал tx2 - управление передатчиком. При приёме активен
		#define TXPATH_BIT_GATE_RX 0	// сигнал tx2 - управление передатчиком. При приёме не активен

		// Подготовленные управляющие слова
		#define TXGFV_RX		TXPATH_BIT_GATE_RX // TXPATH_BIT_GATE для страктов с общими каскадами на приём и передачу
		#define TXGFV_TRANS		0			// переход между режимами приёма и передачи
		#define TXGFV_TX_SSB	TXPATH_BIT_GATE
		#define TXGFV_TX_CW		TXPATH_BIT_GATE
		#define TXGFV_TX_AM		TXPATH_BIT_GATE
		#define TXGFV_TX_NFM	TXPATH_BIT_GATE

	#endif


	// процессор формирует звук самоконтроля при передаче телеграфом
	// генерация сигнала самоконтроля на PD7(OC2)

	#define PTT_TARGET_PORT PORTC	
	#define PTT_TARGET_DDR DDRC
	#define PTT_TARGET_PIN PINC

	#define PTT_BIT_PTT (1U << PC7)		

#endif /* WITHTX */

#if WITHCAT || WITHNMEA
	// Сигналы переключения на передачу и манипуляции через CAT интерфейс.
	#define FROMCAT_TARGET_PIN_RTS		PIND
	#define FROMCAT_TARGET_PORT_RTS		PORTD
	#define FROMCAT_TARGET_DDR_RTS		DDRD
	#define FROMCAT_BIT_RTS			(1U << PD4)	/* сигнал RTS от FT232RL	*/
	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	#define FROMCAT_TARGET_PIN_DTR		PINB
	#define FROMCAT_TARGET_PORT_DTR		PORTB
	#define FROMCAT_TARGET_DDR_DTR		DDRB
	#define FROMCAT_BIT_DTR			(1U << PB3)	/* сигнал DTR от FT232RL	*/
	// разрешение прерываний по изменению состояния на входе манипуляции по CAT
	// в микропроцессорах наподобии ATMega644, ATMega328
	#define DTRPCMSK_PORT	PCMSK1			// PCMSK0 - PA, PCMSK1 - PB, PCMSK2 - PC, PCMSK3 - PD
	#define DTRPCICR_BIT	(1U << PCIE1)	// PCIE0 - PA, PCIE1 - PB, PCIE2 - PC, PCIE3 - PD
	#define DTRPCIVECT PCINT1_vect			// вектор обработчика
#endif


#if WITHELKEY
	/* сигналы подключения электронного ключа */
	#define ELKEY_TARGET_PORT PORTC		// входы процессора - сигналы электронного ключа
	#define ELKEY_TARGET_DDR DDRC		// переключение на ввод - сигналы электронного ключа
	#define ELKEY_TARGET_PIN PINC		// ввод - сигналы электронного ключа

	#define ELKEY_BIT_LEFT (1U << PC5)
	#define ELKEY_BIT_RIGHT (1U << PC6)
	// разрешение прерываний по нажатиям на контакты ключа
	// Микропроцессорах наподобии ATMega644
	#define PCMSK_PORT	PCMSK2			// PCMSK0 - PA, PCMSK1 - PB, PCMSK2 - PC, PCMSK3 - PD
	#define PCICR_BIT	(1U << PCIE2)	// PCIE0 - PA, PCIE1 - PB, PCIE2 - PC, PCIE3 - PD
	#define PCIVECT PCINT2_vect			// вектор обработчика
#endif

#define SIDETONE_TARGET_PORT	PORTD	/* Порт процессора, на котором находится вывод тона самоконтроля */
#define SIDETONE_TARGET_DDR		DDRD
#define SIDETONE_TARGET_BIT		(1U << PD7)

	#define TARGET_TWI_TWCK_PORT PORTC
	#define TARGET_TWI_TWCK_DDR DDRC
	#define TARGET_TWI_TWCK_PIN PINC
	#define TARGET_TWI_TWCK	(1U << PC0)	// DIP40 pin 22

	#define TARGET_TWI_TWD_PORT PORTC
	#define TARGET_TWI_TWD_DDR DDRC
	#define TARGET_TWI_TWD_PIN PINC
	#define TARGET_TWI_TWD	(1U << PC1)	// DIP40 pin 23

	#define SPI_TARGET_SCLK_PORT PORTB
	#define SPI_TARGET_SCLK_DDR DDRB
	#define SPI_TARGET_SCLK_PIN PINB

	#define	SPI_SCLK_BIT (1U << PB7)	// бит, через который идет синхронизация SPI

	#define SPI_TARGET_MOSI_PORT PORTB
	#define SPI_TARGET_MOSI_DDR DDRB
	#define SPI_TARGET_MOSI_PIN PINB

	#define	SPI_MOSI_BIT (1U << PB5)	// бит, через который идет вывод (или ввод в случае двунаправленного SPI).

	#define SPI_TARGET_MISO_PIN PINB
	#define SPI_TARGET_MISO_PORT PORTB
	#define SPI_TARGET_MISO_DDR DDRB

	#define	SPI_MISO_BIT (1U << PB6)	// бит, через который идет ввод с SPI.

	// в этой версии платы нет управления IORESET, IOUPDATE идёт с выхода дешифратора CS интерфейса SPI
	//#define SPI_IOUPDATE_BIT (1U << PB3)	// используются порты SPI

	// Набор определений для работы без внешнего дешифратора
	// Предполагается, что все чипселекты находятся на одном порту ввода-вывода
	// и их модификация возможна одной арифметической операцией.
	#define SPI_ALLCS_PORT PORTB
	#define SPI_ALLCS_DDR DDRB

	#if LCDMODE_HD44780
		// часть выводов (PB0, PB1) занята под управление дисплеем
		#define SPI_CSEL0 (1U << PB2)		// DDS1
		#define SPI_CSEL6 (1U << PB4)		// CTL REGISTER	

		// в данную маску должны быть включены все выводы чипселектов.
		#define SPI_ALLCS_BITS (SPI_CSEL0 | SPI_CSEL6)
		#define SPI_ALLCS_BITSNEG 0		// Выходы, активные при "1"

	#else

		#define SPI_CSEL4 (1U << PB0)	// LS020	
		#define SPI_CSEL1 (1U << PB1)	// NVRAM
		#define SPI_CSEL0 (1U << PB2)	// NC or DDS1 (DDS2 on CTLSTYLE_YUBOARD_V1)
		#define SPI_CSEL6 (1U << PB4)	// CTL REGISTER
		// в данную маску должны быть включены все выводы чипселектов.
		#define SPI_ALLCS_BITS (SPI_CSEL0 | SPI_CSEL1 | SPI_CSEL4 | SPI_CSEL6)
		#define SPI_ALLCS_BITSNEG 0		// Выходы, активные при "1"

	#endif

	// Variations of SPI_ALLCS_INITIALIZE
	#if SPI_ALLCS_BITS != 0
		#define SPI_ALLCS_INITIALIZE() do { \
			HARDWARE_OUTPUT_INITIALIZE(SPI_ALLCS_PORT, SPI_ALLCS_DDR, SPI_ALLCS_BITS, SPI_ALLCS_BITS ^ SPI_ALLCS_BITSNEG); \
			} while (0)
	#else /* SPI_ALLCS_BITS != 0 */
		#define SPI_ALLCS_INITIALIZE() do { \
			} while (0)
	#endif /* SPI_ALLCS_BITS != 0 */

	#define HARDWARE_SPI_CONNECT() do { \
		} while (0)

	#define HARDWARE_SPI_DISCONNECT() do { \
			SPCR = 0x00; \
		} while (0)

	// Separated MOSI and MISO signals, only supported if WITHSPIHW used
	#define SPIIO_INITIALIZE() do { \
			HARDWARE_INPUT_INITIALIZE(SPI_TARGET_MISO_PORT, SPI_TARGET_MISO_DDR, SPI_MISO_BIT, SPI_MISO_BIT); /* enable pull-up on MISO */ \
			/* PB4(~SS) должен быть выходом. SPI_NAEN_BIT - разрешение дешифратора. */ \
			HARDWARE_OUTPUT_INITIALIZE(SPI_TARGET_SCLK_PORT, SPI_TARGET_SCLK_DDR, SPI_SCLK_BIT, SPI_SCLK_BIT); \
			HARDWARE_OUTPUT_INITIALIZE(SPI_TARGET_MOSI_PORT, SPI_TARGET_MOSI_DDR, SPI_MOSI_BIT, SPI_MOSI_BIT); \
		} while (0)

#if KEYBOARD_USE_ADC == 0
	#define KBD_TARGET_PORT PORTA
	#define KBD_TARGET_DDR DDRA
	#define KBD_TARGET_PIN PINA

	#define KBD_MASK 0xfe	// все используемые биты
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

#endif /* ATMEGA_CPUSTYLE_V8S_H_INCLUDED */
