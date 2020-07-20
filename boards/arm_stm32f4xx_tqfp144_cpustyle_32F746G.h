/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

/* Плата STM32F746G-DISCO с процессором STM32F746NGH6	*/

#ifndef ARM_STM32F4XX_TQFP144_CPUSTYLE_32F746G_H_INCLUDED
#define ARM_STM32F4XX_TQFP144_CPUSTYLE_32F746G_H_INCLUDED 1

//#define HARDWARE_ARM_USEUSART0 1		// US0:
//#define HARDWARE_ARM_USEUSART1 1		// US1: PA9/PA10 pins

//#define WITHSPI16BIT	1		/* возможно использование 16-ти битных слов при обмене по SPI */
//#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
//#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
#define WITHSPISW 	1	/* Использование программного управления SPI. Нельзя убирать эту строку - требуется явное отключение из-за конфликта с I2C */

//#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */

//#define WITHI2SHW	1	/* Использование I2S - аудиокодек	*/

#define WITHSAI1HW	1	/* Использование SAI1 - FPGA или IF codec	*/
#define WITHSAI2HW	1	/* Использование SAI2 - аудиокодек	*/

//#define WITHCPUDACHW	1	/* использование встроенного в процессор DAC */
//#define WITHCPUADCHW 	1	/* использование встроенного в процессор ADC */

//#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
//#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */

#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
#define WITHDMA2DHW		1	/* Использование DMA2D для формирования изображений	*/
#define WITHSDRAMHW	1	/* В процессоре есть внешняя память */

#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */
#if 1
	// use FS
	//#define WITHUSBDEV_HSDESC	1	/* Требуется формировать дескрипторы как для HIGH SPEED */
	//#define WITHUSBDEV_VBUSSENSE	1	/* используется предопределенный вывод VBUS_SENSE */
	////#define WITHUSBHW_DEVICE	USB_OTG_FS	/* на этом устройстве поддерживается функциональность DEVICE	*/
#else
	// USE HS with ULPI
	//#define WITHUSBDEV_HSDESC	1	/* Требуется формировать дескрипторы как для HIGH SPEED */
	#define WITHUSBDEV_VBUSSENSE	1	/* используется предопределенный вывод VBUS_SENSE */
	#define WITHUSBDEV_HIGHSPEEDULPI	1
	//#define WITHUSBDEV_HIGHSPEEDPHYC	1
	////#define WITHUSBHW_DEVICE	USB_OTG_HS	/* на этом устройстве поддерживается функциональность DEVICE	*/
#endif

#define WITHUART1HW	1	/* PA9, PB7 Используется периферийный контроллер последовательного порта #1 */
//#define WITHUART2HW	1	/* PD5, PD6 Используется периферийный контроллер последовательного порта #2 */

//#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
//#define WITHMODEM_CDC	1
//#define WITHCAT_USART2		1
//#define WITHDEBUG_CDC	1
#define WITHDEBUG			1
#define WITHDEBUG_USART1	1
//#define WITHNMEA_USART1		1	/* порт подключения GPS/GLONASS */

#ifdef WITHUSBHW_DEVICE
	#define WITHUSBUAC		1	/* использовать виртуальную звуковую плату на USB соединении */
	//#define WITHUSBUACIN2		1	/* формируются три канала передачи звука */
	//#define WITHUAC2		1	/* UAC2 support */

	#define WITHUSBCDC		1	/* использовать виртуальный последовательный порт на USB соединении */
	#define WITHUSBHWCDC_N	2	/* количество виртуальных последовательных портов */
	//#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
	//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */
	//#define WITHUSBHID	1	/* HID использовать Human Interface Device на USB соединении */

#endif

//#define BSRR_S(v) ((v) * GPIO_BSRR_BS_0)	/* Преобразование значения для установки бита в регистре */
//#define BSRR_C(v) ((v) * GPIO_BSRR_BR_0)	/* Преобразование значения для сброса бита в регистре */

#define LS020_RS_INITIALIZE() \
	do { \
		arm_hardware_piof_outputs2m(LS020_RS, LS020_RS); \
	} while (0)

