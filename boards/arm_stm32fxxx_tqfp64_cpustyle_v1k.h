/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Синтезатор с процессором STM32F401RB

#ifndef ARM_STM32FXXX_TQFP64_CPUSTYLE_V1K_H_INCLUDED
#define ARM_STM32FXXX_TQFP64_CPUSTYLE_V1K_H_INCLUDED 1

//#define HARDWARE_ARM_USEUSART0 1		// US0: PA5/PA6 pins - WITHUART1HW
//#define HARDWARE_ARM_USEUSART1 1		// US1: PA21/PA22 pins = WITHUART2HW

#define WITHSPI16BIT	1		/* возможно использование 16-ти битных слов при обмене по SPI */
#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
#define WITHSPISW 	1	/* Использование программного управления SPI. Нельзя убирать эту строку - требуется явное отключение из-за конфликта с I2C */

//#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */
#define WITHCPUADCHW 	1	/* использование ADC */

#if 1
	#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */
	#define WITHUSBHWVBUSSENSE	1	/* используется предопределенный вывод VBUS_SENSE */
	//#define WITHUSBHWHIGHSPEED	1	/* Используется встроенная в процессор поддержка USB HS */
	//#define WITHUSBHWHIGHSPEEDDESC	1	/* Требуется формировать дескрипторы как для HIGH SPEED */
	//#define WITHUART2HW	1	/* Используется периферийный контроллер последовательного порта #2 */
	#define WITHUSBHW_DEVICE	USB_OTG_FS	/* на этом устройстве поддерживается функциональность DEVUCE	*/

	//#define WITHUSBUAC		1	/* использовать виртуальную звуковую плату на USB соединении */
	#define WITHUSBCDC		1	/* ACM использовать виртуальный последовательный порт на USB соединении */
	//#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
	//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */

	#define WITHCAT_CDC		1	/* использовать виртуальный воследовательный порт на USB соединении */
	#define WITHMODEM_CDC	1

	#define WITHDEBUG_USART2	1
	#define WITHNMEA_USART2		1
#else

	#define WITHUART1HW	1	/* Используется периферийный контроллер последовательного порта #1 */

	#define WITHCAT_USART1		1
	#define WITHDEBUG_USART1	1
	#define WITHMODEM_USART1	1
	#define WITHNMEA_USART1		1

	//#define WITHUART2HW	1	/* Используется периферийный контроллер последовательного порта #2 */
#endif

#define LS020_RS_INITIALIZE() \
	do { \
		arm_hardware_pioc_outputs2m(LS020_RS, LS020_RS); \
	} while (0)

#define LS020_RESET_INITIALIZE() \
	do { \
		arm_hardware_pioc_outputs2m(LS020_RST, LS020_RST); \
	} while (0)


#if LCDMODE_SPI_RN
	// эти контроллеры требуют только RESET
	#define LS020_RESET_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RST			(1u << 8)			// D6 signal in HD44780 socket

#elif LCDMODE_SPI_NA
	// Эти контроллеры требуют только RS
	#define LS020_RS_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RS			(1u << 9)			// D7 signal in HD44780 socket

#elif LCDMODE_SPI_RA
	// Эти контроллеры требуют RESET и RS
	#define LS020_RESET_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RST			(1u << 8)			// D6 signal in HD44780 socket

	#define LS020_RS_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RS			(1u << 9)			// D7 signal in HD44780 socket

