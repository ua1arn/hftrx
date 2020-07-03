/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Трансивер с DSP обработкой "Воронёнок-2" на процессоре
// STM32H743ZIT6, STM32F746ZGT6 кодеком NAU8822L и FPGA EP4CE22E22I7N
// с функцией DUAL WATCH
// 2xUSB
// Rmainunit_v5fm.pcb, Rmainunit_v5jm.pcb

#ifndef ARM_STM32F4XX_TQFP144_CPUSTYLE_STORCH_V3_H_INCLUDED
#define ARM_STM32F4XX_TQFP144_CPUSTYLE_STORCH_V3_H_INCLUDED 1

#define WITHSPI16BIT	1	/* возможно использование 16-ти битных слов при обмене по SPI */
#if defined(STM32H743xx)
	#define WITHSPI32BIT	1	/* возможно использование 32-ти битных слов при обмене по SPI */
#endif /* defined(STM32F767xx) */
#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
//#define WITHSPISW 	1	/* Использование программного управления SPI. Нельзя убирать эту строку - требуется явное отключение из-за конфликта с I2C */
#define WITHDMA2DHW		1	/* Использование DMA2D для формирования изображений	*/

//#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
//#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */

#define WITHI2SHW	1	/* Использование I2S - аудиокодек на I2S2 и I2S2_alt	*/
#define WITHSAI1HW	1	/* Использование SAI1 - FPGA или IF codec	*/
//#define WITHSAI2HW	1	/* Использование SAI2 - FPGA или IF codec	*/

#define WITHCPUDACHW	1	/* использование встроенного в процессор DAC */
#define WITHCPUADCHW 	1	/* использование встроенного в процессор ADC */

//#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
//#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */

#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */
#define WITHUSBDEV_VBUSSENSE	1	/* используется предопределенный вывод VBUS_SENSE */
//#define WITHUSBDEV_HSDESC	1	/* Требуется формировать дескрипторы как для HIGH SPEED */
//#define WITHUSBDEV_HIGHSPEEDULPI	1
//#define WITHUSBDEV_HIGHSPEEDPHYC	1

/* For H7 exist: Legacy defines */
//#define USB_OTG_HS                   USB1_OTG_HS
//#define USB_OTG_FS                   USB2_OTG_FS

#define WITHUSBHW_DEVICE	USB_OTG_HS	/* на этом устройстве поддерживается функциональность DEVICE	*/
//#define WITHUSBHW_HOST		USB_OTG_FS

//#define WITHUART1HW	1	/* PA9, PA10 Используется периферийный контроллер последовательного порта #1 */
#define WITHUART2HW	1	/* PD5, PD6 Используется периферийный контроллер последовательного порта #2 */

#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
#define WITHMODEM_CDC	1
//#define WITHCAT_USART2		1
#define WITHDEBUG_USART2	1
#define WITHNMEA_USART2		1	/* порт подключения GPS/GLONASS */

//#define WITHUAC2		1	/* UAC2 support */
#define WITHUSBUAC		1	/* использовать виртуальную звуковую плату на USB соединении */
#if (WITHRTS96 || WITHRTS192)
	#define WITHUSBUACIN2		1	/* формируются три канала передачи звука */
#endif /* (WITHRTS96 || WITHRTS192) */
//#define WITHUABUACOUTAUDIO48MONO	1	/* для уменьшения размера буферов в endpoints */

#define WITHUSBCDC		1	/* ACM использовать виртуальный последовательный порт на USB соединении */
#define WITHUSBHWCDC_N	2	/* количество виртуальных последовательных портов */
//#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */
//#define WITHUSBRNDIS	1	/* RNDIS использовать Remote NDIS на USB соединении */
//#define WITHUSBHID	1	/* HID использовать Human Interface Device на USB соединении */

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
	#define LS020_RS			(1u << 7)			// PF7 D7 signal

#elif LCDMODE_SPI_RN
	// эти контроллеры требуют только RESET

	#define LS020_RESET_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RESET			(1u << 6)			// PF6 D6 signal in HD44780 socket