#define LS020_RESET_INITIALIZE() \
	do { \
		arm_hardware_piof_outputs2m(LS020_RESET, LS020_RESET); \
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

	#define LS020_RS_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RS			0//(1u << 3)			// PF3

#elif LCDMODE_SPI_RN
	// эти контроллеры требуют только RESET

	#define LS020_RESET_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RESET			0//(1u << 2)			// PF2 D6 signal in HD44780 socket

#elif LCDMODE_SPI_RA
	// Эти контроллеры требуют RESET и RS

	#define LS020_RS_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RS			0//(1u << 3)			// PF3

	#define LS020_RESET_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RESET			0//(1u << 2)			// PF2 D6 signal in HD44780 socket

#elif LCDMODE_HD44780 && (LCDMODE_SPI == 0)

	// Выводы подключения ЖКИ индикатора WH2002 или аналогичного HD44780.
	#define LCD_DATA_INPUT			(GPIOF->IDR)

	// E (enable) bit
	#define LCD_STROBE_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LCD_STROBE_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)

	// RS & WE bits
	#define LCD_RS_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LCD_RS_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LCD_WE_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LCD_WE_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LCD_STROBE_BIT			(1u << 6)	// PF6

	#define WRITEE_BIT				(1u << 5)	// PF5
	
	#define ADDRES_BIT				(1u << 4)	// PF4

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
	// Выводы подключения енкодера
	// Encoder inputs: PF8 - PHASE A, PF7 = PHASE B
	// Обязательно буфер (входы процессора низковольтные).

	#define ENCODER_INPUT_PORT			(GPIOF->IDR) 
	#define ENCODER_BITS ((1u << 8) | (1u << 7))		// PF8 & PF7
	#define ENCODER_SHIFT 7
	// Обязательно буфер (входы процессора низковольтные).

	#define ENCODER2_INPUT_PORT			(GPIOF->IDR) 
	#define ENCODER2_BITS ((1u << 10) | (1u << 9))		// PF10 & PF9
	#define ENCODER2_SHIFT 9

	#define ENCODER_INITIALIZE() \
		do { \
			arm_hardware_piof_inputs(ENCODER_BITS); \
			arm_hardware_piof_updown(ENCODER_BITS, 0); \
			arm_hardware_piof_onchangeinterrupt(ENCODER_BITS, ENCODER_BITS, ENCODER_BITS, ARM_OVERREALTIME_PRIORITY); \
			arm_hardware_piof_inputs(ENCODER2_BITS); \
			arm_hardware_piof_updown(ENCODER2_BITS, 0); \
			arm_hardware_piof_onchangeinterrupt(0 * ENCODER2_BITS, ENCODER2_BITS, ENCODER2_BITS, ARM_OVERREALTIME_PRIORITY); \
		} while (0)

#endif

#if WITHI2SHW

	#if defined(STM32H743xx)
		#define PB5_I2S3_AF 7	// AF_7
	#else /* defined(STM32H743xx) */
		#define PB5_I2S3_AF AF_SPI3
	#endif /* defined(STM32H743xx) */

	// Инициализируются I2S2 и I2S3
	#define I2S2HW_INITIALIZE() do { \
		arm_hardware_piob_altfn2(1U << 12, AF_SPI2); /* PB12	*/ \
		arm_hardware_piob_altfn2(1U << 13, AF_SPI2); /* PB13	*/ \
		arm_hardware_piob_altfn2(1U << 15, AF_SPI2); /* PB15 - передача */ \
		arm_hardware_pioa_altfn2(1U << 15, AF_SPI3); /* PA15	*/ \
		arm_hardware_piob_altfn2(1U << 3, AF_SPI3); /* PB3	*/ \
		arm_hardware_piob_altfn2(1U << 5, PB5_I2S3_AF); /* PB5 - приём */ \
	} while (0)
#endif /* WITHSAI1HW */

#if WITHSAI1HW
	#define SAI1HW_INITIALIZE()	do { \
		arm_hardware_piof_altfn2(1U << 9, AF_SAI);		/* PF9 - SAI1_FS_B	- 48 kHz	*/ \
		arm_hardware_piof_altfn20(1U << 8, AF_SAI);		/* PF8 - SAI1_SCK_B	*/ \
		arm_hardware_piob_altfn2(1U << 2, AF_SAI);		/* PB2 - SAI1_SD_A	(i2s data to codec)	*/ \
		arm_hardware_piof_altfn2(1U << 6, AF_SAI);		/* PF6 - SAI1_SD_B	(i2s data from codec)	*/ \
	} while (0)
