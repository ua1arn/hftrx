/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Плата DSP обработки "Дятел-2" на процессоре STM32F429 с кодеками TLV320AIC23B и CS4272

#ifndef ARM_STM32F4XX_TQFP100_CPUSTYLE_WDKP_V1B_H_INCLUDED
#define ARM_STM32F4XX_TQFP100_CPUSTYLE_WDKP_V1B_H_INCLUDED 1

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

//#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
#define WITHCAT_USART1		1
#define WITHDEBUG_USART1	1
#define WITHMODEM_USART1	1
#define WITHNMEA_USART1	1

//#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */
//#define WITHUSBHWVBUSSENSE	1	/* используется предопределенный вывод VBUS_SENSE */
//#define WITHUSBHWHIGHSPEED	1	/* Используется встроенная в процессор поддержка USB HS */
//#define WITHUSBHWHIGHSPEEDDESC	1	/* Требуется формировать дескрипторы как для HIGH SPEED */
//#define WITHUSBHW_DEVICE	USB_OTG_FS	/* на этом устройстве поддерживается функциональность DEVUCE	*/

//#define BSRR_S(v) ((v) * GPIO_BSRR_BS_0)	/* Преобразование значения для установки бита в регистре */
//#define BSRR_C(v) ((v) * GPIO_BSRR_BR_0)	/* Преобразование значения для сброса бита в регистре */

#define LS020_RS_INITIALIZE() \
	do { \
		arm_hardware_piod_outputs2m(LS020_RS, LS020_RS); \
	} while (0)

#define LS020_RESET_INITIALIZE() \
	do { \
		arm_hardware_piod_outputs2m(LS020_RST, LS020_RST); \
	} while (0)


#if LCDMODE_SPI_NA || LCDMODE_SPI_RA
	// эти контроллеры требуют только RS
	// Эти контроллеры требуют RESET и RS

	#define LS020_RS_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RS			0//(1u << 9)			// PC9

#endif

#if LCDMODE_UC1608
	// LCDMODE_SPI_RA already defined in paramdepend.h

	#define UC1608_CSP_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define UC1608_CSP_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define UC1608_CSP			0//(1u << 13)			// PC13
	#define SPI_CSEL255			255				// по этому чипселекту выбираем положительным сигналом

	#define UC1608_CSP_INITIALIZE() do { \
			arm_hardware_piod_outputs(UC1608_CSP, 0); \
		} while (0)
#endif


#if WITHENCODER
	// Выводы подключения енкодера
	// Encoder inputs: PC15 - PHASE A, PC14 = PHASE B
	// Обязательно буфер (входы процессора низковольтные).

	#define ENCODER_INPUT_PORT			(GPIOC->IDR) 
	#define ENCODER_BITS ((1u << 15) | (1u << 14))
	#define ENCODER_SHIFT 14

	#define ENCODER_INITIALIZE() \
		do { \
			arm_hardware_pioc_inputs(ENCODER_BITS); \
			arm_hardware_pioc_updown(ENCODER_BITS, 0); \
			arm_hardware_pioc_onchangeinterrupt(ENCODER_BITS, ENCODER_BITS, ENCODER_BITS, ARM_OVERREALTIME_PRIORITY); \
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
			arm_hardware_pioe_altfn20(1U << 2, AF_SAI);		/* PE2 - SAI1_MCK_A - 12.288 MHz	*/ \
			arm_hardware_pioe_altfn20(1U << 3, AF_SAI);		/* PE3 - SAI1_SD_B	(i2s data from codec)	*/ \
			arm_hardware_pioe_altfn2(1U << 4, AF_SAI);		/* PE4 - SAI1_FS_A	- 48 kHz	*/ \
			arm_hardware_pioe_altfn20(1U << 5, AF_SAI);		/* PE5 - SAI1_SCK_A	*/ \
			arm_hardware_pioe_altfn20(1U << 6, AF_SAI);		/* PE6 - SAI1_SD_A	(i2s data to codec)	*/ \
		/* #if CODEC_TYPE_CS4272_STANDALONE	*/ \
			arm_hardware_pioe_updown(0, 1U << 6);	/* pull-down на выходе данных от кодека: после RESET вход в stand-alone mode */ \
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
#endif

#if WITHTX

	// txpath outputs
	#define TXPATH_TARGET_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TXPATH_TARGET_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)
	#if 1
		// Управление передатчиком - сигналы TXPATH_ENABLE_SSB (PC7) и TXPATH_ENABLE_CW (PC8) - активны при нуле на выходе.
		#define TXPATH_BIT_ENABLE_SSB		(1u << 9)		// PD9
		#define TXPATH_BIT_ENABLE_CW		(1u << 10)		// PD10

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
				arm_hardware_piod_opendrain(TXPATH_BITS_ENABLE, TXPATH_BITS_ENABLE); \
			} while (0)
	#else
		// Управление передатчиком - единственный сигнал разрешения тракта
		#define TXPATH_BIT_GATE (1u << 9)	// PD9 - выходной сигнал из процессора - управление передатчиком.
		//#define TXPATH_BIT_GATE_RX TXPATH_BIT_GATE	// сигнал tx2 - управление передатчиком. При приёме активен
		#define TXPATH_BIT_GATE_RX 0	// сигнал tx2 - управление передатчиком. При приёме не активен

		// Подготовленные управляющие слова
		#define TXGFV_RX		TXPATH_BIT_GATE_RX
		#define TXGFV_TRANS		0			// переход между режимами приёма и передачи
		#define TXGFV_TX_SSB	TXPATH_BIT_GATE
		#define TXGFV_TX_CW		TXPATH_BIT_GATE
		// Управление передатчиком - сигнал TXPATH_BIT_GATE
		// Установка начального стстояния выходов
		#define TXPATH_INITIALIZE() \
			do { \
				arm_hardware_piod_outputs2m(TXPATH_BIT_GATE, 0); \
			} while (0)
	#endif

	// PTT input - PC9
	#define PTT_TARGET_PIN				(GPIOC->IDR)
	#define PTT_BIT_PTT					(1U << 9)		// PC9

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
	#define ELKEY_BIT_LEFT				(1U << 7)		// PC7
	#define ELKEY_BIT_RIGHT				(1U << 8)		// PC8

	#define ELKEY_INITIALIZE() \
		do { \
			arm_hardware_pioc_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
			arm_hardware_pioc_updown(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, 0); \
		} while (0)