#elif LCDMODE_SPI_RA
	// Эти контроллеры требуют RESET и RS
	// LCDMODE_UC1608

	#define LS020_RS_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RS			(1u << 7)			// PF7 D7 signal

	#define LS020_RESET_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RESET			(1u << 6)			// PF6 D6 signal in HD44780 socket

#elif LCDMODE_HD44780 && (LCDMODE_SPI == 0)

	#define WITHUSEPARALELDISPLAY	1

	// E (enable) bit
	#define LCD_STROBE_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LCD_STROBE_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LCD_STROBE_BIT			(1u << 10)	// PF10

	// RS (address, register select) bit
	#define LCD_RS_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LCD_RS_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define ADDRES_BIT				(1u << 8)	// PF8 - bit in RS port

	// WE (write enable) bit
	#define LCD_WE_PORT_S(v)		do { GPIOF->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LCD_WE_PORT_C(v)		do { GPIOF->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define WRITEE_BIT				(1u << 9)	// PF9 - bit in 
	
	// Выводы подключения ЖКИ индикатора WH2002 или аналогичного HD44780.
	#define LCD_DATA_INPUT			(GPIOF->IDR)
	#define LCD_DATAS_BITS			((1u << 7) | (1u << 6) | (1u << 5) | (1u << 4))	// PF7..PF4
	#define LCD_DATAS_BIT_LOW		4		// какой бит данных младший в слове считанном с порта

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
	#define ENCODER2_INPUT_PORT			(GPIOG->IDR) 
	#define ENCODER2_BITS ((1u << 10) | (1u << 9))		// PG10 & PG9
	#define ENCODER2_SHIFT 9

	// Выводы подключения енкодера
	#define ENCODER_INPUT_PORT			(GPIOG->IDR) 
	#define ENCODER_BITS ((1u << 12) | (1u << 11))		// PG12 & PG11
	#define ENCODER_SHIFT 11

	#define ENCODER_INITIALIZE() \
		do { \
			arm_hardware_piog_inputs(ENCODER_BITS); \
			arm_hardware_piog_updown(ENCODER_BITS, 0); \
			arm_hardware_piog_onchangeinterrupt(ENCODER_BITS, ENCODER_BITS, ENCODER_BITS, ARM_OVERREALTIME_PRIORITY); \
			arm_hardware_piog_inputs(ENCODER2_BITS); \
			arm_hardware_piog_updown(ENCODER2_BITS, 0); \
			arm_hardware_piog_onchangeinterrupt(0 * ENCODER2_BITS, ENCODER2_BITS, ENCODER2_BITS, ARM_OVERREALTIME_PRIORITY); \
		} while (0)

#endif

#if WITHI2SHW
	// Инициализируются I2S2 и I2S3
	#define I2S2HW_INITIALIZE() do { \
		arm_hardware_piob_altfn2(1U << 12,	AF_SPI2); /* PB12 I2S2_WS	*/ \
		arm_hardware_piob_altfn2(1U << 10,	AF_SPI2); /* PB10 I2S2_CK	*/ \
		arm_hardware_pioc_altfn2(1U << 3,	AF_SPI2); /* PC3 I2S2_SD - передача */ \
		arm_hardware_pioa_altfn2(1U << 15,	AF_SPI3); /* PA15 I2S3_WS	*/ \
		arm_hardware_piob_altfn2(1U << 3,	AF_SPI3); /* PB3 I2S3_CK	*/ \
		arm_hardware_piob_altfn2(1U << 2,	7 /* AF_7 */); /* PB2 I2S3_SD, - приём от кодека */ \
	} while (0)
#endif /* WITHSAI1HW */