#endif /* WITHSAI1HW */

#if WITHSAI2HW
	/* 
	*/
	#define SAI2HW_INITIALIZE()	do { \
		arm_hardware_pioi_altfn20(1U << 4, AF_SAI2);	/* PI4 - SAI2_MCLK_A - 12.288 MHz	*/ \
		arm_hardware_pioi_altfn20(1U << 5, AF_SAI2);	/* PI5 - SAI2_SCK_A	*/ \
		arm_hardware_pioi_altfn2(1U << 6, AF_SAI2); 	/* PI6 SAI2_SD_A (i2s data to codec)	*/ \
		arm_hardware_pioi_altfn2(1U << 7, AF_SAI2); 	/* PI7 - SAI2_FS_A	- 48 kHz	*/ \
		arm_hardware_piog_altfn2(1U << 10, AF_SAI2);	/* PG10 - SAI2_SD_B	(i2s data from codec)	*/ \
	} while (0)
#endif /* WITHSAI1HW */

/* Распределение битов в ARM контроллерах */

#if (WITHCAT && WITHCAT_USART1)
	// CAT data lites
	// RXD at PB7, TXD at PA9

	// CAT control lines
	#define FROMCAT_TARGET_PIN_RTS		(GPIOA->IDR) 
	#define FROMCAT_BIT_RTS				0//(1u << 11)	/* PA11 сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	#define FROMCAT_TARGET_PIN_DTR		(GPIOA->IDR)
	#define FROMCAT_BIT_DTR				0//(1u << 12)	/* PA12 сигнал DTR от FT232RL	*/

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

#endif /* (WITHCAT && WITHCAT_USART1) */

#if (WITHCAT && WITHCAT_USART2)
	// CAT data lites
	// RXD at PA10, TXD at PA9

	// CAT control lines
	//#define FROMCAT_TARGET_PIN_RTS		(GPIOA->IDR) 
	//#define FROMCAT_BIT_RTS				(1u << 11)	/* PA11 сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	//#define FROMCAT_TARGET_PIN_DTR		(GPIOA->IDR)
	//#define FROMCAT_BIT_DTR				(1u << 12)	/* PA12 сигнал DTR от FT232RL	*/

	/* манипуляция от порта RS-232 */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
		} while (0)

	/* переход на передачу от порта RS-232 */
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
		} while (0)

#endif /* (WITHCAT && WITHCAT_USART2) */

#if (WITHNMEA && WITHNMEA_USART1)
	// CAT data lites
	// RXD at PB7, TXD at PA9
	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	#define FROMCAT_TARGET_PIN_DTR		(GPIOA->IDR) 
	#define FROMCAT_BIT_DTR				(1u << 12)	/* PA12 сигнал DTR от FT232RL	*/

	/* сигнал PPS от GPS/GLONASS/GALILEO модуля */
	#define NMEA_INITIALIZE() \
		do { \
			arm_hardware_pioa_inputs(FROMCAT_BIT_DTR); \
			arm_hardware_pioa_onchangeinterrupt(FROMCAT_BIT_DTR, FROMCAT_BIT_DTR, FROMCAT_BIT_DTR, ARM_SYSTEM_PRIORITY); \
		} while (0)

#endif /* (WITHNMEA && WITHNMEA_USART1) */

#if (WITHCAT && WITHCAT_CDC)

	// CAT data lites
	// RXD at PA10, TXD at PA9

	// CAT control lines
	//#define FROMCAT_TARGET_PIN_RTS		(GPIOA->IDR) // was PINA 
	//#define FROMCAT_BIT_RTS				(1u << 11)	/* сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	//#define FROMCAT_TARGET_PIN_DTR		(GPIOA->IDR) // was PINA 
	//#define FROMCAT_BIT_DTR				(1u << 12)	/* сигнал DTR от FT232RL	*/

	/* манипуляция от виртуального CDC порта */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
		} while (0)

	/* переход на передачу от виртуального CDC порта*/
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
		} while (0)

#endif /* (WITHCAT && WITHCAT_CDC) */

