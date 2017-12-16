/* $Id$ */
/* board-specific CPU TQFP44 attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
//
// ATxmega128A4U, ATxmega32A4

#ifndef ATXMEGA32A4_CPUSTYLE_V1_H_INCLUDED
#define ATXMEGA32A4_CPUSTYLE_V1_H_INCLUDED 1

#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
//#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
//#define WITHSPISW 	1	/* Использование программного контроллера SPI */
////#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
//#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */
//#define WITHCPUADCHW 	1	/* использование ADC */

#define WITHUART1HW	1	/* Используется периферийный контроллер последовательного порта #1 */
//#define WITHUART2HW	1	/* Используется периферийный контроллер последовательного порта #2 */

//#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */
//#define WITHUSBHWVBUSSENSE	1	/* используется предопределенный вывод VBUS_SENSE */
//#define WITHUSBHWHIGHSPEED	1	/* Используется встроенная в процессор поддержка USB HS */
//#define WITHUSBHWHIGHSPEEDDESC	1	/* Требуется формировать дескрипторы как для HIGH SPEED */

//#define WITHCAT_CDC		1	/* использовать виртуальный воследовательный порт на USB соединении */
#define WITHCAT_USART1		1
#define WITHDEBUG_USART1	1
#define WITHMODEM_USART1	1
#define WITHNMEA_USART1		1

	//#define LS020_RESET_PORT_S(v)	do { PORTC.OUTSET = (v); } while (0)
	//#define LS020_RESET_PORT_C(v)	do { PORTC.OUTCLR = (v); } while (0)
	//#define LS020_RST	(PIN2_bm)			// 

	#define LS020_RS_PORT_S(v)	do { PORTC.OUTSET = (v); } while (0)
	#define LS020_RS_PORT_C(v)	do { PORTC.OUTCLR = (v); } while (0)
	#define LS020_RS	(PIN2_bm)			// PC2

	#define LS020_RS_DDR_S (PORTC.DIRSET)

	#define LS020_RS_INITIALIZE() do { \
			LS020_RS_PORT_S(LS020_RS); \
			LS020_RS_DDR_S = LS020_RS; \
		} while(0)

	#define LS020_RESET_INITIALIZE() do { \
			LS020_RESET_PORT_S(LS020_RST); \
			LS020_RESET_DDR_S = LS020_RST; \
		} while(0)

	// I2C interface pins
	#if WITHTWISW
	//	#error Why without WITHTWIHW?
	#endif
	#if ! WITHTWIHW
	//	#error Why without WITHTWIHW?
	#endif
	//#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */

	#define DISPLAY_BUS_DATA_GET() ((LCD_DATA_INPUT & LCD_DATAS_BITS) >> LCD_DATAS_BIT_LOW) /* получить данные с шины LCD */
	#define DISPLAY_BUS_DATA_SET(v) do { /* выдача данных (не сдвинуьых) */ \
			const portholder_t t = (portholder_t) (v) << LCD_DATAS_BIT_LOW; \
			LCD_DATA_PORT_S(LCD_DATAS_BITS & t); \
			LCD_DATA_PORT_C(LCD_DATAS_BITS & ~ t); \
		} while (0)

	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