#if WITHSAI1HW
	#define SAI1HW_INITIALIZE()	do { \
		/*arm_hardware_pioe_altfn20(1U << 2, AF_SAI); */	/* PE2 - SAI1_MCK_A - 12.288 MHz	*/ \
		arm_hardware_pioe_altfn2(1U << 4,	AF_SAI);			/* PE4 - SAI1_FS_A	- 48 kHz	*/ \
		arm_hardware_pioe_altfn20(1U << 5,	AF_SAI);			/* PE5 - SAI1_SCK_A	*/ \
		arm_hardware_pioe_altfn2(1U << 6,	AF_SAI);			/* PE6 - SAI1_SD_A	(i2s data to codec)	*/ \
		arm_hardware_pioe_altfn2(1U << 3,	AF_SAI);			/* PE3 - SAI1_SD_B	(i2s data from codec)	*/ \
	} while (0)
#endif /* WITHSAI1HW */

#if WITHSAI2HW
	/* 
	Поскольку блок SAI2 инициализируется как SLAVE с синхронизацией от SAI1,
	из внешних сигналов требуется только SAI2_SD_A
	*/
	#define SAI2HW_INITIALIZE()	do { \
		/* arm_hardware_pioe_altfn20(1U << 0, AF_SAI2); */	/* PE0 - SAI2_MCK_A - 12.288 MHz	*/ \
		/* arm_hardware_piod_altfn2(1U << 12, AF_SAI2); */	/* PD12 - SAI2_FS_A	- 48 kHz	*/ \
		/* arm_hardware_piod_altfn20(1U << 13, AF_SAI2); */	/* PD13 - SAI2_SCK_A	*/ \
		/* arm_hardware_piod_altfn2(1U << 11, AF_SAI2); */	/* PD11 - SAI2_SD_A	(i2s data to codec)	*/ \
		/* arm_hardware_pioe_altfn2(1U << 11, AF_SAI2);	*/ /* PE11 - SAI2_SD_B	(i2s data from codec)	*/ \
	} while (0)
#endif /* WITHSAI1HW */