#if WITHSDHCHW
	#if WITHSDHCHW4BIT
		#define HARDWARE_SDIO_INITIALIZE()	do { \
			arm_hardware_piod_altfn50(1U << 2, AF_SDIO);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_altfn50(1U << 12, AF_SDIO);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_altfn50(1U << 8, AF_SDIO);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_altfn50(1U << 9, AF_SDIO);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_altfn50(1U << 10, AF_SDIO);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_altfn50(1U << 11, AF_SDIO);	/* PC11 - SDIO_D3	*/ \
		} while (0)
	#else /* WITHSDHCHW4BIT */
		#define HARDWARE_SDIO_INITIALIZE()	do { \
			arm_hardware_piod_altfn50(1U << 2, AF_SDIO);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_altfn50(1U << 12, AF_SDIO);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_altfn50(1U << 8, AF_SDIO);	/* PC8 - SDIO_D0	*/ \
		} while (0)
	#endif /* WITHSDHCHW4BIT */

	//#define HARDWARE_SDIO_WP_BIT	(1U << 1)	/* PD1 - SDIO_WP */
	#define HARDWARE_SDIO_CD_BIT	(1U << 0)	/* PC13 - uSD_Detect SDIO_SENSE */

	#define HARDWARE_SDIOSENSE_INITIALIZE()	do { \
			arm_hardware_pioc_inputs(HARDWARE_SDIO_CD_BIT); /* PD0 - SDIO_SENSE */ \
			arm_hardware_pioc_updown(HARDWARE_SDIO_CD_BIT, 0); \
	} while (0)

	#define HARDWARE_SDIOSENSE_CD() ((GPIOC->IDR & HARDWARE_SDIO_CD_BIT) == 0)	/* получить состояние датчика CARD PRESENT */
	#define HARDWARE_SDIOSENSE_WP() (0)	/* получить состояние датчика CARD WRITE PROTECT */

	/* если питание SD CARD управляется прямо с процессора */
	#define HARDWARE_SDIOPOWER_INITIALIZE()	do { \
		/* arm_hardware_pio3_outputs(HARDWARE_SDIO_POWER_BIT, HARDWARE_SDIO_POWER_BIT); */ /* питание выключено */ \
		} while (0)

#endif /* WITHSDHCHW */

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

	// PTT input - PD10
	#define PTT_TARGET_PIN				(GPIOI->IDR)
	#define PTT_BIT_PTT					(1U << 11)		// PI11

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) != 0)
	#define PTT_INITIALIZE() \
		do { \
			arm_hardware_pioi_inputs(PTT_BIT_PTT); \
		} while (0)

#endif /* WITHTX */

#if WITHELKEY
	// Electronic key inputs
	#define ELKEY_TARGET_PIN			(GPIOD->IDR)
	#define ELKEY_BIT_LEFT				0//(1U << 8)		// PD8
	#define ELKEY_BIT_RIGHT				0//(1U << 9)		// PD9

	#define ELKEY_INITIALIZE() \
		do { \
			arm_hardware_piod_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
			arm_hardware_piod_updown(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, 0); \
		} while (0)

#endif /* WITHELKEY */

// IOUPDATE = PA15
//#define SPI_IOUPDATE_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)
//#define SPI_IOUPDATE_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)
//#define SPI_IOUPDATE_BIT		(1U << 15)	// * PA15

// Набор определений для работы без внешнего дешифратора
#define SPI_ALLCS_PORT_S(v)	do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_ALLCS_PORT_C(v)	do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)

#define SPI_CSEL_PG15	0//(1U << 15)	// PG15 ext1
#define SPI_CSEL_PG8	0//(1U << 8)	// PG8 nvmem FM25L16B
#define SPI_CSEL_PG7	0//(1U << 7)	// PG7 board control registers chain
#define SPI_CSEL_PG6	0//(1U << 6)	// PG6 on-board codec1 NAU8822L
//#define SPI_CSEL_PG5	0//(1U << 5)	// PG5 FPGA CS2 - used as overflov signal from ADC
#define SPI_CSEL_PC7	(1U << 7)		// PC7 FPGA control registers CS
#define SPI_CSEL_PG1	0//(1U << 7)	// PG1 FPGA NCO1 registers CS

// Здесь должны быть перечислены все биты формирования CS в устройстве.
#define SPI_ALLCS_BITS (SPI_CSEL_PC7)

#define SPI_ALLCS_BITSNEG 0		// Выходы, активные при "1"

//#define SPI_NAEN_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
//#define SPI_NAEN_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)

//#define SPI_NAEN_BIT (1u << 7)		// * PE7 used

/* инициализация лиий выбора периферийных микросхем */

