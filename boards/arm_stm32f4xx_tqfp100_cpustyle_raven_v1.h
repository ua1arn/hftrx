/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Трансивер с 12-кГц DSP обработкой "Воронёнок-2" на процессоре STM32F429 с кодеками TLV320AIC23B и CS4272

#ifndef ARM_STM32F4XX_TQFP100_CPUSTYLE_RAVEN_V1_H_INCLUDED
#define ARM_STM32F4XX_TQFP100_CPUSTYLE_RAVEN_V1_H_INCLUDED 1

//#define HARDWARE_ARM_USEUSART0 1		// US0:
//#define HARDWARE_ARM_USEUSART1 1		// US1: PA9/PA10 pins

#define WITHSPI16BIT	1		/* возможно использование 16-ти битных слов при обмене по SPI */
#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
#define WITHSPISW 	1	/* Использование программного управления SPI. Нельзя убирать эту строку - требуется явное отключение из-за конфликта с I2C */

#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
//#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */

#define WITHI2SHW	1	/* Использование I2S - аудиокодек	*/
#define WITHSAI1HW	1	/* Использование SAI1 - FPGA или IF codec	*/

#define WITHCPUDACHW	1	/* использование DAC */
#define WITHCPUADCHW 	1	/* использование ADC */

#define WITHUART1HW	1	/* Используется периферийный контроллер последовательного порта #1 */
//#define WITHUART2HW	1	/* Используется периферийный контроллер последовательного порта #2 */

//#define WITHCAT_CDC		1	/* использовать виртуальный воследовательный порт на USB соединении */
#define WITHCAT_USART1		1
#define WITHDEBUG_USART1	1
#define WITHMODEM_USART1	1
#define WITHNMEA_USART1		1

//#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */
//#define WITHUSBHWVBUSSENSE	1	/* используется предопределенный вывод VBUS_SENSE */
//#define WITHUSBHWHIGHSPEED	1	/* Используется встроенная в процессор поддержка USB HS */
//#define WITHUSBHWHIGHSPEEDDESC	1	/* Требуется формировать дескрипторы как для HIGH SPEED */
//#define WITHUSBHW_DEVICE	USB_OTG_FS	/* на этом устройстве поддерживается функцилнальность DEVUCE	*/

//#define BSRR_S(v) ((v) * GPIO_BSRR_BS_0)	/* Преобразование значения для установки бита в регистре */
//#define BSRR_C(v) ((v) * GPIO_BSRR_BR_0)	/* Преобразование значения для сброса бита в регистре */

#define LS020_RS_INITIALIZE() \
	do { \
		arm_hardware_pioe_outputs2m(LS020_RS, LS020_RS); \
	} while (0)

#define LS020_RESET_INITIALIZE() \
	do { \
		arm_hardware_pioe_outputs2m(LS020_RST, LS020_RST); \
	} while (0)

#if LCDMODE_SPI_NA
	// эти контроллеры требуют только RS

	#define LS020_RS_PORT_S(v)		do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RS			(1u << 11)			// * PE11

#elif LCDMODE_SPI_RN
	// эти контроллеры требуют только RESET

	#define LS020_RESET_PORT_S(v)		do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RST			(1u << 10)			// * PE10 D6 signal in HD44780 socket

#elif LCDMODE_SPI_RA
	// Эти контроллеры требуют RESET и RS

	#define LS020_RS_PORT_S(v)		do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RS			(1u << 11)			// * PE11

	#define LS020_RESET_PORT_S(v)		do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RST			(1u << 10)			// * PE10 D6 signal in HD44780 socket