/* Распределение битов в ARM контроллерах */

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
			arm_hardware_piod_updown(1U << 2, 0);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(1U << 12, 0);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(1U << 8, 0);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_updown(1U << 9, 0);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_updown(1U << 10, 0);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_updown(1U << 11, 0);	/* PC11 - SDIO_D3	*/ \
		} while (0)
		/* отключить процессор от SD карты - чтобы при выполнении power cycle не возникало фантомное питание через сигналы управления. */
		#define HARDWARE_SDIO_HANGOFF()	do { \
			arm_hardware_piod_inputs(1U << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_inputs(1U << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_inputs(1U << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_inputs(1U << 9);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_inputs(1U << 10);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_inputs(1U << 11);	/* PC11 - SDIO_D3	*/ \
			arm_hardware_piod_updown(0, 1U << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(0, 1U << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(0, 1U << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_updown(0, 1U << 9);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_updown(0, 1U << 10);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_updown(0, 1U << 11);	/* PC11 - SDIO_D3	*/ \
		} while (0)
	#else /* WITHSDHCHW4BIT */
		#define HARDWARE_SDIO_INITIALIZE()	do { \
			arm_hardware_piod_altfn50(1U << 2, AF_SDIO);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_altfn50(1U << 12, AF_SDIO);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_altfn50(1U << 8, AF_SDIO);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_piod_updown(1U << 2, 0);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(1U << 12, 0);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(1U << 8, 0);	/* PC8 - SDIO_D0	*/ \
		} while (0)
		/* отключить процессор от SD карты - чтобы при выполнении power cycle не возникало фантомное питание через сигналы управления. */
		#define HARDWARE_SDIO_HANGOFF()	do { \
			arm_hardware_piod_inputs(1U << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_inputs(1U << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_inputs(1U << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_piod_updown(0, 1U << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(0, 1U << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(0, 1U << 8);	/* PC8 - SDIO_D0	*/ \
		} while (0)
	#endif /* WITHSDHCHW4BIT */

	#define HARDWARE_SDIO_WP_BIT	(1U << 1)	/* PD1 - SDIO_WP */
	#define HARDWARE_SDIO_CD_BIT	(1U << 0)	/* PD0 - SDIO_SENSE */

	#define HARDWARE_SDIOSENSE_INITIALIZE()	do { \
			arm_hardware_piod_inputs(HARDWARE_SDIO_WP_BIT); /* PD1 - SDIO_WP */ \
			arm_hardware_piod_updown(HARDWARE_SDIO_WP_BIT, 0); \
			arm_hardware_piod_inputs(HARDWARE_SDIO_CD_BIT); /* PD0 - SDIO_SENSE */ \
			arm_hardware_piod_updown(HARDWARE_SDIO_CD_BIT, 0); \
	} while (0)

	#define HARDWARE_SDIOSENSE_CD() ((GPIOD->IDR & HARDWARE_SDIO_CD_BIT) == 0)	/* получить состояние датчика CARD PRESENT */
	#define HARDWARE_SDIOSENSE_WP() ((GPIOD->IDR & HARDWARE_SDIO_WP_BIT) != 0)	/* получить состояние датчика CARD WRITE PROTECT */

	#define HARDWARE_SDIOPOWER_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define HARDWARE_SDIOPOWER_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define HARDWARE_SDIOPOWER_BIT (1u << 1)	/* PE1 */
	/* если питание SD CARD управляется прямо с процессора */
	#define HARDWARE_SDIOPOWER_INITIALIZE()	do { \
		arm_hardware_pioe_outputs2m(HARDWARE_SDIOPOWER_BIT, HARDWARE_SDIOPOWER_BIT); /* питание выключено */ \
		} while (0)
	/* parameter on not zero for powering SD CARD */
	#define HARDWARE_SDIOPOWER_SET(on)	do { \
		if ((on) != 0) \
			HARDWARE_SDIOPOWER_C(HARDWARE_SDIOPOWER_BIT); \
		else \
			HARDWARE_SDIOPOWER_S(HARDWARE_SDIOPOWER_BIT); \
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

	// TXDISABLE input - PB11
	#define TXDISABLE_TARGET_PIN				(GPIOB->IDR)
	#define TXDISABLE_BIT_TXDISABLE				(1U << 11)		// PB11
	// получить бит запрета передачи (от усилителя мощности)
	#define HARDWARE_GET_TXDISABLE() ((TXDISABLE_TARGET_PIN & TXDISABLE_BIT_TXDISABLE) != 0)
	#define TXDISABLE_INITIALIZE() \
		do { \
			arm_hardware_piob_inputs(TXDISABLE_BIT_TXDISABLE); \
			arm_hardware_piob_updown(0, TXDISABLE_BIT_TXDISABLE); \
		} while (0)

	// PTT input - PD10
	#define PTT_TARGET_PIN				(GPIOD->IDR)
	#define PTT_BIT_PTT					(1U << 12)		// PD12
	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() \
		do { \
			arm_hardware_piod_inputs(PTT_BIT_PTT); \
			arm_hardware_piod_updown(PTT_BIT_PTT, 0); \
		} while (0)

	//#if ! WITHUSEPARALELDISPLAY
		#if WITHUART2HW
			//#warning Disable WITHUART2HW
			#define TUNE_INITIALIZE() \
				do { \
				} while (0)
		#else /* WITHUART2HW */
			// TUNE input - PD5
			#define TUNE_TARGET_PIN				(GPIOD->IDR)
			#define TUNE_BIT_TUNE					(1U << 5)		// PD5
			#define HARDWARE_GET_TUNE() ((TUNE_TARGET_PIN & TUNE_BIT_TUNE) == 0)
			#define TUNE_INITIALIZE() \
				do { \
					arm_hardware_piod_inputs(TUNE_BIT_TUNE); \
					arm_hardware_piod_updown(TUNE_BIT_TUNE, 0); \
				} while (0)
		#endif /* WITHUART2HW */
	//#endif /* ! WITHUSEPARALELDISPLAY */
#else /* WITHTX */
	#define TXDISABLE_INITIALIZE() \
		do { \
		} while (0)
	#define PTT_INITIALIZE() \
		do { \
		} while (0)
#endif /* WITHTX */

#if WITHELKEY
	// Electronic key inputs
	#define ELKEY_TARGET_PIN			(GPIOD->IDR)
	#define ELKEY_BIT_LEFT				(1U << 10)		// PD10
	#define ELKEY_BIT_RIGHT				(1U << 11)		// PD11

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

#if WITHSPIHW || WITHSPISW
	// Набор определений для работы без внешнего дешифратора
	#define SPI_ALLCS_PORT_S(v)	do { GPIOG->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define SPI_ALLCS_PORT_C(v)	do { GPIOG->BSRR = BSRR_C(v); __DSB(); } while (0)

	#define SPI_CSEL_PG15	(1U << 15)	// PG15 ext1
	#define SPI_CSEL_PG14	(1U << 14)	// PG14 ext2
	#define SPI_CSEL_PG8	(1U << 8)	// PG8 nvmem FM25L16B
	#define SPI_CSEL_PG7	(1U << 7)	// PG7 board control registers chain
	#define SPI_CSEL_PG6	(1U << 6)	// PG6 on-board codec1 NAU8822L
	#define SPI_CSEL_PG1	(1U << 1)	// PG0 FPGA control registers CS

	// Здесь должны быть перечислены все биты формирования CS в устройстве.
	#define SPI_ALLCS_BITS ( \
		SPI_CSEL_PG15	| 	/* PG15 ext1 */ \
		SPI_CSEL_PG14	| 	/* PG14 ext2 */ \
		SPI_CSEL_PG8	| 	/* PG8 nvmem FM25L16B */ \
		SPI_CSEL_PG7	| 	/* PG7 board control registers chain */ \
		SPI_CSEL_PG6	| 	/* PG6 on-board codec1 NAU8822L */ \
		SPI_CSEL_PG1	| 	/* PG1 FPGA control registers CS */ \
		0)

	#define SPI_ALLCS_BITSNEG 0		// Выходы, активные при "1"

	//#define SPI_NAEN_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	//#define SPI_NAEN_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)

	//#define SPI_NAEN_BIT (1u << 7)		// * PE7 used

	/* инициализация лиий выбора периферийных микросхем */
	#define SPI_ALLCS_INITIALIZE() \
		do { \
			arm_hardware_piog_outputs(SPI_ALLCS_BITS, SPI_ALLCS_BITS ^ SPI_ALLCS_BITSNEG); \
		} while (0)

	// MOSI & SCK port
	#define SPI_TARGET_SCLK_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define SPI_TARGET_SCLK_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define	SPI_SCLK_BIT			(1U << 5)	// * PA5 бит, через который идет синхронизация SPI

	#define SPI_TARGET_MOSI_PORT_C(v)	do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define SPI_TARGET_MOSI_PORT_S(v)	do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define	SPI_MOSI_BIT			(1U << 5)	// * PB5 бит, через который идет вывод (или ввод в случае двунаправленного SPI).

	#define SPI_TARGET_MISO_PIN		(GPIOB->IDR)
	#define	SPI_MISO_BIT			(1U << 4)	// * PB4 бит, через который идет ввод с SPI.

	#define SPIIO_INITIALIZE() do { \
			arm_hardware_pioa_outputs(SPI_SCLK_BIT, SPI_SCLK_BIT); \
			arm_hardware_piob_outputs(SPI_MOSI_BIT, SPI_MOSI_BIT); \
			arm_hardware_piob_inputs(SPI_MISO_BIT); \
		} while (0)
	#define HARDWARE_SPI_CONNECT() do { \
			arm_hardware_piob_altfn20(SPI_MOSI_BIT | SPI_MISO_BIT, AF_SPI1); /* В этих процессорах и входы и выходы перекдючаются на ALT FN */ \
			arm_hardware_pioa_altfn20(SPI_SCLK_BIT, AF_SPI1); /* В этих процессорах и входы и выходы перекдючаются на ALT FN */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT() do { \
			arm_hardware_pioa_outputs(SPI_SCLK_BIT, SPI_SCLK_BIT); \
			arm_hardware_piob_outputs(SPI_MOSI_BIT, SPI_MOSI_BIT); \
			arm_hardware_piob_inputs(SPI_MISO_BIT); \
		} while (0)
	#define HARDWARE_SPI_CONNECT_MOSI() do { \
			arm_hardware_piob_altfn20(SPI_MOSI_BIT, AF_SPI1);	/* PIO disable for MOSI bit (SD CARD read support) */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
			arm_hardware_piob_outputs(SPI_MOSI_BIT, SPI_MOSI_BIT);	/* PIO enable for MOSI bit (SD CARD read support)  */ \
		} while (0)

	#define HARDWARE_USART1_INITIALIZE() do { \
			arm_hardware_pioa_altfn2((1U << 9) | (1U << 10), AF_USART1); /* PA9: TX DATA line (2 MHz), PA10: RX data line */ \
			arm_hardware_pioa_updown((1U << 10), 0);	/* PA10: pull-up RX data */ \
		} while (0)
	#define HARDWARE_USART2_INITIALIZE() do { \
			arm_hardware_piod_altfn2((1U << 5) | (1U << 6), AF_USART2); /* PD5: TX DATA line (2 MHz), PD6: RX data line */ \
			arm_hardware_piod_updown((1U << 6), 0);	/* PD6: pull-up RX data */ \
		} while (0)

#endif

#define HARDWARE_SIDETONE_INITIALIZE() do { \
	} while (0)