#endif /* WITHELKEY */


// IOUPDATE = PС13
#define SPI_IOUPDATE_PORT_S(v)	do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_IOUPDATE_PORT_C(v)	do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_IOUPDATE_BIT		(1U << 13)	// PC13 - только как выход, слаботочный, 3.3 вольта

/* инициализация сигнала IOUPDATE на DDS */
/* начальное состояние = 1 */
#define SPI_IOUPDATE_INITIALIZE() \
	do { \
		arm_hardware_pioc_outputs2m(SPI_IOUPDATE_BIT, SPI_IOUPDATE_BIT); \
	} while (0)

#define TARGET_CS4272_RESET_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
#define TARGET_CS4272_RESET_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)
#define TARGET_CS4272_RESET_BIT		(1U << 2)	// PD2

#define CS4272_RESET_INITIALIZE() \
	do { \
		arm_hardware_piod_outputs2m(TARGET_CS4272_RESET_BIT, TARGET_CS4272_RESET_BIT); \
	} while (0)

// Набор определений для работы без внешнего дешифратора
#define SPI_ALLCS_PORT_S(v)	do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_ALLCS_PORT_C(v)	do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)

//	#define targetdds1 SPI_CSEL0 	/* DDS1 */
//	#define targetdds2 SPI_CSEL1 	/* DDS2 - LO3 output */
//	#define targetdds3 SPI_CSEL2 	/* DDS3 - PBT output */
//	//#define targetpll1 SPI_CSEL2 	/* ADF4001 after DDS1 - divide by r1 and scale to n1. Для двойной DDS первого гетеродина - вторая DDS */
//	//#define targetpll2 SPI_CSEL3 	/* ADF4001 - fixed 2-nd LO generate or LO1 divider */ 
//	#define targetext1 SPI_CSEL4 	/* external devices control */
//#ifndef SPI_IOUPDATE_BIT 
//	#define targetupd1 SPI_CSEL5 	/* DDS IOUPDATE signals at output of SPI chip-select register */
//#endif
//	#define targetctl1 SPI_CSEL6 	/* control register */
//	#define targetnvram SPI_CSEL7  	/* serial nvram */