#elif LCDMODE_HD44780 && (LCDMODE_SPI == 0)

	// Выводы подключения ЖКИ индикатора WH2002 или аналогичного HD44780.
	#define LCD_DATA_INPUT			(GPIOE->IDR)

	// E (enable) bit
	#define LCD_STROBE_PORT_S(v)		do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LCD_STROBE_PORT_C(v)		do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)

	// RS & WE bits
	#define LCD_RS_PORT_S(v)		do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LCD_RS_PORT_C(v)		do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LCD_WE_PORT_S(v)		do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LCD_WE_PORT_C(v)		do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LCD_STROBE_BIT			(1u << 15)	// * PE15

	#define WRITEE_BIT				(1u << 13)	// * PE13
	//#define WRITEE_BIT_ZERO				(1u << 13)	// * PE13
	
	#define ADDRES_BIT				(1u << 12)	// * PE12

	#define LCD_DATAS_BITS			((1u << 11) | (1u << 10) | (1u << 9) | (1u << 8))
	#define LCD_DATAS_BIT_LOW		8		// какой бит данных младший в слове считанном с порта

	#define DISPLAY_BUS_DATA_GET() ((LCD_DATA_INPUT & LCD_DATAS_BITS) >> LCD_DATAS_BIT_LOW) /* получить данные с шины LCD */
	#define DISPLAY_BUS_DATA_SET(v) do { /* выдача данных (не сдвинуьых) */ \
			const portholder_t t = (portholder_t) (v) << LCD_DATAS_BIT_LOW; \
			GPIOE->BSRR = BSRR_S(t & LCD_DATAS_BITS) | BSRR_C(~ t & LCD_DATAS_BITS); \
			__DSB(); \
		} while (0)


	/* инициализация управляющих выходов процессора для управления HD44780 - полный набор выходов */
	#define LCD_CONTROL_INITIALIZE() \
		do { \
			arm_hardware_pioe_outputs2m(LCD_STROBE_BIT | WRITEE_BIT | ADDRES_BIT, 0); \
		} while (0)
	/* инициализация управляющих выходов процессора для управления HD44780 - WE=0 */
	#define LCD_CONTROL_INITIALIZE_WEEZERO() \
		do { \
			arm_hardware_pioe_outputs2m(LCD_STROBE_BIT | WRITEE_BIT_ZERO | ADDRES_BIT, 0); \
		} while (0)
	/* инициализация управляющих выходов процессора для управления HD44780 - WE отсутствует - сигнал к индикатору заземлён */
	#define LCD_CONTROL_INITIALIZE_WEENONE() \
		do { \
			arm_hardware_pioe_outputs2m(LCD_STROBE_BIT | ADDRES_BIT, 0); \
		} while (0)

	#define LCD_DATA_INITIALIZE_READ() \
		do { \
			arm_hardware_pioe_inputs(LCD_DATAS_BITS);	/* переключить порт на чтение с выводов */ \
		} while (0)

	#define LCD_DATA_INITIALIZE_WRITE(v) \
		do { \
			arm_hardware_pioe_outputs2m(LCD_DATAS_BITS, (v) << LCD_DATAS_BIT_LOW);	/* открыть выходы порта */ \
		} while (0)

#endif

// при подключении панели по spi не требуется - на дешивраторе висим.
#if 0 && LCDMODE_UC1608
	#define UC1608_CSP_PORT_S(v)		do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define UC1608_CSP_PORT_C(v)		do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define UC1608_CSP			(1u << 15)			// * PE15
	#define SPI_CSEL255			255				// по этому чипселекту выбираем положительным сигналом

	#define UC1608_CSP_INITIALIZE() do { \
			arm_hardware_pioe_outputs(UC1608_CSP, 0); \
		} while (0)
#endif


#if WITHENCODER
	// Выводы подключения енкодера
	// Encoder inputs: PB11 - PHASE A, PB10 = PHASE B
	// Обязательно буфер (входы процессора низковольтные).

	#define ENCODER_INPUT_PORT			(GPIOB->IDR) 
	#define ENCODER_BITS ((1u << 11) | (1u << 10))		// * PB11 & PB10
	#define ENCODER_SHIFT 10

	#define ENCODER_INITIALIZE() \
		do { \
			arm_hardware_piob_inputs(ENCODER_BITS); \
			arm_hardware_piob_updown(ENCODER_BITS, 0); \
			arm_hardware_piob_onchangeinterrupt(ENCODER_BITS, ENCODER_BITS, ENCODER_BITS, ARM_SYSTEM_PRIORITY); \
		} while (0)