#elif LCDMODE_HD44780 && (LCDMODE_SPI == 0)

	// Выводы подключения ЖКИ индикатора WH2002 или аналогичного HD44780.
	#define LCD_DATA_INPUT			(GPIOC->IDR)

	//#define LCD_DIRECTION_PORT			GPIOA->PIO_OSR
	//#define LCD_DATA_DIRECTION_S		GPIOA->PIO_OER
	//#define LCD_DATA_DIRECTION_C		GPIOA->PIO_ODR

	// E (enable) bit
	//#define LCD_STROBE_DIRECTION_S		GPIOA->PIO_OER
	//#define LCD_STROBE_DIRECTION_C		GPIOA->PIO_ODR
	#define LCD_STROBE_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LCD_STROBE_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)

	// RS & WE bits
	#define LCD_RS_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LCD_RS_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LCD_WE_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LCD_WE_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)

	#define LCD_STROBE_BIT			(1u << 13)

	#define WRITEE_BIT				(1u << 11)
	//#define WRITEE_BIT_ZERO				GPIO_ODR_ODR11
	
	#define ADDRES_BIT				(1u << 10)

	#define LCD_DATAS_BITS			((1u << 9) | (1u << 8) | (1u << 7) | (1u << 6))
	#define LCD_DATAS_BIT_LOW		6		// какой бит данных младший в слове считанном с порта

	#define DISPLAY_BUS_DATA_GET() ((LCD_DATA_INPUT & LCD_DATAS_BITS) >> LCD_DATAS_BIT_LOW) /* получить данные с шины LCD */
	#define DISPLAY_BUS_DATA_SET(v) do { /* выдача данных (не сдвинуьых) */ \
			const portholder_t t = (portholder_t) (v) << LCD_DATAS_BIT_LOW; \
			GPIOC->BSRR = BSRR_S(t & LCD_DATAS_BITS) | BSRR_C(~ t & LCD_DATAS_BITS); \
			__DSB(); \
		} while (0)

	/* инициализация управляющих выходов процессора для управления HD44780 - полный набор выходов */
	#define LCD_CONTROL_INITIALIZE() \
		do { \
			arm_hardware_pioc_outputs2m(LCD_STROBE_BIT | WRITEE_BIT | ADDRES_BIT, 0); \
		} while (0)
	/* инициализация управляющих выходов процессора для управления HD44780 - WE=0 */
	#define LCD_CONTROL_INITIALIZE_WEEZERO() \
		do { \
			arm_hardware_pioc_outputs2m(LCD_STROBE_BIT | WRITEE_BIT_ZERO | ADDRES_BIT, 0); \
		} while (0)
	/* инициализация управляющих выходов процессора для управления HD44780 - WE отсутствует - сигнал к индикатору заземлён */
	#define LCD_CONTROL_INITIALIZE_WEENONE() \
		do { \
			arm_hardware_pioc_outputs2m(LCD_STROBE_BIT | ADDRES_BIT, 0); \
		} while (0)

	#define LCD_DATA_INITIALIZE_READ() \
		do { \
			arm_hardware_pioc_inputs(LCD_DATAS_BITS);	/* переключить порт на чтение с выводов */ \
		} while (0)

	#define LCD_DATA_INITIALIZE_WRITE(v) \
		do { \
			arm_hardware_pioc_outputs2m(LCD_DATAS_BITS, (v) << LCD_DATAS_BIT_LOW);	/* открыть выходы порта */ \
		} while (0)

#endif

#if WITHENCODER

	// Выводы подключения енкодера
	// Encoder inputs: PC12 - PHASE A, PC11 = PHASE B

	#define ENCODER_INPUT_PORT			(GPIOC->IDR)
	#define ENCODER_BITS ((1u << 12) | (1u << 11))
	#define ENCODER_SHIFT 11

	#define ENCODER_INITIALIZE() \
		do { \
			arm_hardware_pioc_inputs(ENCODER_BITS); \
			arm_hardware_pioc_updown(ENCODER_BITS, 0); \
			arm_hardware_pioc_onchangeinterrupt(ENCODER_BITS, ENCODER_BITS, ENCODER_BITS, ARM_OVERREALTIME_PRIORITY); \
		} while (0)

#endif

/* Распределение битов в ARM контроллерах */



#if (WITHCAT && WITHCAT_USART1)
	// CAT control lines
	#define FROMCAT_TARGET_PIN_RTS		(GPIOA->IDR) // was PINA 
	#define FROMCAT_BIT_RTS				(1u << 11)	/* сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	#define FROMCAT_TARGET_PIN_DTR		(GPIOA->IDR) // was PINA 
	#define FROMCAT_BIT_DTR				(1u << 12)	/* сигнал DTR от FT232RL	*/

	/* манипуляция от порта RS-232 */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
			arm_hardware_pioa_inputs(FROMCAT_BIT_DTR); \
			arm_hardware_pioa_updown(FROMCAT_BIT_DTR, 0); \
		} while (0)

	/* переход на передачу от порта RS-232 */
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
			arm_hardware_pioa_inputs(FROMCAT_BIT_RTS); \
			arm_hardware_pioa_updown(FROMCAT_BIT_RTS, 0); \
		} while (0)

	/* сигнал PPS от GPS/GLONASS/GALILEO модуля */
	#define NMEA_INITIALIZE() \
		do { \
			arm_hardware_pioa_inputs(FROMCAT_BIT_DTR); \
			arm_hardware_pioa_onchangeinterrupt(FROMCAT_BIT_DTR, FROMCAT_BIT_DTR, FROMCAT_BIT_DTR, ARM_SYSTEM_PRIORITY); \
		} while (0)

	#define HARDWARE_USART1_INITIALIZE() do { \
			arm_hardware_pioa_altfn2((1U << 9) | (1U << 10), AF_USART1); /* PA9: TX DATA line (2 MHz), PA10: RX data line */ \
			arm_hardware_pioa_updown((1U << 10), 0);	/* PA10: pull-up RX data */ \
		} while (0)