#define SPI_ALLCS_INITIALIZE() \
	do { \
		arm_hardware_pioc_outputs(SPI_ALLCS_BITS, SPI_ALLCS_BITS ^ SPI_ALLCS_BITSNEG); \
	} while (0)


/* инициализация сигналов управлдения дешифратором CS */
#define SPI_ADDRESS_NAEN_INITIALIZE() \
	do { \
	} while (0)
/* инициализация сигналов управлдения дешифратором CS */
#define SPI_ADDRESS_AEN_INITIALIZE() \
	do { \
	} while (0)
/* инициализация сигнала IOUPDATE на DDS */
/* начальное состояние = 1 */
#define SPI_IOUPDATE_INITIALIZE() \
	do { \
	} while (0)
#define SPI_IORESET_INITIALIZE() \
	do { \
	} while (0)

// MOSI & SCK port
#define SPI_TARGET_SCLK_PORT_C(v)	do { GPIOH->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_TARGET_SCLK_PORT_S(v)	do { GPIOH->BSRR = BSRR_S(v); __DSB(); } while (0)
#define	SPI_SCLK_BIT			(1U << 6)	// * PH6 бит, через который идет синхронизация SPI

#define SPI_TARGET_MOSI_PORT_C(v)	do { GPIOI->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_TARGET_MOSI_PORT_S(v)	do { GPIOI->BSRR = BSRR_S(v); __DSB(); } while (0)
#define	SPI_MOSI_BIT			(1U << 0)	// * PI0 бит, через который идет вывод (или ввод в случае двунаправленного SPI).

#define SPI_TARGET_MISO_PIN		(GPIOG->IDR)		// was PINA
#define	SPI_MISO_BIT			(1U << 7)	// * PG7 бит, через который идет ввод с SPI.

#define SPIIO_INITIALIZE() do { \
		arm_hardware_pioh_outputs(SPI_SCLK_BIT, SPI_SCLK_BIT); \
		arm_hardware_pioi_outputs(SPI_MOSI_BIT, SPI_MOSI_BIT); \
		arm_hardware_piog_inputs(SPI_MISO_BIT); \
	} while (0)


#define HARDWARE_SPI_CONNECT() do { \
		arm_hardware_piob_altfn50(SPI_MISO_BIT, AF_SPI1); /* В этих процессорах и входы и выходы перекдючаются на ALT FN */ \
		arm_hardware_pioa_altfn50(SPI_MOSI_BIT | SPI_SCLK_BIT, AF_SPI1); /* В этих процессорах и входы и выходы перекдючаются на ALT FN */ \
	} while (0)
#define HARDWARE_SPI_DISCONNECT() do { \
		arm_hardware_pioa_outputs(SPI_MOSI_BIT | SPI_SCLK_BIT, SPI_MOSI_BIT | SPI_SCLK_BIT); \
		arm_hardware_piob_inputs(SPI_MISO_BIT); \
	} while (0)
#define HARDWARE_SPI_CONNECT_MOSI() do { \
		arm_hardware_pioa_altfn50(SPI_MOSI_BIT, AF_SPI1);	/* PIO disable for MOSI bit (SD CARD read support) */ \
	} while (0)
#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
		arm_hardware_pioa_outputs(SPI_MOSI_BIT, SPI_MOSI_BIT);	/* PIO enable for MOSI bit (SD CARD read support)  */ \
	} while (0)

/* PA9, PB7 Используется периферийный контроллер последовательного порта #1 */
#define HARDWARE_USART1_INITIALIZE() do { \
		arm_hardware_pioa_altfn2((1U << 9), AF_USART1); /* PA9: TX DATA line (2 MHz) */ \
		arm_hardware_piob_altfn2((1U << 7), AF_USART1); /* PB7: RX DATA line (2 MHz) */ \
		arm_hardware_piob_updown((1U << 7), 0);	/* PB7: pull-up RX data */ \
	} while (0)

#define HARDWARE_USART2_INITIALIZE() do { \
	} while (0)

#define HARDWARE_SIDETONE_INITIALIZE() do { \
	} while (0)

#if KEYBOARD_USE_ADC
	#define HARDWARE_KBD_INITIALIZE() do { \
		} while (0)
#else
	#define HARDWARE_KBD_INITIALIZE() do { \
		/*arm_hardware_pioa_inputs(KBD_MASK); */\
		} while (0)