#endif

#if WITHI2SHW
	#define I2S2HW_INITIALIZE() \
		do { \
			enum { \
				I2S2_MCK = (1U << 6),		/* PC6	*/ \
				I2S2ext_SD = (1U << 14),	/* PB14	- приём */ \
				I2S2_WS = (1U << 12),		/* PB12	*/ \
				I2S2_CK = (1U << 13),		/* PB13	*/ \
				I2S2_SD = (1U << 15)		/* PB15	*/ \
			}; \
			arm_hardware_piob_altfn2(I2S2_WS, AF_SPI2); \
			arm_hardware_piob_altfn2(I2S2_CK, AF_SPI2); \
			arm_hardware_piob_altfn2(I2S2_SD, AF_SPI2); \
			arm_hardware_pioc_altfn20(I2S2_MCK, AF_SPI2); \
			arm_hardware_piob_altfn2(I2S2ext_SD, AF_SPI2ext); \
		} while (0)
#endif /* WITHSAI1HW */

#if WITHSAI1HW
	#define SAI1HW_INITIALIZE() \
		do { \
			arm_hardware_pioe_altfn20(1U << 2, AF_SAI);	/* PE2 - SAI1_MCK_A - 12.288 MHz	*/ \
			arm_hardware_pioe_altfn20(1U << 3, AF_SAI);	/* PE3 - SAI1_SD_B	(i2s data from codec)	*/ \
			arm_hardware_pioe_altfn2(1U << 4, AF_SAI);		/* PE4 - SAI1_FS_A	- 48 kHz	*/ \
			arm_hardware_pioe_altfn20(1U << 5, AF_SAI);	/* PE5 - SAI1_SCK_A	*/ \
			arm_hardware_pioe_altfn20(1U << 6, AF_SAI);	/* PE6 - SAI1_SD_A	(i2s data to codec)	*/ \
		/* #if CODEC_TYPE_CS4272_STANDALONE	*/ \
			arm_hardware_pioe_updown(0, 1U << 6);	/* PE6: pull-down на выходе данных от кодека: после RESET вход в stand-alone mode */ \
		/* #endif */ \
		} while (0)
#endif /* WITHSAI1HW */

/* Распределение битов в ARM контроллерах */

#if WITHCAT || WITHNMEA
	// CAT data lites
	// RXD at PA10, TXD at PA9

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

#endif

#if WITHTX

	// txpath outputs not used
	////#define TXPATH_TARGET_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
	////#define TXPATH_TARGET_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)
	// 
	#define TXGFV_RX		(1u << 4)
	#define TXGFV_TRANS		0			// переход между режимами приёма и передачи
	#define TXGFV_TX_SSB	(1u << 0)
	#define TXGFV_TX_CW		(1u << 1)
	#define TXGFV_TX_AM		(1u << 2)
	#define TXGFV_TX_NFM	(1u << 3)

	#define TXPATH_INITIALIZE() \
		do { \
		} while (0)

	// PTT input - PC9
	#define PTT_TARGET_PIN				(GPIOC->IDR)
	#define PTT_BIT_PTT					(1U << 9)		// * PC9

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() \
		do { \
			arm_hardware_pioc_inputs(PTT_BIT_PTT); \
			arm_hardware_pioc_updown(PTT_BIT_PTT, 0); \
		} while (0)

#endif /* WITHTX */

#if WITHELKEY
	// Electronic key inputs
	#define ELKEY_TARGET_PIN			(GPIOC->IDR)
	#define ELKEY_BIT_LEFT				(1U << 7)		// * PC7
	#define ELKEY_BIT_RIGHT				(1U << 8)		// * PC8

	#define ELKEY_INITIALIZE() \
		do { \
			arm_hardware_pioc_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
			arm_hardware_pioc_updown(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, 0); \
		} while (0)

#endif /* WITHELKEY */

// IOUPDATE = PA15
#define SPI_IOUPDATE_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_IOUPDATE_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_IOUPDATE_BIT		(1U << 15)	// * PA15