#if ! WITHUSEPARALELDISPLAY
	/* PF10: pull-up second encoder button */
	#define KBD_MASK (1U << 10)	// PF10
	#define KBD_TARGET_PIN (GPIOF->IDR)

	#define HARDWARE_KBD_INITIALIZE() do { \
			arm_hardware_piof_inputs(KBD_MASK); \
			arm_hardware_piof_updown(KBD_MASK, 0);	/* PF10: pull-up second encoder button */ \
		} while (0)
#else /* ! WITHUSEPARALELDISPLAY */
	#define HARDWARE_KBD_INITIALIZE() do { \
		} while (0)
#endif /* ! WITHUSEPARALELDISPLAY */

#if 1 // WITHTWISW
	#define TARGET_TWI_TWCK_PORT_C(v) do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_TWI_TWD_PORT_C(v) do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v) do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_TWI_TWCK_PIN		(GPIOB->IDR)
	#define TARGET_TWI_TWD_PIN		(GPIOB->IDR)
	#define TARGET_TWI_TWCK		(1u << 6)		// PB6 SCL
	#define TARGET_TWI_TWD		(1u << 7)		// PB7 SDA

	// Инициализация битов портов ввода-вывода для программной реализации I2C
	#define	TWISOFT_INITIALIZE() do { \
			arm_hardware_piob_opendrain(TARGET_TWI_TWCK | TARGET_TWI_TWD, TARGET_TWI_TWCK | TARGET_TWI_TWD); \
		} while (0) 
	// Инициализация битов портов ввода-вывода для аппаратной реализации I2C
	// присоединение выводов к периферийному устройству
	#define	TWIHARD_INITIALIZE() do { \
			arm_hardware_piob_periphopendrain_altfn2(TARGET_TWI_TWCK | TARGET_TWI_TWD, AF_I2C1);	/* AF=4 */ \
		} while (0) 