#if WITHENCODER

	#define ENCODER_INPUT_PORT (PORTD.IN)

	#define ENCODER_SHIFT PIN0_bp
	#define ENCODER_BITS (PIN1_bm | PIN0_bm)	// PD1, PD0

	#define ENCODER_INITIALIZE() do { \
			PORTD.PIN0CTRL = (PORTD.PIN0CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
			PORTD.PIN1CTRL = (PORTD.PIN1CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
			PORTD.DIRCLR = ENCODER_BITS;	/* хотя используется периферия, PULL-UP только при программировании на ввод работает. */ \
		} while (0)

#endif

#if WITHTX
	/* переключение бита управления несущей вызывается из прерываний -
	 всем остальным, модифицирующим биты на этом порту, выполнять под запретом прерываний.
		*/

	#define TXPATH_TARGET_PORT_S(v)		do { PORTD.OUTSET = (v); } while (0)
	#define TXPATH_TARGET_PORT_C(v)		do { PORTD.OUTCLR = (v); } while (0)

	#if 1
		// Управление передатчиком - сигналы TXPATH_ENABLE (PA11) и TXPATH_ENABLE_CW (PA10) - активны при нуле на выходе.
		#define TXPATH_BIT_ENABLE_SSB		(PIN2_bm)	// PD2 - pin 22
		#define TXPATH_BIT_ENABLE_CW	(PIN3_bm)		// PD3 - pin 23
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
		#define TXPATH_BIT_GATE (PIN2_bm)	// PD2 выходной сигнал из процессора - управление передатчиком.
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

	// Биты управления трактом на передачу
	#if defined (TXPATH_BITS_ENABLE)
		// неактивное состояние - "1" в регистре данных..
		// Выходы запрограммированы на выход - "открытый сток"

		#define TXPATH_INITIALIZE() do { \
			PORTD.PIN2CTRL = (PORTD.PIN2CTRL & PORT_OPC_gm) | PORT_OPC_WIREDANDPULL_gc; \
			PORTD.PIN3CTRL = (PORTD.PIN3CTRL & PORT_OPC_gm) | PORT_OPC_WIREDANDPULL_gc; \
			PORTD.OUTSET = (TXPATH_BIT_ENABLE_SSB | TXPATH_BIT_ENABLE_CW); \
			PORTD.OUTSET = (TXPATH_BIT_ENABLE_SSB | TXPATH_BIT_ENABLE_CW);	/* бит выхода манипуляции - открытый сток */ \
		} while (0)

	#elif defined (TXPATH_BIT_GATE)

		#define TXPATH_INITIALIZE() do { \
			PORTD.PIN2CTRL = (PORTD.PIN2CTRL & PORT_OPC_gm) | PORT_OPC_TOTEM_gc; \
			PORTD.OUTCLR = TXPATH_BIT_GATE;	/* неактивное состояние - "0" */ \
			PORTD.DIRSET = TXPATH_BIT_GATE;	/* бит выхода манипуляции */ \
		} while (0)
	
	#elif TXPATH_BIT_GATE_RX
		#error TODO: complete TXPATH_BIT_GATE_RX support

		#define TXPATH_INITIALIZE() do { \
		} while (0)
	#endif

	// разрешение прерываний по нажатиям на контакты ключа
	// Микропроцессорах наподобии ATMega644
	//#define PCMSK_PORT	PCMSK2			// PCMSK0 - PA, PCMSK1 - PB, PCMSK2 - PC, PCMSK3 - PD
	//#define PCICR_BIT	(1U << PCIE2)	// PCIE0 - PA, PCIE1 - PB, PCIE2 - PC, PCIE3 - PD
	//#define PCIVECT PCINT2_vect			// вектор обработчика

	// процессор формирует звук самоконтроля при передаче телеграфом
	// генерация сигнала самоконтроля на PD7(OC2)

	//#define PTT_TARGET_PORT (PORTD.OUT)	
	#define PTT_TARGET_PINCTRL (PORTD.PIN5CTRL)	// write PORT_OPC_PULLUP_gc
	#define PTT_TARGET_DDR_C (PORTD.DIRCLR)
	#define PTT_TARGET_PIN (PORTD.IN)
	#define PTT_BIT_PTT (PIN5_bm)				// PD7

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() \
		do { \
		/* хотя используется периферия, PULL-UP только при программировании на ввод работает. */ \
		PTT_TARGET_PINCTRL = (PTT_TARGET_PINCTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc;	/* включть pull-up на этот вход. */ \
		PTT_TARGET_DDR_C = PTT_BIT_PTT;	/* бит входа PTT */ \
		} while (0)


#endif /* WITHTX */

#if WITHCAT || WITHNMEA
	// Сигналы переключения на передачу и манипуляции через CAT интерфейс.
	#define FROMCAT_TARGET_PIN_RTS		(PORTE.IN)
	#define FROMCAT_BIT_RTS			(PIN1_bm)	/* PE1 сигнал RTS от FT232RL	*/
	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	#define FROMCAT_TARGET_PIN_DTR		(PORTE.IN)
	#define FROMCAT_BIT_DTR			(PIN0_bm)	/* PE0 сигнал DTR от FT232RL	*/

	#define FROMCAT_RTS_INITIALIZE() do { \
			PORTE.PIN1CTRL = (PORTE.PIN1CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc;	/* включть pull-up на этот вход. */ \
			PORTE.DIRCLR = FROMCAT_BIT_RTS;	/* бит входа RTS */ \
		} while (0)

	#define FROMCAT_DTR_INITIALIZE() do { \
			PORTE.PIN0CTRL = (PORTE.PIN0CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc;	/* включть pull-up на этот вход. */ \
			PORTE.DIRCLR = FROMCAT_BIT_DTR;	/* бит входа DTR */ \
		} while (0)
	
#endif

#if WITHELKEY

	#define ELKEY_TARGET_PIN (PORTD.IN)		// ввод - сигналы электронного ключа
	#define ELKEY_BIT_RIGHT (PIN7_bm)		// PD7
	#define ELKEY_BIT_LEFT (PIN6_bm)		// PD6

	#define ELKEY_INITIALIZE() do { \
		PORTD.DIRCLR = ELKEY_BIT_LEFT;	/* бит входа левого контакта манипулятора */ \
		PORTD.DIRCLR = ELKEY_BIT_RIGHT;	/* бит входа правого контакта манипулятора */ \
		PORTD.PIN6CTRL = (PORTD.PIN6CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
		PORTD.PIN7CTRL = (PORTD.PIN7CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
		} while (0)
#endif

//#define SIDETONE_TARGET_PORT	(PORTD.OUT)	/* Порт процессора, на котором находится вывод тона самоконтроля */
#define SIDETONE_TARGET_DDR		(PORTD.DIR)
#define SIDETONE_TARGET_BIT		(PIN4_bm)		// PD4

#if 1 // WITHTWISW
	#define TARGET_TWI_PORT_S(v)	do { PORTC.OUTSET = (v); } while (0)
	#define TARGET_TWI_PORT_C(v)	do { PORTC.OUTCLR = (v); } while (0)
	#define TARGET_TWI_DDR_S (PORTC.DIRSET)
	#define TARGET_TWI_DDR_C (PORTC.DIRCLR)
	#define TARGET_TWI_PIN (PORTC.IN)
	#define TARGET_TWI_TWCK	(PIN1_bm)			// SCL - PC1
	#define TARGET_TWI_TWD	(PIN0_bm)			// SDA - PC0

	// Инициализация битов портов ввода-вывода для программной реализации I2C
	#define	TWISOFT_INITIALIZE() \
		do { \
			/* программирование выводов, управляющих I2C */ \
			TARGET_TWI_PORT_S(TARGET_TWI_TWCK | TARGET_TWI_TWD); \
			TARGET_TWI_DDR_S = (TARGET_TWI_TWCK | TARGET_TWI_TWD);	/* На вызоде - "1" */ \
			PORTC.PIN1CTRL = PORT_OPC_WIREDANDPULL_gc;	/* Wired AND w/ Pull-up */ \
			PORTC.PIN0CTRL = PORT_OPC_WIREDANDPULL_gc;	/* Wired AND w/ Pull-up */ \
		} while (0) 

	// Программирование выводов для аппаратной реализации I2C не требуется.
	#define	TWIHARD_INITIALIZE() do { \
		} while (0) 

	#define TARGET_TWI	TWIC	/* TWI порта C */

#endif

#if LCDMODE_UC1608
	#define UC1608_CSP_PORT_S(v)	do { PORTC.OUTSET = (c); } while (0)
	#define UC1608_CSP_PORT_C(v)	do { PORTC.OUTCLR = (c); } while (0)
	//#define UC1608_CSP_DDR_C (PORTC.DIRCLR)
	#define UC1608_CSP_DDR_S (PORTC.DIRSET)
	#define UC1608_CSP 	(PIN3_bm)			// PC3 - DFU PIN - так же используется как CSP для uc1608
	#define SPI_CSEL255	255					// по этому чипселекту выбираем положительным сигналом

	#define UC1608_CSP_INITIALIZE() do { \
			UC1608_CSP_PORT_C(UC1608_CSP);	/* неактивное состояние */ \
			UC1608_CSP_DDR_S = UC1608_CSP;	/* Выход */ \
		} while (0)