// Набор определений для работы без внешнего дешифратора
#define SPI_ALLCS_PORT_S(v)	do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_ALLCS_PORT_C(v)	do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)

#define SPI_CSEL8	(1U << 0)	// * PD0 on-board nvmem FM25L16B
#define SPI_CSEL9	(1U << 3)	// * PD3 on-board codec1 TLV320AIC23B
// Здесь должны быть перечислены все биты формирования CS  устройстве.
#define SPI_ALLCS_BITS (SPI_CSEL8 | SPI_CSEL9)

#define SPI_ADDRESS_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_ADDRESS_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)

// Биты адреса для дешифратора SPI
#define SPI_A0 ((1u << 12))			// * PE13 
#define SPI_A1 ((1u << 13))			// * PE14 
#define SPI_A2 ((1u << 14))			// * PE15 

#define SPI_NAEN_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_NAEN_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)

#define SPI_NAEN_BIT (1u << 7)		// * PE7 used

/* инициализация лиий выбора периферийных микросхем */
#define SPI_ALLCS_INITIALIZE() \
	do { \
		arm_hardware_piod_outputs2m(SPI_ALLCS_BITS, SPI_ALLCS_BITS); \
	} while (0)

/* инициализация сигналов управлдения дешифратором CS */
#define SPI_ADDRESS_NAEN_INITIALIZE() \
	do { \
		arm_hardware_pioe_outputs2m(SPI_NAEN_BIT | SPI_ADDRESS_BITS, SPI_NAEN_BIT); \
	} while (0)
/* инициализация сигналов управлдения дешифратором CS */
#define SPI_ADDRESS_AEN_INITIALIZE() \
	do { \
	} while (0)
/* инициализация сигнала IOUPDATE на DDS */
/* начальное состояние = 1 */
#define SPI_IOUPDATE_INITIALIZE() \
	do { \
		arm_hardware_pioa_outputs2m(SPI_IOUPDATE_BIT, SPI_IOUPDATE_BIT); \
	} while (0)
#define SPI_IORESET_INITIALIZE() \
	do { \
	} while (0)


// MOSI & SCK port
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

	#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

		#define	TWIHARD_INITIALIZE() do { \
				arm_hardware_piob_periphopendrain_altfn2(TARGET_TWI_TWCK | TARGET_TWI_TWD, AF_I2C1);	/* AF=4 */ \
			} while (0) 

	#endif

#if WITHCPUDACHW
	/* включить нужные каналы */
	#define HARDWARE_DAC_INITIALIZE() do { \
			DAC1->CR = DAC_CR_EN1; /* DAC1 enable */ \
		} while (0)
	#define HARDWARE_DAC_AGC(v) do { \
			DAC1->DHR12R1 = (v);	/* DAC1 set value */ \
		} while (0)
	#define HARDWARE_DAC_ALC(v) do { \
		} while (0)
#endif /* WITHCPUDACHW */

#endif // WITHTWISW

#if WITHCPUADCHW
	#define HARDWARE_ADC_INITIALIZE(ainmask) do { \
			arm_hardware_pioa_analoginput(((ainmask) >> 0) & 0xff);	/* ADC12_IN0..ADC12_IN7 */ \
			arm_hardware_piob_analoginput(((ainmask) >> 8) & 0x03);	/* ADC12_IN8..ADC12_IN0 */ \
			arm_hardware_pioc_analoginput(((ainmask) >> 10) & 0x3f);	/* ADC12_IN10..ADC12_IN15 */ \
		} while (0)
#endif /* WITHCPUADCHW */

#if WITHUART1HW
	#define HARDWARE_USART1_INITIALIZE() do { \
			arm_hardware_pioa_altfn2((1U << 9) | (1U << 10), AF_USART1); /* PA9: TX DATA line (2 MHz), PA10: RX data line */ \
			arm_hardware_pioa_updown((1U << 10), 0);	/* PA10: pull-up RX data */ \
		} while (0)
#endif /* WITHUART1HW */


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

#endif /* ARM_STM32F4XX_TQFP100_CPUSTYLE_RAVEN_V1_H_INCLUDED */