#endif // WITHTWISW

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

	#define FPGA_INIT_DONE_INPUT	(GPIOE->IDR)
	#define FPGA_INIT_DONE_BIT		(1UL << 12)	/* PE12 bit conneced to INIT_DONE pin ALTERA FPGA */

	/* Инициадизация выводов GPIO процессора для получения состояния и управлением загрузкой FPGA */
	#define HARDWARE_FPGA_LOADER_INITIALIZE() do { \
			arm_hardware_pioe_outputs(FPGA_NCONFIG_BIT, FPGA_NCONFIG_BIT); \
			arm_hardware_pioe_inputs(FPGA_NSTATUS_BIT); \
			arm_hardware_pioe_inputs(FPGA_CONF_DONE_BIT); \
			arm_hardware_pioe_inputs(FPGA_INIT_DONE_BIT); \
		} while (0)

	/* Проверяем, проинициализировалась ли FPGA (вошла в user mode). */
	/*
		After the option bit to enable INIT_DONE is programmed into the device (during the first
		frame of configuration data), the INIT_DONE pin goes low.
		When initialization is complete, the INIT_DONE pin is released and pulled high. 
		This low-to-high transition signals that the device has entered user mode.
	*/
	#define HARDWARE_FPGA_IS_USER_MODE() ((FPGA_INIT_DONE_INPUT & FPGA_INIT_DONE_BIT) != 0)