#endif

	#define TARGET_SPI	PORTC

	#define SPI_TARGET_SCLK_PORT (TARGET_SPI.OUT)
	#define SPI_TARGET_SCLK_PORT_S(v)	do { TARGET_SPI.OUTSET = (v); } while (0)
	#define SPI_TARGET_SCLK_PORT_C(v)	do { TARGET_SPI.OUTCLR = (v); } while (0)
	#define SPI_TARGET_SCLK_PORT_T (TARGET_SPI.OUTTGL)	/* при записи в этот порт происходит инверсия состояния выхода */
	
	#define SPI_TARGET_SCLK_DDR (TARGET_SPI.DIR)
	#define SPI_TARGET_SCLK_DDR_S (TARGET_SPI.DIRSET)
	#define SPI_TARGET_SCLK_DDR_C (TARGET_SPI.DIRCLR)
	//#define SPI_TARGET_SCLK_PIN (TARGET_SPI.IN)

	#define SPI_TARGET_MOSI_PORT (TARGET_SPI.OUT)
	#define SPI_TARGET_MOSI_PORT_S(v)	do { TARGET_SPI.OUTSET = (v); } while (0)
	#define SPI_TARGET_MOSI_PORT_C(v)	do { TARGET_SPI.OUTCLR = (v); } while (0)
	#define SPI_TARGET_MOSI_PORT_T (TARGET_SPI.OUTTGL)	/* при записи в этот порт происходит инверсия состояния выхода */
	
	//#define SPI_TARGET_MOSI_DDR (TARGET_SPI.DIR)
	#define SPI_TARGET_MOSI_DDR_S (TARGET_SPI.DIRSET)
	//#define SPI_TARGET_MOSI_DDR_C (TARGET_SPI.DIRCLR)
	//#define SPI_TARGET_MOSI_PIN (TARGET_SPI.IN)

	#define	SPI_SCLK_BIT (PIN7_bm)	// PC7 бит, через который идет синхронизация SPI
	#define	SPI_MOSI_BIT (PIN5_bm)	// PC5 бит, через который идет вывод (или ввод в случае двунаправленного SPI).

	#define SPI_TARGET_MISO_PIN (TARGET_SPI.IN)
	//#define SPI_TARGET_MISO_PORT (TARGET_SPI.OUT)
	#define SPI_TARGET_MISO_PINCTRL (TARGET_SPI.PIN6CTRL)	// write PORT_OPC_PULLUP_gc
	//#define SPI_TARGET_MISO_DDR (TARGET_SPI.DIR)
	#define SPI_TARGET_MISO_DDR_C (TARGET_SPI.DIRCLR)