#endif

#if (WITHCAT && WITHCAT_CDC)

	/* манипуляция от порта RS-232 */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
		} while (0)

	/* переход на передачу от порта RS-232 */
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
		} while (0)

#endif /* (WITHCAT && WITHCAT_CDC) */


#if WITHTX

	// txpath outputs

	//#define TXPATH_TARGET_DDR_S			GPIOF->PIO_OER	// was DDRA
	//#define TXPATH_TARGET_DDR_C		GPIOD->PIO_ODR	// was DDRA
	#define TXPATH_TARGET_PORT_S(v)		do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TXPATH_TARGET_PORT_C(v)		do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
	#if 0

		// Управление передатчиком - сигналы TXPATH_ENABLE (PC12) и TXPATH_ENABLE_CW (PC13) - активны при нуле на выходе.
		#define TXPATH_BIT_ENABLE_SSB		(1u << 12)
		#define TXPATH_BIT_ENABLE_CW		(1u << 13)
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
		#define TXGFV_RX		TXPATH_BIT_GATE_RX
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

	#define PTT_TARGET_PIN				(GPIOC->IDR) // was PINA 
	#define PTT_BIT_PTT					(1u << 5)

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() \
		do { \
			arm_hardware_pioc_inputs(PTT_BIT_PTT); \
			arm_hardware_pioc_updown(PTT_BIT_PTT, 0); \
		} while (0)

#endif /* WITHTX */


#if WITHELKEY
	// Electronic key inputs
	//#define ELKEY_TARGET_PORT			GPIOF->PIO_ODSR
	#define ELKEY_TARGET_PIN			(GPIOC->IDR)	// was PINA 
	#define ELKEY_BIT_LEFT				(1u << 6)	//GPIO_ODR_ODR14
	#define ELKEY_BIT_RIGHT				(1u << 7)	//GPIO_ODR_ODR15

	#define ELKEY_INITIALIZE() \
		do { \
			arm_hardware_pioc_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
			arm_hardware_pioc_updown(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, 0); \
		} while (0)

#endif

	#define SPI_IOUPDATE_PORT_C(v)	do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define SPI_IOUPDATE_PORT_S(v)	do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define SPI_IOUPDATE_BIT		(1u << 13)	// PC13

	// Набор определений для работы без внешнего дешифратора
	#define SPI_ALLCS_PORT_S(v)	do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define SPI_ALLCS_PORT_C(v)	do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)

	#define SPI_CSEL0	(1u << 0)	// DDS1
	#define SPI_CSEL1	(1u << 1)	// DDS2
	#define SPI_CSEL2	(1u << 2)	// EXT
	#define SPI_CSEL3	(1u << 3)	// CSCTL
	#define SPI_CSEL4	(1u << 4)	// NVRAM
	// Здесь должны быть перечислены все биты формирования CS  устройстве.
	#define SPI_ALLCS_BITS (SPI_CSEL0 | SPI_CSEL1 | SPI_CSEL2 | SPI_CSEL3 | SPI_CSEL4)
	#define SPI_ALLCS_BITSNEG 0

	/* инициализация лиий выбора периферийных микросхем */
	#define SPI_ALLCS_INITIALIZE() \
		do { \
			arm_hardware_pioc_outputs2m(SPI_ALLCS_BITS, SPI_ALLCS_BITS ^ SPI_ALLCS_BITSNEG); \
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
			arm_hardware_pioc_outputs2m(SPI_IOUPDATE_BIT, SPI_IOUPDATE_BIT); \
		} while (0)
	/* инициализация сигнала IOUPDATE на DDS */
	/* начальное состояние = 0 */
	#define SPI_IORESET_INITIALIZE() \
		do { \
		} while (0)

// SPI control pins
// SPI1 hardware used.

// MOSI & SCK port
// STM32F303: SPI1_NSS can be placed on PA4 or PA15
#define SPI_TARGET_SCLK_PORT_C(v)	do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_TARGET_SCLK_PORT_S(v)	do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_TARGET_MOSI_PORT_C(v)	do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_TARGET_MOSI_PORT_S(v)	do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)