#endif

// PH7 I2C3_SCL & PH8 I2C3_SDA, I2C3
#if WITHTWISW || WITHTWIHW
	#define TARGET_TWI_TWCK_PORT_C(v) do { GPIOH->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do { GPIOH->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_TWI_TWD_PORT_C(v) do { GPIOH->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v) do { GPIOH->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_TWI_TWCK_PIN		(GPIOH->IDR)
	#define TARGET_TWI_TWD_PIN		(GPIOH->IDR)
	#define TARGET_TWI_TWCK		(1u << 7)		// PH7 I2C3_SCL
	#define TARGET_TWI_TWD		(1u << 8)		// PH8 I2C3_SDA

	// Инициализация битов портов ввода-вывода для программной реализации I2C
	#define	TWISOFT_INITIALIZE() do { \
			arm_hardware_pioh_opendrain(TARGET_TWI_TWCK | TARGET_TWI_TWD, TARGET_TWI_TWCK | TARGET_TWI_TWD); \
		} while (0) 
	// Инициализация битов портов ввода-вывода для аппаратной реализации I2C
	// присоединение выводов к периферийному устройству
	#define	TWIHARD_INITIALIZE() do { \
			arm_hardware_pioh_periphopendrain_altfn2(TARGET_TWI_TWCK | TARGET_TWI_TWD, AF_I2C3);	/* AF=4 */ \
		} while (0) 


#endif // WITHTWISW || WITHTWIHW

#if WITHFPGAWAIT_AS || WITHFPGALOAD_PS

	/* outputs */
	#define FPGA_NCONFIG_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define FPGA_NCONFIG_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define FPGA_NCONFIG_BIT		(1UL << 13)	/* PE13 bit conneced to nCONFIG pin ALTERA FPGA */

	/* inputs */
	#define FPGA_CONF_DONE_INPUT	(GPIOE->IDR)
	#define FPGA_CONF_DONE_BIT		(1UL << 14)	/* PE14 bit conneced to CONF_DONE pin ALTERA FPGA */

	#define FPGA_NSTATUS_INPUT		(GPIOE->IDR)
	#define FPGA_NSTATUS_BIT		(1UL << 15)	/* PE15 bit conneced to NSTATUS pin ALTERA FPGA */

	/* Инициадизация выводов GPIO процессора для получения состояния и управлением загрузкой FPGA */
	#define HARDWARE_FPGA_LOADER_INITIALIZE() do { \
			arm_hardware_pioe_outputs(FPGA_NCONFIG_BIT, FPGA_NCONFIG_BIT); \
			arm_hardware_pioe_inputs(FPGA_CONF_DONE_BIT | FPGA_NSTATUS_BIT); \
		} while (0)

	/* по косвенным признакам проверяем, проинициализировалась ли FPGA (вошла в user mode). */
	#define HARDWARE_FPGA_IS_USER_MODE() (local_delay_ms(400), 1)

#endif /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS */

#if WITHDSPEXTFIR
	// Биты доступа к массиву коэффициентов FIR фильтра в FPGA
	#define TARGET_FPGA_FIR_CS_PORT_C(v)	do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR_CS_PORT_S(v)	do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR_CS_BIT (1U << 4)	/* PB4 - fir CLK */

	#define TARGET_FPGA_FIR1_WE_PORT_C(v)	do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR1_WE_PORT_S(v)	do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR1_WE_BIT (1U << 6)	/* PC6 - fir1 WE */

	#define TARGET_FPGA_FIR2_WE_PORT_C(v)	do { GPIOG->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR2_WE_PORT_S(v)	do { GPIOG->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR2_WE_BIT (1U << 6)	/* PG6 - fir2 WE */

	#define TARGET_FPGA_FIR_INITIALIZE() do { \
			arm_hardware_pioe_outputs(TARGET_FPGA_FIR1_WE_BIT, TARGET_FPGA_FIR1_WE_BIT); \
			arm_hardware_pioe_outputs(TARGET_FPGA_FIR2_WE_BIT, TARGET_FPGA_FIR2_WE_BIT); \
			arm_hardware_pioe_outputs(TARGET_FPGA_FIR_CS_BIT, TARGET_FPGA_FIR_CS_BIT); \
		} while (0)
#endif /* WITHDSPEXTFIR */