#if SPI_BIDIRECTIONAL

	#define SPIIO_MOSI_TO_INPUT() do { \
		SPI_TARGET_MOSI_DDR_C = SPI_MOSI_BIT;	/* переключить порт на чтение с выводов */ \
		} while (0)
	#define SPIIO_MOSI_TO_OUTPUT() do { \
		SPI_TARGET_MOSI_DDR_S = SPI_MOSI_BIT;	/* открыть выходы порта */ \
		} while (0)

	#define SPIIO_INITIALIZE() do { \
			/* PB4(~SS) должен быть выходом. SPI_NAEN_BIT - разрешение дешифратора. */ \
			SPI_TARGET_SCLK_PORT_S(SPI_SCLK_BIT); \
			SPI_TARGET_SCLK_DDR_S = SPI_SCLK_BIT; \
			SPI_TARGET_MOSI_PORT_S(SPI_MOSI_BIT); \
			SPI_TARGET_MOSI_DDR_S = SPI_MOSI_BIT; \
			TARGETSOFT_SPI.DIRSET = SPI_NAEN_BIT;		/* Это вывод CS -  всегда должен быть выходом при использовангии аппаратного SPI */ \
		} while (0)

#else /* SPI_BIDIRECTIONAL */

	#define	SPI_MISO_BIT (PIN6_bm)	// PC6 бит, через который идет ввод с SPI.

	#define SPIIO_INITIALIZE() do { \
			SPI_TARGET_MISO_DDR_C  = SPI_MISO_BIT; \
			/* PB4(~SS) должен быть выходом. SPI_NAEN_BIT - разрешение дешифратора. */ \
			SPI_TARGET_SCLK_PORT_S(SPI_SCLK_BIT); \
			SPI_TARGET_SCLK_DDR_S = SPI_SCLK_BIT; \
			SPI_TARGET_MOSI_PORT_S(SPI_MOSI_BIT); \
			SPI_TARGET_MOSI_DDR_S = SPI_MOSI_BIT; \
			TARGETSOFT_SPI.DIRSET = SPI_NAEN_BIT;		/* Это вывод CS -  всегда должен быть выходом при использовангии аппаратного SPI */ \
		} while (0)