#define	SPI_SCLK_BIT			(1U << 3)	// * PB3 бит, через который идет синхронизация SPI
#define	SPI_MOSI_BIT			(1U << 5)	// * PB5 бит, через который идет вывод (или ввод в случае двунаправленного SPI).

#define SPI_TARGET_MISO_PIN		(GPIOB->IDR)		// was PINA 
#define	SPI_MISO_BIT			(1U << 4)	// * PB4 бит, через который идет ввод с SPI.

	#define SPIIO_INITIALIZE() do { \
			arm_hardware_piob_outputs(SPI_MOSI_BIT | SPI_SCLK_BIT, SPI_MOSI_BIT | SPI_SCLK_BIT); \
			arm_hardware_piob_inputs(SPI_MISO_BIT); \
		} while (0)

#define SIDETONE_TARGET_BIT		(1u << 8)	// output TIM4_CH3 (PB8, base mapping)
//#define SIDETONE_TARGET_BIT		GPIO_ODR_ODR12	// output TIM4_CH1 (PD12, alt mapping)

#if 1 // WITHTWISW
	#define TARGET_TWI_TWCK_PORT_C(v) do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_TWI_TWD_PORT_C(v) do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v) do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_TWI_TWCK_PIN		(GPIOB->IDR)
	#define TARGET_TWI_TWD_PIN		(GPIOB->IDR)
	#define TARGET_TWI_TWCK		(1u << 6)		// * PB6
	#define TARGET_TWI_TWD		(1u << 7)		// * PB6

	// Инициализация битов портов ввода-вывода для программной реализации I2C
	#define	TWISOFT_INITIALIZE() do { \
			arm_hardware_piob_opendrain(TARGET_TWI_TWCK | TARGET_TWI_TWD, TARGET_TWI_TWCK | TARGET_TWI_TWD); \
		} while (0) 
	// Инициализация битов портов ввода-вывода для аппаратной реализации I2C
	// присоединение выводов к периферийному устройству
	#if CPUSTYLE_STM32F1XX

		#define	TWIHARD_INITIALIZE() do { \
				arm_hardware_piob_periphopendrain_altfn2(TARGET_TWI_TWCK | TARGET_TWI_TWD, 255);	/* AF=4 */ \
			} while (0) 

	#elif CPUSTYLE_STM32F30X

		#define	TWIHARD_INITIALIZE() do { \
				arm_hardware_piob_periphopendrain_altfn2(TARGET_TWI_TWCK | TARGET_TWI_TWD, 4 /* AF_I2C1 */);	/* AF=4 */ \
			} while (0) 

	#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

		#define	TWIHARD_INITIALIZE() do { \
				arm_hardware_piob_periphopendrain_altfn2(TARGET_TWI_TWCK | TARGET_TWI_TWD, AF_I2C1);	/* AF=4 */ \
			} while (0) 

	#endif
#endif // WITHTWISW

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


	#define HARDWARE_SPI_CONNECT() do { \
			arm_hardware_piob_altfn50(SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT, AF_SPI1); /* В этих процессорах и входы и выходы перекдючаются на ALT FN */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT() do { \
			arm_hardware_piob_outputs(SPI_SCLK_BIT | SPI_MOSI_BIT, SPI_SCLK_BIT | SPI_MOSI_BIT); /* connect back to GPIO */ \
			arm_hardware_piob_inputs(SPI_MISO_BIT); /* connect back to GPIO */ \
		} while (0)
	#define HARDWARE_SPI_CONNECT_MOSI() do { \
			arm_hardware_piob_altfn50(SPI_MOSI_BIT, AF_SPI1);	/* PIO disable for MOSI bit (SD CARD read support) */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
			arm_hardware_piob_outputs(SPI_MOSI_BIT, SPI_MOSI_BIT);	/* PIO enable for MOSI bit (SD CARD read support)  */ \
		} while (0)
	#define HARDWARE_SIDETONE_INITIALIZE() do { \
		arm_hardware_piob_altfn2(SIDETONE_TARGET_BIT, AF_TIM4);	/* output TIM4_CH3 (PB8, base mapping) */ \
		} while (0)

	#if KEYBOARD_USE_ADC
		#define HARDWARE_KBD_INITIALIZE() do { \
			} while (0)
	#else
		#define HARDWARE_KBD_INITIALIZE() do { \
			arm_hardware_pioa_inputs(KBD_MASK); \
			} while (0)
	#endif

#endif /* ARM_STM32FXXX_TQFP64_CPUSTYLE_V1K_H_INCLUDED */