/* получение состояния переполнения АЦП */
/*
#define TARGET_FPGA_OVF_PIN		(GPIOG->IDR)
#define TARGET_FPGA_OVF_BIT		(1u << 5)	// PG5
#define TARGET_FPGA_OVF_GET		((TARGET_FPGA_OVF_PIN & TARGET_FPGA_OVF_BIT) == 0)	// 1 - overflow active
#define TARGET_FPGA_OVF_INITIALIZE() do { \
			arm_hardware_piog_inputs(TARGET_FPGA_OVF_BIT); \
		} while (0)
*/

#if WITHCPUDACHW
	/* включить нужные каналы */
	#define HARDWARE_DAC_INITIALIZE() do { \
			DAC1->CR = DAC_CR_EN1; /* DAC1 enable */ \
		} while (0)
	#define HARDWARE_DAC_ALC(v) do { /* вывод 12-битного значения на ЦАП - канал 1 */ \
			DAC1->DHR12R1 = (v); /* DAC1 set value */ \
		} while (0)
#endif /* WITHCPUDACHW */

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
		arm_hardware_pioa_updownoff((1U << 11) | (1U << 12)); \
		} while (0)

	/**USB_OTG_HS GPIO Configuration    
	*/
	#define	USBD_HS_FS_INITIALIZE() do { \
		} while (0)

    /**USB_OTG_HS GPIO Configuration    
    PB5     ------> USB_OTG_HS_ULPI_D7
    PH4     ------> USB_OTG_HS_ULPI_NXT
    PB13     ------> USB_OTG_HS_ULPI_D6
    PB12     ------> USB_OTG_HS_ULPI_D5
    PC0     ------> USB_OTG_HS_ULPI_STP
    PC2     ------> USB_OTG_HS_ULPI_DIR
    PA5     ------> USB_OTG_HS_ULPI_CK
    PB10     ------> USB_OTG_HS_ULPI_D3
    PA3     ------> USB_OTG_HS_ULPI_D0
    PB1     ------> USB_OTG_HS_ULPI_D2
    PB0     ------> USB_OTG_HS_ULPI_D1
    PB11     ------> USB_OTG_HS_ULPI_D4 
    */
	#define	USBD_HS_ULPI_INITIALIZE() do { \
		arm_hardware_pioa_altfn50((1U << 3), AF_OTGHS);		/* PA3 - USB_OTG_HS_ULPI_D0		*/ \
		arm_hardware_piob_altfn50((1U << 0), AF_OTGHS);		/* PB0 - USB_OTG_HS_ULPI_D1		*/ \
		arm_hardware_piob_altfn50((1U << 1), AF_OTGHS);		/* PB1 - USB_OTG_HS_ULPI_D2		*/ \
		arm_hardware_piob_altfn50((1U << 10), AF_OTGHS);	/* PB10 - USB_OTG_HS_ULPI_D3	*/ \
		arm_hardware_piob_altfn50((1U << 11), AF_OTGHS);	/* PB11 - USB_OTG_HS_ULPI_D4	*/ \
		arm_hardware_piob_altfn50((1U << 12), AF_OTGHS);	/* PB12 - USB_OTG_HS_ULPI_D5	*/ \
		arm_hardware_piob_altfn50((1U << 13), AF_OTGHS);	/* PB13 - USB_OTG_HS_ULPI_D6	*/ \
		arm_hardware_piob_altfn50((1U << 5), AF_OTGHS);		/* PB5 - USB_OTG_HS_ULPI_D7		*/ \
		arm_hardware_pioh_altfn50((1U << 4), AF_OTGHS);		/* PH4 - USB_OTG_HS_ULPI_NXT	*/ \
		arm_hardware_pioc_altfn50((1U << 0), AF_OTGHS);		/* PC0 - USB_OTG_HS_ULPI_STP	*/ \
		arm_hardware_pioc_altfn50((1U << 2), AF_OTGHS);		/* PC2 - USB_OTG_HS_ULPI_DIR	*/ \
		arm_hardware_pioa_altfn50((1U << 5), AF_OTGHS);		/* PA5 - USB_OTG_HS_ULPI_CK		*/ \
		} while (0)
#endif /* WITHUSBHW */