#endif /* SPI_BIDIRECTIONAL */



	// в этой версии платы нет управления IORESET, IOUPDATE идёт с выхода дешифратора CS интерфейса SPI
	//#define SPI_IOUPDATE_BIT (1U << PB3)	// используются порты SPI

	// Есть внешний дешифратор на шине адреса SPI 
	#define SPI_ADDRESS_PORT (PORTB.OUT)
	#define SPI_ADDRESS_PORT_S(v)	do { PORTB.OUTSET = (v); } while (0)
	#define SPI_ADDRESS_PORT_C(v)	do { PORTB.OUTCLR = (v); } while (0)
	//#define SPI_ADDRESS_DDR_S (PORTB.DIRSET)
	#define SPI_ADDRESS_DDR (PORTB.DIR)
	//#define SPI_ADDRESS_PIN (PORTB.IN)

	#define SPI_A0 (PIN1_bm)			// PB1 биты адреса для дешифратора SPI
	#define SPI_A1 (PIN2_bm)			// PB2
	#define SPI_A2 (PIN3_bm)			// PB3

	#define TARGETHARD_SPI	SPIC	/* SPI порта C */
	#define TARGETSOFT_SPI	PORTC	/* SPI порта C */

	#define SPI_NAEN_PORT (TARGETSOFT_SPI.OUT)
	#define SPI_NAEN_PORT_S(v)	do { TARGETSOFT_SPI.OUTSET = (v); } while (0)
	#define SPI_NAEN_PORT_C(v)	do { TARGETSOFT_SPI.OUTCLR = (v); } while (0)
	#define SPI_NAEN_DDR (TARGETSOFT_SPI.DIR)
	#define SPI_NAEN_DDR_S (TARGETSOFT_SPI.DIRSET)
	#define SPI_NAEN_DDR_C (TARGETSOFT_SPI.DIRCLR)

	#define SPI_NAEN_BIT	PIN4_bm	// PC4 "0" - разрешение выхода CS
	#define SPI_ALLCS_BITS	0		// требуется для указания того, что работа с прямым выбором CS (без дешифратора) не требуется
	#define SPI_ALLCS_BITSNEG 0		// Выходы, активные при "1"

	// Variations of SPI_ALLCS_INITIALIZE
	#if SPI_ALLCS_BITS != 0
		#define SPI_ALLCS_INITIALIZE() do { \
			SPI_ALLCS_PORT |= (SPI_ALLCS_BITS); \
			SPI_ALLCS_DDR |= (SPI_ALLCS_BITS); \
			} while (0)
	#else /* SPI_ALLCS_BITS != 0 */
		#define SPI_ALLCS_INITIALIZE() do { \
			} while (0)
	#endif /* SPI_ALLCS_BITS != 0 */

	#define HARDWARE_SPI_CONNECT() do { \
		} while (0)

	#define HARDWARE_SPI_DISCONNECT() do { \
			TARGETHARD_SPI.CTRL = 0x00; \
		} while (0)


	#if KEYBOARD_USE_ADC
		#define HARDWARE_KBD_INITIALIZE() do { \
			} while (0)
	#else
		//#define KBD_TARGET_PORT PORTA.OUT
		#define KBD_TARGET_PIN PORTA.IN

		#define KBD_MASK (PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm)	// все используемые биты

		#define HARDWARE_KBD_INITIALIZE() do { \
			PORTA.PIN1CTRL = (PORTA.PIN1CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
			PORTA.PIN2CTRL = (PORTA.PIN2CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
			PORTA.PIN3CTRL = (PORTA.PIN3CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
			PORTA.PIN4CTRL = (PORTA.PIN4CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
			PORTA.PIN5CTRL = (PORTA.PIN5CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
			PORTA.PIN6CTRL = (PORTA.PIN6CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
			PORTA.PIN7CTRL = (PORTA.PIN7CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
			PORTA.DIR &= ~ KBD_MASK;	/* define these bits as inputs - enable pull-up */ \
			} while (0)
	#endif

	#define CS4272_RESET_INITIALIZE() do { \
			TARGET_CS4272_RESET_PORT_S(TARGET_CS4272_RESET_BIT); \
			TARGET_CS4272_RESET_DDR_S(TARGET_CS4272_RESET_BIT); \
		} while (0)

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

#endif /* ATXMEGA32A4_CPUSTYLE_V1_H_INCLUDED */