#endif /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS */

#if WITHDSPEXTFIR
	// Биты доступа к массиву коэффициентов FIR фильтра в FPGA
	#define TARGET_FPGA_FIR_CS_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR_CS_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR_CS_BIT (1U << 7)	/* PE7 - fir CLK */

	#define TARGET_FPGA_FIR1_WE_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR1_WE_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR1_WE_BIT (1U << 8)	/* PE8 - fir1 WE */

	#define TARGET_FPGA_FIR2_WE_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR2_WE_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR2_WE_BIT (1U << 9)	/* PE9 - fir2 WE */

	#define TARGET_FPGA_FIR_INITIALIZE() do { \
			arm_hardware_pioe_outputs(TARGET_FPGA_FIR1_WE_BIT, TARGET_FPGA_FIR1_WE_BIT); \
			arm_hardware_pioe_outputs(TARGET_FPGA_FIR2_WE_BIT, TARGET_FPGA_FIR2_WE_BIT); \
			arm_hardware_pioe_outputs(TARGET_FPGA_FIR_CS_BIT, TARGET_FPGA_FIR_CS_BIT); \
		} while (0)
#endif /* WITHDSPEXTFIR */

#if 1
	/* получение состояния переполнения АЦП */
	#define TARGET_FPGA_OVF_INPUT		(GPIOF->IDR)
	#define TARGET_FPGA_OVF_BIT			(1u << 12)	// PF12
	#define TARGET_FPGA_OVF_GET			((TARGET_FPGA_OVF_INPUT & TARGET_FPGA_OVF_BIT) == 0)	// 1 - overflow active
	#define TARGET_FPGA_OVF_INITIALIZE() do { \
				arm_hardware_piof_inputs(TARGET_FPGA_OVF_BIT); \
			} while (0)
#endif

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
	#define TARGET_USBFS_VBUSON_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_USBFS_VBUSON_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_USBFS_VBUSON_BIT (1U << 8)	// PA8 - нулём включение питания для device
	/**USB_OTG_FS GPIO Configuration    
	PA9     ------> USB_OTG_FS_VBUS
	PA11     ------> USB_OTG_FS_DM
	PA12     ------> USB_OTG_FS_DP 
	*/
	#define	USBD_FS_INITIALIZE() do { \
		arm_hardware_pioa_altfn50((1U << 11) | (1U << 12), AF_OTGFS);			/* PA11, PA12 - USB_OTG_FS	*/ \
		arm_hardware_pioa_inputs(1U << 9);		/* PA9 - USB_OTG_FS_VBUS */ \
		arm_hardware_pioa_updownoff((1U << 9) | (1U << 11) | (1U << 12)); \
		arm_hardware_pioa_outputs(TARGET_USBFS_VBUSON_BIT, TARGET_USBFS_VBUSON_BIT); \
		} while (0)

	#define TARGET_USBFS_VBUSON_SET(on)	do { \
		if ((on) != 0) \
			TARGET_USBFS_VBUSON_PORT_C(TARGET_USBFS_VBUSON_BIT); \
		else \
			TARGET_USBFS_VBUSON_PORT_S(TARGET_USBFS_VBUSON_BIT); \
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

	/* макроопределение, которое должно включить в себя все инициализации */
	#define	HARDWARE_INITIALIZE() do { \
		HARDWARE_SIDETONE_INITIALIZE(); \
		HARDWARE_KBD_INITIALIZE(); \
		HARDWARE_DAC_INITIALIZE(); \
		TUNE_INITIALIZE(); \
		TXDISABLE_INITIALIZE(); \
		} while (0)

#endif /* ARM_STM32F4XX_TQFP144_CPUSTYLE_STORCH_V3_H_INCLUDED */