#if LCDMODE_LTDC
	enum
	{
		GPIO_AF_LTDC = 14,  /* LCD-TFT Alternate Function mapping */
		GPIO_AF_LTDC9 = 9  /* LCD-TFT Alternate Function mapping */
	};
	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_INITIALIZE(demode) do { \
		arm_hardware_pioi_outputs((1U << 12), 0 * (1U << 12));	/* PI12 DISP=constant high */ \
		/* Control & synchronisation signals */ \
		arm_hardware_pioi_altfn20((1U << 9), GPIO_AF_LTDC);		/* VSYNC */ \
		arm_hardware_pioi_altfn20((1U << 10), GPIO_AF_LTDC);	/* HSYNC */ \
		arm_hardware_pioi_altfn20((1U << 14), GPIO_AF_LTDC);	/* CLK */ \
		arm_hardware_piok_altfn20((1U << 7), GPIO_AF_LTDC);		/* DE */ \
		/* RED */ \
		arm_hardware_pioi_altfn20((1U << 15), GPIO_AF_LTDC);	/* R0 */ \
		arm_hardware_pioj_altfn20((1U << 0), GPIO_AF_LTDC);		/* R1 */ \
		arm_hardware_pioj_altfn20((1U << 1), GPIO_AF_LTDC);		/* R2 */ \
		arm_hardware_pioj_altfn20((1U << 2), GPIO_AF_LTDC);		/* R3 */ \
		arm_hardware_pioj_altfn20((1U << 3), GPIO_AF_LTDC);		/* R4 */ \
		arm_hardware_pioj_altfn20((1U << 4), GPIO_AF_LTDC);		/* R5 */ \
		arm_hardware_pioj_altfn20((1U << 5), GPIO_AF_LTDC);		/* R6 */ \
		arm_hardware_pioj_altfn20((1U << 6), GPIO_AF_LTDC);		/* R7 */ \
		/* GREEN */ \
		arm_hardware_pioj_altfn20((1U << 7), GPIO_AF_LTDC);		/* G0 */ \
		arm_hardware_pioj_altfn20((1U << 8), GPIO_AF_LTDC);		/* G1 */ \
		arm_hardware_pioj_altfn20((1U << 9), GPIO_AF_LTDC);		/* G2 */ \
		arm_hardware_pioj_altfn20((1U << 10), GPIO_AF_LTDC);	/* G3 */ \
		arm_hardware_pioj_altfn20((1U << 11), GPIO_AF_LTDC);	/* G4 */ \
		arm_hardware_piok_altfn20((1U << 0), GPIO_AF_LTDC);		/* G5 */ \
		arm_hardware_piok_altfn20((1U << 1), GPIO_AF_LTDC);		/* G6 */ \
		arm_hardware_piok_altfn20((1U << 2), GPIO_AF_LTDC);		/* G7 */ \
		/* BLUE */ \
		arm_hardware_pioe_altfn20((1U << 4), GPIO_AF_LTDC);		/* B0 */ \
		arm_hardware_pioj_altfn20((1U << 13), GPIO_AF_LTDC);	/* B1 */ \
		arm_hardware_pioj_altfn20((1U << 14), GPIO_AF_LTDC);	/* B2 */ \
		arm_hardware_pioj_altfn20((1U << 15), GPIO_AF_LTDC);	/* B3 */ \
		arm_hardware_piog_altfn20((1U << 12), GPIO_AF_LTDC9);	/* B4 */ \
		arm_hardware_piok_altfn20((1U << 4), GPIO_AF_LTDC);		/* B5 */ \
		arm_hardware_piok_altfn20((1U << 5), GPIO_AF_LTDC);		/* B6 */ \
		arm_hardware_piok_altfn20((1U << 6), GPIO_AF_LTDC);		/* B7 */ \
		} while (0)
	/* управление состоянием сигнала DISP панели */
	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_SET_DISP(demode, state) do { \
		const uint32_t VSYNC = (1U << 9); \
		const uint32_t mask = (1U << 12); \
		while ((GPIOI->IDR & VSYNC) != 0) ; /* дождаться 0 */ \
		while ((GPIOI->IDR & VSYNC) == 0) ; /* дождаться 1 */ \
		arm_hardware_pioi_outputs(mask, ((state) != 0) * mask);	/* DE=DISP, pin 31 - можно менять только при VSYNC=1 */ \
	} while (0)
#endif /* LCDMODE_LTDC */

#endif /* ARM_STM32F4XX_TQFP144_CPUSTYLE_32F746G_H_INCLUDED */