#define SPI_CSEL7	(1U << 0)	// PD0 on-board nvmem FM25L16B
#define SPI_CSEL2	(1U << 1)	// PD1 on-board codec2 CS4272
#define SPI_CSEL3	(1U << 3)	// PD3 on-board codec1 TLV320AIC23B
#define SPI_CSEL0	(1U << 4)	// PD4 ext
#define SPI_CSEL1	(1U << 5)	// PD5 ext
#define SPI_CSEL4	(1U << 6)	// PD6 ext
#define SPI_CSEL5	(1U << 7)	// PD7 ext
#define SPI_CSEL6	(1U << 8)	// PD8 ext
// Здесь должны быть перечислены все биты формирования CS  устройстве.
#define SPI_ALLCS_BITS (SPI_CSEL0 | SPI_CSEL1 | SPI_CSEL2 | SPI_CSEL3 | SPI_CSEL4 | SPI_CSEL5 | SPI_CSEL6 | SPI_CSEL7)

/* инициализация лиий выбора периферийных микросхем */
#define SPI_ALLCS_INITIALIZE() \
	do { \
		arm_hardware_piod_outputs2m(SPI_ALLCS_BITS, SPI_ALLCS_BITS); \
	} while (0)

// MOSI & SCK port
#define SPI_TARGET_SCLK_PORT_C(v)	do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_TARGET_SCLK_PORT_S(v)	do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_TARGET_MOSI_PORT_C(v)	do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_TARGET_MOSI_PORT_S(v)	do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)

#define	SPI_SCLK_BIT			(1U << 3)	// PB3 бит, через который идет синхронизация SPI
#define	SPI_MOSI_BIT			(1U << 5)	// PB5 бит, через который идет вывод (или ввод в случае двунаправленного SPI).

#define SPI_TARGET_MISO_PIN		(GPIOB->IDR)		// was PINA 
#define	SPI_MISO_BIT			(1U << 4)	// PB4 бит, через который идет ввод с SPI.

	#define SPIIO_INITIALIZE() do { \
			arm_hardware_piob_outputs(SPI_MOSI_BIT | SPI_SCLK_BIT, SPI_MOSI_BIT | SPI_SCLK_BIT); \
			arm_hardware_piob_inputs(SPI_MISO_BIT); \
		} while (0)

#if WITHSPISLAVE
	// SPI SLAVE на dspboard_v1
	// SPI control pins
	// SPI3 hardware used.

	// MOSI & SCK port
	// STM32F303: SPI1_NSS can be placed on PA4 or PA15

	//#define SPI_TARGET_PIN			GPIOB->IDR // was PINA 
	#define	SPI_SLAVE_SCLK_BIT			(1U << 10)	// PC10 бит, через который идет синхронизация SPI
	#define	SPI_SLAVE_MISO_BIT			(1U << 11)	// PC11 бит, через который идет ввод с SPI.
	#define	SPI_SLAVE_MOSI_BIT			(1U << 12)	// PC12 бит, через который идет вывод (или ввод в случае двунаправленного SPI).

	#define SPI_SLAVE_NSS				(1U << 15)	// PA15 used

	// В этих процессорах и входы и выходы перекдючаются на ALT FN
	// Выход в сторону master не используется - не подключаем

	#define HARDWARE_SPISLAVE_INITIALIZE() do { \
		arm_hardware_pioc_altfn50(SPI_SLAVE_MOSI_BIT | SPI_SLAVE_SCLK_BIT /*| SPI_SLAVE_MISO_BIT*/, AF_SPI3); \
		arm_hardware_pioa_altfn50(SPI_SLAVE_NSS, AF_SPI3);	/* PA15 */ \
		arm_hardware_pioa_onchangeinterrupt(SPI_SLAVE_NSS, SPI_SLAVE_NSS, 0, ARM_SYSTEM_PRIORITY); \
	} while (0)
#endif /* WITHSPISLAVE */


////#define SIDETONE_TARGET_BIT		(1u << 8)	// (PB8, base mapping) output TIM4_CH3 

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

#endif // WITHTWISW


#if WITHCPUDACHW
	/* включить нужные каналы */
	#define HARDWARE_DAC_INITIALIZE() do { \
			DAC1->CR = DAC_CR_EN2 | DAC_CR_EN1; /* DAC2 and DAC1 enable */ \
		} while (0)
	#define HARDWARE_DAC_AGC(v) do { \
			DAC1->DHR12R1 = (v);	/* DAC1 set value */ \
		} while (0)
	#define HARDWARE_DAC_ALC(v) do { \
			DAC1->DHR12R2 = (v); /* DAC2 set value */ \
		} while (0)
#endif /* WITHCPUDACHW */

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

#endif /* ARM_STM32F4XX_TQFP100_CPUSTYLE_WDKP_V1B_H_INCLUDED */
