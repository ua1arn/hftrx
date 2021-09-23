/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Трансивер с DSP обработкой "Аист" на процессоре STM32MP1xx
// rmainunit_sv9.pcb STM32MP157AAC - модуль MYC-YA157-V2, 2xUSB, NAU8822L и FPGA EP4CE22E22I7N

#ifndef ARM_STM32MP1_LFBGA354_CPUSTYLE_STORCH_V9C_H_INCLUDED
#define ARM_STM32MP1_LFBGA354_CPUSTYLE_STORCH_V9C_H_INCLUDED 1

#define WITHSPI16BIT	1	/* возможно использование 16-ти битных слов при обмене по SPI */
#define WITHSPI32BIT	1	/* возможно использование 32-ти битных слов при обмене по SPI */
#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
//#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
//#define WITHSPISW 	1	/* Использование программного управления SPI. Нельзя убирать эту строку - требуется явное отключение из-за конфликта с I2C */
//#define WITHDMA2DHW		1	/* Использование DMA2D для формирования изображений	- у STM32MP1 его нет */

//#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */
#if WITHINTEGRATEDDSP
	#define WITHI2SHW	1	/* Использование I2S - аудиокодек на I2S2 и I2S2_alt или I2S2 и I2S3	*/
	//#define WITHSAI1HW	1	/* Использование SAI1 - FPGA или IF codec	*/
	#define WITHSAI2HW	1	/* Использование SAI2 - FPGA или IF codec	*/
	//#define WITHSAI3HW	1	/* Использование SAI3 - FPGA скоростной канал записи спктра	*/
#endif /* WITHINTEGRATEDDSP */

#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */
//#define WITHETHHW 1	/* Hardware Ethernet controller */

//#define WITHUART1HW	1	/* PA9, PA10 Используется периферийный контроллер последовательного порта #1 */

#if WITHDEBUG
	#define WITHUART4HW	1	/* PD5, PD6 Используется периферийный контроллер последовательного порта #2 */
	#define WITHUARTFIFO	1	/* испольование FIFO */
#endif /* WITHDEBUG */

//#define WITHCAT_USART1		1
#define WITHDEBUG_USART4	1
#define WITHNMEA_USART4		1	/* порт подключения GPS/GLONASS */


#if WITHISBOOTLOADER

	//#define WIHSPIDFSW	1	/* программное обслуживание DATA FLASH */
	#define WIHSPIDFHW		1	/* аппаратное обслуживание DATA FLASH */
	//#define WIHSPIDFHW2BIT	1	/* аппаратное обслуживание DATA FLASH с подддержкой QSPI подключения по 2-м проводам */
	#define WIHSPIDFHW4BIT	1	/* аппаратное обслуживание DATA FLASH с подддержкой QSPI подключения по 4-м проводам */

	#define WITHSDRAMHW	1		/* В процессоре есть внешняя память */
	//#define WITHSDRAM_PMC1	1	/* power management chip */

	//#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
	//#define WITHGPUHW	1	/* Graphic processor unit */
	//#define WITHEHCIHW	1	/* USB_EHCI controller */
	#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */
	#define USBPHYC_MISC_SWITHOST_VAL 0		// 0 or 1 - value for USBPHYC_MISC_SWITHOST field. 0: Select OTG controller for 2nd PHY port, 1: Select Host controller for 2nd PHY port
	#define USBPHYC_MISC_PPCKDIS_VAL 0x00

	#define WITHUSBHW_DEVICE	USB_OTG_HS	/* на этом устройстве поддерживается функциональность DEVICE	*/
	#define WITHUSBDEV_VBUSSENSE	1		/* используется предопределенный вывод OTG_VBUS */
	#define WITHUSBDEV_HSDESC	1			/* Требуется формировать дескрипторы как для HIGH SPEED */
	//#define WITHUSBDEV_HIGHSPEEDULPI	1
	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USBH_HS_DP & USBH_HS_DM
	#define WITHUSBDEV_DMAENABLE 1

	//#define WITHUSBHW_HOST		USB_OTG_HS
	#define WITHUSBHOST_HIGHSPEEDPHYC	1	// UTMI -> USB_DP2 & USB_DM2
	//#define WITHUSBHOST_DMAENABLE 1

	//#define WITHEHCIHW	1	/* USB_EHCI controller */
	//#define WITHUSBHW_EHCI		USB1_EHCI
	//#define WITHEHCIHW_EHCIPORT 0	// 0 - use 1st PHY port, 1 - 2nd PHY port


	#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
	#define WITHMODEM_CDC	1

	//#define WITHUSBUAC		1	/* использовать виртуальную звуковую плату на USB соединении */
	//#define WITHUSBUACIN2		1	/* формируются три канала передачи звука */
	//#define WITHUABUACOUTAUDIO48MONO	1	/* для уменьшения размера буферов в endpoints */

	//#define WITHUSBCDCACM		1	/* ACM использовать виртуальный последовательный порт на USB соединении */
	//#define WITHUSBCDCACM_N	1	/* количество виртуальных последовательных портов */
    //#define WITHUSBCDCACMINTSHARING 1    /* Использование общей notification endpoint на всех CDC ACM устрйоствах */
	//#define WITHUSBHID	1	/* HID использовать Human Interface Device на USB соединении */
	#define WITHUSBDFU	1	/* DFU USB Device Firmware Upgrade support */
	#define WITHMOVEDFU 1	// Переместить интерфейс DFU в область меньших номеров. Утилита dfu-util 0.9 не работает с DFU на интерфейсе с индексом 10
	#define WITHUSBWCID	1

	//#define WITHLWIP 1
	//#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
	//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */
	//#define WITHUSBRNDIS	1	/* RNDIS использовать Remote NDIS на USB соединении */

#else /* WITHISBOOTLOADER */

	//#define WIHSPIDFSW	1	/* программное обслуживание DATA FLASH */
	#define WIHSPIDFHW		1	/* аппаратное обслуживание DATA FLASH */
	//#define WIHSPIDFHW2BIT	1	/* аппаратное обслуживание DATA FLASH с подддержкой QSPI подключения по 2-м проводам */
	#define WIHSPIDFHW4BIT	1	/* аппаратное обслуживание DATA FLASH с подддержкой QSPI подключения по 4-м проводам */

	#define WITHMDMAHW		1	/* Использование MDMA для формирования изображений */
	//#define WITHCPUDACHW	1	/* использование встроенного в процессор DAC */
	#define WITHCPUADCHW 	1	/* использование встроенного в процессор ADC */

	#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
	//#define WITHGPUHW	1	/* Graphic processor unit */
	#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */
	#define USBPHYC_MISC_SWITHOST_VAL 0		// 0 or 1 - value for USBPHYC_MISC_SWITHOST field. 0: Select OTG controller for 2nd PHY port, 1: Select Host controller for 2nd PHY port
	#define USBPHYC_MISC_PPCKDIS_VAL 0x00

	#define WITHUSBHW_DEVICE	USB_OTG_HS	/* на этом устройстве поддерживается функциональность DEVICE	*/
	#define WITHUSBDEV_VBUSSENSE	1		/* используется предопределенный вывод OTG_VBUS */
	//#define WITHUSBDEV_HSDESC	1			/* Требуется формировать дескрипторы как для HIGH SPEED */
	//#define WITHUSBDEV_HIGHSPEEDULPI	1	// ULPI
	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USB_DP2 & USB_DM2
	#define WITHUSBDEV_DMAENABLE 1

	//#define WITHUSBHW_HOST		USB_OTG_HS
	#define WITHUSBHOST_HIGHSPEEDPHYC	1	// UTMI -> USB_DP2 & USB_DM2
	//#define WITHUSBHOST_DMAENABLE 1

	#define WITHEHCIHW	1	/* USB_EHCI controller */
	#define WITHUSBHW_EHCI		USB1_EHCI
	#define WITHEHCIHW_EHCIPORT 0	// 0 - use 1st PHY port, 1 - 2nd PHY port

	#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
	#define WITHMODEM_CDC	1

	#if WITHINTEGRATEDDSP

		//#define WITHUAC2		1	/* UAC2 support */
		#define WITHUSBUACINOUT	1	/* совмещённое усройство ввода/вывода (без спектра) */
		#define WITHUSBUACOUT		1	/* использовать виртуальную звуковую плату на USB соединении */
		#if WITHRTS96 || WITHRTS192
			#define WITHUSBUACIN	1
			#define WITHUSBUACIN2		1	/* формируются три канала передачи звука */
		#else /* WITHRTS96 || WITHRTS192 */
			#define WITHUSBUACIN	1
		#endif /* WITHRTS96 || WITHRTS192 */
		//#define WITHUABUACOUTAUDIO48MONO	1	/* для уменьшения размера буферов в endpoints */
	#endif /* WITHINTEGRATEDDSP */

	#define WITHUSBCDCACM		1	/* ACM использовать виртуальный последовательный порт на USB соединении */
	#define WITHUSBCDCACM_N	2	/* количество виртуальных последовательных портов */
    //#define WITHUSBCDCACMINTSHARING 1    /* Использование общей notification endpoint на всех CDC ACM устрйоствах */
	#if WITHLWIP
		#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
		//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */
		//#define WITHUSBRNDIS	1	/* RNDIS использовать Remote NDIS на USB соединении */
	#endif /* WITHLWIP */
	//#define WITHUSBHID	1	/* HID использовать Human Interface Device на USB соединении */

	#define WITHUSBDFU	1	/* DFU USB Device Firmware Upgrade support */
	#define WITHMOVEDFU 1	// Переместить интерфейс DFU в область меньших номеров. Утилита dfu-util 0.9 не работает с DFU на интерфейсе с индексом 10
	#define WITHUSBWCID	1

#endif /* WITHISBOOTLOADER */


#define LS020_RS_INITIALIZE() \
	do { \
		arm_hardware_piod_outputs2m(LS020_RS, LS020_RS); /* PD3 */ \
	} while (0)

#define LS020_RESET_INITIALIZE() \
	do { \
		arm_hardware_piod_outputs2m(LS020_RESET, LS020_RESET); /* PD4 */ \
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

	#define LS020_RS_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); (void) GPIOD->BSRR; } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); (void) GPIOD->BSRR; } while (0)
	#define LS020_RS			(1u << 3)			// PD3 signal

#elif LCDMODE_SPI_RN
	// эти контроллеры требуют только RESET

	#define LS020_RESET_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); (void) GPIOD->BSRR; } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); (void) GPIOD->BSRR; } while (0)
	#define LS020_RESET			(1u << 4)			// PD4 signal

#elif LCDMODE_SPI_RA
	// Эти контроллеры требуют RESET и RS
	// LCDMODE_UC1608

	#define LS020_RS_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); (void) GPIOD->BSRR; } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); (void) GPIOD->BSRR; } while (0)
	#define LS020_RS			(1u << 3)			// PD3 signal

	#define LS020_RESET_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); (void) GPIOD->BSRR; } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); (void) GPIOD->BSRR; } while (0)
	#define LS020_RESET			(1u << 2)			// PD4 signal

#elif LCDMODE_HD44780 && (LCDMODE_SPI == 0)

	#error Unsupported LCDMODE_HD44780

#endif

#if WITHENCODER

	// Выводы подключения енкодера #1
	#define ENCODER_INPUT_PORT	(GPIOE->IDR)
	#define ENCODER_BITA		(1uL << 13)		// PE1
	#define ENCODER_BITB		(1uL << 9)		// PE0

	// Выводы подключения енкодера #2
	#define ENCODER2_INPUT_PORT	(GPIOE->IDR)
	#define ENCODER2_BITA		(1uL << 15)		// PE4
	#define ENCODER2_BITB		(1uL << 14)		// PE6


	#define ENCODER_BITS		(ENCODER_BITA | ENCODER_BITB)
	#define ENCODER2_BITS		(ENCODER2_BITA | ENCODER2_BITB)

	#define ENCODER_INITIALIZE() \
		do { \
			arm_hardware_pioe_inputs(ENCODER_BITS); \
			arm_hardware_pioe_updown(ENCODER_BITS, 0); \
			arm_hardware_pioe_onchangeinterrupt(ENCODER_BITS, ENCODER_BITS, ENCODER_BITS, ARM_OVERREALTIME_PRIORITY, TARGETCPU_OVRT); \
			arm_hardware_pioe_inputs(ENCODER2_BITS); \
			arm_hardware_pioe_updown(ENCODER2_BITS, 0); \
			arm_hardware_pioe_onchangeinterrupt(0 * ENCODER2_BITS, ENCODER2_BITS, ENCODER2_BITS, ARM_OVERREALTIME_PRIORITY, TARGETCPU_OVRT); \
		} while (0)

#endif

#if WITHI2SHW
	// Инициализируются I2S2 в дуплексном режиме.
	#define I2S2HW_INITIALIZE() do { \
		arm_hardware_pioi_altfn2(1uL << 0,	AF_SPI2); /* PI0 I2S2_WS	*/ \
		arm_hardware_pioi_updown(0, 1uL << 0); \
		arm_hardware_pioi_altfn2(1uL << 1,	AF_SPI2); /* PI1 I2S2_CK	*/ \
		arm_hardware_pioi_updown(0, 1uL << 1); \
		arm_hardware_pioi_altfn2(1uL << 3,	AF_SPI2); /* PI3 I2S2_SDO - приём от кодека */ \
		arm_hardware_pioi_updown(0, 1uL << 3); \
		arm_hardware_pioi_altfn2(1uL << 2,	AF_SPI2); /* PI2 I2S2_SDI, - передача */ \
		arm_hardware_pioi_updown(0, 1uL << 2); \
		arm_hardware_pioc_altfn2(1uL << 6,	AF_SPI2); /* PC6 I2S2_MCK */ \
		arm_hardware_pioc_updown(0, 1uL << 6); \
	} while (0)
#endif /* WITHI2SHW */

	// для предотвращения треска от оставшегося инициализированным кодека
	#define I2S2HW_POOLDOWN() do { \
		arm_hardware_pioi_inputs(1uL << 0); /* PI0 I2S2_WS	*/ \
		arm_hardware_pioi_updown(0, 1uL << 0); \
		arm_hardware_pioi_inputs(1uL << 1); /* PI1 I2S2_CK	*/ \
		arm_hardware_pioi_updown(0, 1uL << 1); \
		arm_hardware_pioi_inputs(1uL << 3); /* PI3 I2S2_SDO - передача */ \
		arm_hardware_pioi_updown(0, 1uL << 3); \
		arm_hardware_pioi_inputs(1uL << 2); /* PI2 I2S2_SDI, - приём от кодека */ \
		arm_hardware_pioi_updown(0, 1uL << 2); \
		arm_hardware_pioc_inputs(1uL << 6); /* PC6 I2S2_MCK */ \
		arm_hardware_pioc_updown(0, 1uL << 6); \
	} while (0)

#if WITHSAI2HW
	/*
	 *
	 */
	#define SAI2HW_INITIALIZE()	do { \
		/*arm_hardware_pioe_altfn20(0 * 1uL << 2, AF_SAI); */	/* PExx - SAI2_MCK_A - 12.288 MHz	*/ \
		arm_hardware_pioi_altfn2(1uL << 4,	AF_SAI2);			/* PI7 - SAI2_FS_A	- 48 kHz	*/ \
		arm_hardware_piod_altfn20(1uL << 13, AF_SAI2);			/* PD13 - SAI2_SCK_A	*/ \
		arm_hardware_piod_altfn2(1uL << 6,	AF_SAI2);			/* PI6 - SAI2_SD_A	(i2s data to codec)	*/ \
		arm_hardware_pioe_altfn2(1uL << 11,	AF_SAI2);			/* PE11 - SAI2_SD_B	(i2s data from codec)	*/ \
		arm_hardware_pioi_altfn20(1uL << 11, AF_SPI1);		 /* PI11 I2S_CKIN AF_5 */ \
		arm_hardware_pioe_updown(1uL << 11, 0); \
	} while (0)
#endif /* WITHSAI2HW */

/* Распределение битов в ARM контроллерах */

#if (WITHCAT && WITHCAT_USART4)
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

#endif /* (WITHCAT && WITHCAT_USART4) */

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
			arm_hardware_piod_altfn50(1uL << 2, AF_SDIO);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_altfn50(1uL << 12, AF_SDIO);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_altfn50(1uL << 8, AF_SDIO);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_altfn50(1uL << 9, AF_SDIO);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_altfn50(1uL << 10, AF_SDIO);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_altfn50(1uL << 11, AF_SDIO);	/* PC11 - SDIO_D3	*/ \
		} while (0)
		/* отключить процессор от SD карты - чтобы при выполнении power cycle не возникало фантомное питание через сигналы управления. */
		#define HARDWARE_SDIO_HANGOFF()	do { \
			arm_hardware_piod_inputs(1uL << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_inputs(1uL << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_inputs(1uL << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_inputs(1uL << 9);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_inputs(1uL << 10);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_inputs(1uL << 11);	/* PC11 - SDIO_D3	*/ \
			arm_hardware_piod_updown(0, 1uL << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(0, 1uL << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(0, 1uL << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_updown(0, 1uL << 9);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_updown(0, 1uL << 10);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_updown(0, 1uL << 11);	/* PC11 - SDIO_D3	*/ \
		} while (0)
	#else /* WITHSDHCHW4BIT */
		#define HARDWARE_SDIO_INITIALIZE()	do { \
			arm_hardware_piod_altfn50(1uL << 2, AF_SDIO);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_altfn50(1uL << 12, AF_SDIO);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_altfn50(1uL << 8, AF_SDIO);	/* PC8 - SDIO_D0	*/ \
		} while (0)
		/* отключить процессор от SD карты - чтобы при выполнении power cycle не возникало фантомное питание через сигналы управления. */
		#define HARDWARE_SDIO_HANGOFF()	do { \
			arm_hardware_piod_inputs(1uL << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_inputs(1uL << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_inputs(1uL << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_piod_updown(0, 1uL << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(0, 1uL << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(0, 1uL << 8);	/* PC8 - SDIO_D0	*/ \
		} while (0)
	#endif /* WITHSDHCHW4BIT */

	#define HARDWARE_SDIO_WP_BIT	0//(1U << 8)	/* Pxx - SDIO_WP */
	#define HARDWARE_SDIO_CD_BIT	(1U << 7)	/* PB7 - SDIO_SENSE */

	#define HARDWARE_SDIOSENSE_CD() ((GPIOB->IDR & HARDWARE_SDIO_CD_BIT) == 0)	/* получить состояние датчика CARD PRESENT */
	#define HARDWARE_SDIOSENSE_WP() 0//((GPIOG->IDR & HARDWARE_SDIO_WP_BIT) != 0)	/* получить состояние датчика CARD WRITE PROTECT (0 - write enabled) */

	#define HARDWARE_SDIOSENSE_INITIALIZE()	do { \
			arm_hardware_piog_inputs(HARDWARE_SDIO_WP_BIT); /* Pxx - SDIO_WP */ \
			arm_hardware_piog_updown(HARDWARE_SDIO_WP_BIT, 0); \
			arm_hardware_piob_inputs(HARDWARE_SDIO_CD_BIT); /* PB7 - SDIO_SENSE */ \
			arm_hardware_piob_updown(HARDWARE_SDIO_CD_BIT, 0); \
	} while (0)

	#define HARDWARE_SDIOPOWER_BIT (1u << 12)	/* PB12 */
	#define HARDWARE_SDIOPOWER_C(v)	do { GPIOB->BSRR = BSRR_C(v); (void) GPIOC->BSRR; } while (0)
	#define HARDWARE_SDIOPOWER_S(v)	do { GPIOB->BSRR = BSRR_S(v); (void) GPIOC->BSRR; } while (0)
	/* если питание SD CARD управляется прямо с процессора */
	#define HARDWARE_SDIOPOWER_INITIALIZE()	do { \
		arm_hardware_piob_outputs2m(HARDWARE_SDIOPOWER_BIT, HARDWARE_SDIOPOWER_BIT); /* питание выключено */ \
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
	////#define TXPATH_TARGET_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); (void) GPIOD->BSRR; } while (0)
	////#define TXPATH_TARGET_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); (void) GPIOD->BSRR; } while (0)
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


	// +++
	// TXDISABLE input - PD10
	#define TXDISABLE_TARGET_PIN				(GPIOD->IDR)
	#define TXDISABLE_BIT_TXDISABLE				0//(1U << 10)		// PD10 - TX INHIBIT
	// получить бит запрета передачи (от усилителя мощности)
	#define HARDWARE_GET_TXDISABLE() (0) //((TXDISABLE_TARGET_PIN & TXDISABLE_BIT_TXDISABLE) != 0)
	#define TXDISABLE_INITIALIZE() \
		do { \
			arm_hardware_piod_inputs(TXDISABLE_BIT_TXDISABLE); \
			arm_hardware_piod_updown(0, TXDISABLE_BIT_TXDISABLE); \
		} while (0)
	// ---

	// +++
	// PTT input - PF2
	// PTT2 input - PF3
	#define PTT_TARGET_PIN				(GPIOF->IDR)
	#define PTT_BIT_PTT					(1uL << 2)		// PF2 - PTT
	#define PTT2_TARGET_PIN				(GPIOF->IDR)
	#define PTT2_BIT_PTT				(1uL << 3)		// PF3 - PTT2
	// получить бит запроса оператором перехода на пердачу
	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0 || (PTT2_TARGET_PIN & PTT2_BIT_PTT) == 0)
	#define PTT_INITIALIZE() \
		do { \
			arm_hardware_piof_inputs(PTT_BIT_PTT); \
			arm_hardware_piof_updown(PTT_BIT_PTT, 0); \
			arm_hardware_piof_inputs(PTT2_BIT_PTT); \
			arm_hardware_piof_updown(PTT2_BIT_PTT, 0); \
		} while (0)
	// ---
	// TUNE input - PF4
	#define TUNE_TARGET_PIN				(GPIOF->IDR)
	#define TUNE_BIT_TUNE					(1U << 4)		// PF4
	#define HARDWARE_GET_TUNE() ((TUNE_TARGET_PIN & TUNE_BIT_TUNE) == 0)
	#define TUNE_INITIALIZE() \
		do { \
			arm_hardware_piof_inputs(TUNE_BIT_TUNE); \
			arm_hardware_piof_updown(TUNE_BIT_TUNE, 0); \
		} while (0)

#else /* WITHTX */

	#define TXDISABLE_INITIALIZE() \
		do { \
		} while (0)
	#define PTT_INITIALIZE() \
		do { \
		} while (0)
	#define TUNE_INITIALIZE() \
		do { \
		} while (0)
#endif /* WITHTX */

#if WITHELKEY
	// Electronic key inputs
	#define ELKEY_BIT_LEFT				(1uL << 0)		// PF0
	#define ELKEY_BIT_RIGHT				(1uL << 1)		// PF1

	#define ELKEY_TARGET_PIN			(GPIOF->IDR)

	#define ELKEY_INITIALIZE() \
		do { \
			arm_hardware_piof_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
			arm_hardware_piof_updown(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, 0); \
		} while (0)

#endif /* WITHELKEY */

// IOUPDATE = PA15
//#define SPI_IOUPDATE_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); (void) GPIOA->BSRR; } while (0)
//#define SPI_IOUPDATE_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); (void) GPIOA->BSRR; } while (0)
//#define SPI_IOUPDATE_BIT		(1uL << 15)	// * PA15

#if WITHSPIHW || WITHSPISW
	// Набор определений для работы без внешнего дешифратора
	#define SPI_ALLCS_PORT_S(v)	do { GPIOH->BSRR = BSRR_S(v); (void) GPIOH->BSRR; } while (0)
	#define SPI_ALLCS_PORT_C(v)	do { GPIOH->BSRR = BSRR_C(v); (void) GPIOH->BSRR; } while (0)

	#define targetext1		(1uL << 7)		// PH7 ext1 on front panel
	#define targetxad2		(1uL << 2)		// PH2 ext2 двунаправленный SPI для подключения внешних устройств - например тюнера
	#define targetnvram		(1uL << 8)		// PH8 nvram FM25L16B
	#define targetctl1		(1uL << 5)		// PH5 board control registers chain
	#define targetcodec1	(1uL << 3)		// PH3 on-board codec1 NAU8822L
	#define targetadc2		(1uL << 6) 		// PH6 ADC MCP3208-BI/SL chip select (potentiometers)
	#define targetfpga1		(1uL << 14)		// PH14 FPGA control registers CS1

	// Здесь должны быть перечислены все биты формирования CS в устройстве.
	#define SPI_ALLCS_BITS ( \
		targetext1		| 	/* PH7 ext1 on front panel */ \
		targetxad2		|	/* PH2 PA100W on-board ADC (not connected on this board) */ \
		targetnvram		| 	/* PH8 nvmem FM25L16B */ \
		targetctl1		| 	/* PH5 board control registers chain */ \
		targetcodec1	| 	/* PH3 on-board codec1 NAU8822L */ \
		targetadc2		| 	/* PH6 ADC MCP3208-BI/SL chip select (potentiometers) */ \
		targetfpga1		| 	/* PH14 FPGA control registers CS1 */ \
		0)

	#define targetlcd	targetext1 	/* LCD over SPI line devices control */ 
	#define targetuc1608 targetext1	/* LCD with positive chip select signal	*/
	#define targettsc1 		targetext1	/* XPT2046 SPI chip select signal */

	#define SPI_ALLCS_BITSNEG 0		// Выходы, активные при "1"

	//#define SPI_NAEN_PORT_S(v)	do { GPIOx->BSRR = BSRR_S(v); (void) GPIOx->BSRR; } while (0)
	//#define SPI_NAEN_PORT_C(v)	do { GPIOx->BSRR = BSRR_C(v); (void) GPIOx->BSRR; } while (0)

	//#define SPI_NAEN_BIT (1u << 7)		// * PE7 used

	/* инициализация лиий выбора периферийных микросхем */
	#define SPI_ALLCS_INITIALIZE() \
		do { \
			arm_hardware_pioh_outputs2m(SPI_ALLCS_BITS, SPI_ALLCS_BITS ^ SPI_ALLCS_BITSNEG); \
		} while (0)

	// MOSI & SCK port
	// SPI1
	//	SPI1_SCK	PZ0	AF_5
	//	SPI1_MISO	PZ1 AF_5
	//	SPI1_MOSI	PZ2 AF_5
	#define SPI_TARGET_SCLK_PORT_C(v)	do { GPIOZ->BSRR = BSRR_C(v); (void) GPIOZ->BSRR; } while (0)
	#define SPI_TARGET_SCLK_PORT_S(v)	do { GPIOZ->BSRR = BSRR_S(v); (void) GPIOZ->BSRR; } while (0)
	#define	SPI_SCLK_BIT			(1uL << 0)	// PZ0 бит, через который идет синхронизация SPI

	#define SPI_TARGET_MOSI_PORT_C(v)	do { GPIOZ->BSRR = BSRR_C(v); (void) GPIOZ->BSRR; } while (0)
	#define SPI_TARGET_MOSI_PORT_S(v)	do { GPIOZ->BSRR = BSRR_S(v); (void) GPIOZ->BSRR; } while (0)
	#define	SPI_MOSI_BIT			(1uL << 2)	// PZ2 бит, через который идет вывод

	#define SPI_TARGET_MISO_PIN		(GPIOB->IDR)
	#define	SPI_MISO_BIT			(1uL << 1)	// PZ1 бит, через который идет ввод с SPI.

	#define SPIIO_INITIALIZE() do { \
			arm_hardware_pioz_outputs50m(SPI_SCLK_BIT, SPI_SCLK_BIT); /* PZ0 */ \
			arm_hardware_pioz_outputs50m(SPI_MOSI_BIT, SPI_MOSI_BIT); /* PZ2 */ \
			arm_hardware_pioz_inputs(SPI_MISO_BIT); /* PZ1 */ \
		} while (0)
	#define HARDWARE_SPI_CONNECT() do { \
			arm_hardware_pioz_altfn20(SPI_MOSI_BIT | SPI_MISO_BIT, AF_SPI2); /* AF_5 В этих процессорах и входы и выходы перекдючаются на ALT FN */ \
			arm_hardware_pioz_altfn20(SPI_SCLK_BIT, AF_SPI2); /* AF_5 В этих процессорах и входы и выходы перекдючаются на ALT FN */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT() do { \
			arm_hardware_pioz_outputs50m(SPI_SCLK_BIT, SPI_SCLK_BIT); \
			arm_hardware_pioz_outputs50m(SPI_MOSI_BIT, SPI_MOSI_BIT); \
			arm_hardware_pioz_inputs(SPI_MISO_BIT); \
		} while (0)
	#define HARDWARE_SPI_CONNECT_MOSI() do { \
			arm_hardware_pioz_altfn20(SPI_MOSI_BIT, AF_SPI2);	/* AF_5 PIO disable for MOSI bit (SD CARD read support) */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
			arm_hardware_pioz_outputs50m(SPI_MOSI_BIT, SPI_MOSI_BIT);	/* PIO enable for MOSI bit (SD CARD read support)  */ \
		} while (0)

#endif /* WITHSPIHW || WITHSPISW */

// WITHUART4HW
#define HARDWARE_UART4_INITIALIZE() do { \
		const uint_fast32_t TXMASK = (1uL << 11); /* PG11: TX DATA line (2 MHz) */ \
		const uint_fast32_t RXMASK = (1uL << 2); /* PB2: RX DATA line (2 MHz) - pull-up RX data */  \
		arm_hardware_piog_altfn50(TXMASK, 6); /* AF6 */ \
		arm_hardware_piob_altfn50(RXMASK, 8); /* AF8 */ \
		arm_hardware_piob_updown(RXMASK, 0); \
	} while (0)

#define TARGET_ENC2BTN_BIT (1U << 12)	// PE12 - second encoder button with pull-up

#if WITHKEYBOARD
	/* PE15: pull-up second encoder button */

	#define TARGET_POWERBTN_BIT (1U << 5)	// PF5 - ~CPU_POWER_SW signal

#if WITHENCODER2
	// P7_8
	#define TARGET_ENC2BTN_GET	(((GPIOE->IDR) & TARGET_ENC2BTN_BIT) == 0)
#endif /* WITHENCODER2 */

#if WITHPWBUTTON
	// P5_3 - ~CPU_POWER_SW signal
	#define TARGET_POWERBTN_GET	(((GPIOF->IDR) & TARGET_POWERBTN_BIT) == 0)
#endif /* WITHPWBUTTON */

	#define HARDWARE_KBD_INITIALIZE() do { \
			arm_hardware_pioe_inputs(TARGET_ENC2BTN_BIT); \
			arm_hardware_pioe_updown(TARGET_ENC2BTN_BIT, 0); /* PE15: pull-up second encoder button */ \
			arm_hardware_piof_inputs(TARGET_POWERBTN_BIT); \
			arm_hardware_piof_updown(TARGET_POWERBTN_BIT, 0);	/* PF5: pull-up second encoder button */ \
		} while (0)

#else /* WITHKEYBOARD */

	#define HARDWARE_KBD_INITIALIZE() do { \
		} while (0)

#endif /* WITHKEYBOARD */

#if 1 // WITHTWISW
	// I2C2_SDA	PZ5
	// I2C2_SCL	PZ4
	#define TARGET_TWI_TWCK		(1u << 4)		// I2C2_SCL	PZ4
	#define TARGET_TWI_TWCK_PIN		(GPIOZ->IDR)
	#define TARGET_TWI_TWCK_PORT_C(v) do { GPIOZ->BSRR = BSRR_C(v); (void) GPIOZ->BSRR; } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do { GPIOZ->BSRR = BSRR_S(v); (void) GPIOZ->BSRR; } while (0)

	#define TARGET_TWI_TWD		(1u << 5)		// I2C2_SDA	PZ5
	#define TARGET_TWI_TWD_PIN		(GPIOZ->IDR)
	#define TARGET_TWI_TWD_PORT_C(v) do { GPIOZ->BSRR = BSRR_C(v); (void) GPIOZ->BSRR; } while (0)
	#define TARGET_TWI_TWD_PORT_S(v) do { GPIOZ->BSRR = BSRR_S(v); (void) GPIOZ->BSRR; } while (0)

	// Инициализация битов портов ввода-вывода для программной реализации I2C
	#define	TWISOFT_INITIALIZE() do { \
			arm_hardware_pioz_opendrain(TARGET_TWI_TWCK, TARGET_TWI_TWCK); /* PZ4 I2C2_SCL */ \
			arm_hardware_pioz_opendrain(TARGET_TWI_TWD, TARGET_TWI_TWD);  	/* PZ5 I2C2_SDA */ \
		} while (0) 
	#define	TWISOFT_DEINITIALIZE() do { \
			arm_hardware_pioz_inputs(TARGET_TWI_TWCK); 	/* PZ4 I2C2_SCL */ \
			arm_hardware_pioz_inputs(TARGET_TWI_TWD);	/* PZ5 I2C2_SDA */ \
		} while (0)
	// Инициализация битов портов ввода-вывода для аппаратной реализации I2C
	// присоединение выводов к периферийному устройству
	#define	TWIHARD_INITIALIZE() do { \
			arm_hardware_pioz_periphopendrain_altfn2(TARGET_TWI_TWCK, 3);	/* PZ4 I2C2_SCL AF=3 */ \
			arm_hardware_pioz_periphopendrain_altfn2(TARGET_TWI_TWD, 3);	/* PZ5 I2C2_SDA AF=3 */ \
		} while (0) 


#endif // WITHTWISW

#if WITHFPGAWAIT_AS || WITHFPGALOAD_PS

	// FPGA INTERFACE
	//	FPGA_NSTATUS	PA14
	//	FPGA_NCONFIG	PA10
	//	FPGA_CONFDONE	PA15
	//	FPGA_INITDONE	PF11
	//	FPGA ADC OVF	PI8	не мощные

	/* outputs */
	#define FPGA_NCONFIG_PORT_S(v)	do { GPIOC->BSRR = BSRR_S(v); (void) GPIOC->BSRR; } while (0)
	#define FPGA_NCONFIG_PORT_C(v)	do { GPIOC->BSRR = BSRR_C(v); (void) GPIOC->BSRR; } while (0)
	#define FPGA_NCONFIG_BIT		(1UL << 10)	/* PA10 bit conneced to nCONFIG pin ALTERA FPGA */

	/* inputs */
	#define FPGA_CONF_DONE_INPUT	(GPIOA->IDR)
	#define FPGA_CONF_DONE_BIT		(1UL << 15)	/* PA15 bit conneced to CONF_DONE pin ALTERA FPGA */

	#define FPGA_NSTATUS_INPUT		(GPIOA->IDR)
	#define FPGA_NSTATUS_BIT		(1UL << 14)	/* PA14 bit conneced to NSTATUS pin ALTERA FPGA */

	#define FPGA_INIT_DONE_INPUT	(GPIOF->IDR)
	#define FPGA_INIT_DONE_BIT		(1UL << 11)	/* PF11 bit conneced to INIT_DONE pin ALTERA FPGA */

	/* Инициадизация выводов GPIO процессора для получения состояния и управлением загрузкой FPGA */
	#define HARDWARE_FPGA_LOADER_INITIALIZE() do { \
			arm_hardware_pioc_outputs(FPGA_NCONFIG_BIT, FPGA_NCONFIG_BIT); \
			arm_hardware_pioa_inputs(FPGA_NSTATUS_BIT); \
			arm_hardware_pioa_inputs(FPGA_CONF_DONE_BIT); \
			arm_hardware_piof_inputs(FPGA_INIT_DONE_BIT); \
		} while (0)

	/* Проверяем, проинициализировалась ли FPGA (вошла в user mode). */
	/*
		After the option bit to enable INIT_DONE is programmed into the device (during the first
		frame of configuration data), the INIT_DONE pin goes low.
		When initialization is complete, the INIT_DONE pin is released and pulled high. 
		This low-to-high transition signals that the device has entered user mode.
	*/
	#define HARDWARE_FPGA_IS_USER_MODE() (local_delay_ms(100), (FPGA_INIT_DONE_INPUT & FPGA_INIT_DONE_BIT) != 0)

#endif /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS */

#if WITHDSPEXTFIR
	// Биты доступа к массиву коэффициентов FIR фильтра в FPGA
	//	FPGA_FIR1_WE	PG2	не мощные
	//	FPGA_FIR2_WE	PG3	не мощные
	//	FPGA_FIR_CLK	PG8

	// FPGA PIN_23
	#define TARGET_FPGA_FIR_CS_PORT_C(v)	do { GPIOG->BSRR = BSRR_C(v); (void) GPIOG->BSRR; } while (0)
	#define TARGET_FPGA_FIR_CS_PORT_S(v)	do { GPIOG->BSRR = BSRR_S(v); (void) GPIOG->BSRR; } while (0)
	#define TARGET_FPGA_FIR_CS_BIT (1uL << 8)	/* PG8 - fir CS ~FPGA_FIR_CLK */

	// FPGA PIN_8
	#define TARGET_FPGA_FIR1_WE_PORT_C(v)	do { GPIOD->BSRR = BSRR_C(v); (void) GPIOG->BSRR; } while (0)
	#define TARGET_FPGA_FIR1_WE_PORT_S(v)	do { GPIOD->BSRR = BSRR_S(v); (void) GPIOG->BSRR; } while (0)
	#define TARGET_FPGA_FIR1_WE_BIT (1uL << 2)	/* PG2 - fir1 WE */

	// FPGA PIN_7
	#define TARGET_FPGA_FIR2_WE_PORT_C(v)	do { GPIOG->BSRR = BSRR_C(v); (void) GPIOG->BSRR; } while (0)
	#define TARGET_FPGA_FIR2_WE_PORT_S(v)	do { GPIOG->BSRR = BSRR_S(v); (void) GPIOG->BSRR; } while (0)
	#define TARGET_FPGA_FIR2_WE_BIT (1uL << 3)	/* PG3 - fir2 WE */

	#define TARGET_FPGA_FIR_INITIALIZE() do { \
			arm_hardware_piog_outputs2m(TARGET_FPGA_FIR1_WE_BIT, TARGET_FPGA_FIR1_WE_BIT); \
			arm_hardware_piog_outputs2m(TARGET_FPGA_FIR2_WE_BIT, TARGET_FPGA_FIR2_WE_BIT); \
			arm_hardware_piog_outputs2m(TARGET_FPGA_FIR_CS_BIT, TARGET_FPGA_FIR_CS_BIT); \
		} while (0)
#endif /* WITHDSPEXTFIR */

#if 1
	/* получение состояния переполнения АЦП */
	// PI8
	#define TARGET_FPGA_OVF_INPUT		(GPIOI->IDR)
	#define TARGET_FPGA_OVF_BIT			(1u << 8)	// PI8
	#define TARGET_FPGA_OVF_GET			((TARGET_FPGA_OVF_INPUT & TARGET_FPGA_OVF_BIT) == 0)	// 1 - overflow active
	#define TARGET_FPGA_OVF_INITIALIZE() do { \
				arm_hardware_pioi_inputs(TARGET_FPGA_OVF_BIT); \
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

#else /* WITHCPUDACHW */
	#define HARDWARE_DAC_INITIALIZE() do { \
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

	#define TARGET_USBFS_VBUSON_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); (void) GPIOA->BSRR; } while (0)
	#define TARGET_USBFS_VBUSON_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); (void) GPIOA->BSRR; } while (0)
	#define TARGET_USBFS_VBUSON_BIT (1uL << 11)	// PA11 - нулём включение питания для device

	/**USB_OTG_FS GPIO Configuration    
	PA9     ------> USB_OTG_FS_VBUS
	PA10     ------> USB_OTG_FS_ID
	PA11     ------> USB_OTG_FS_DM
	PA12     ------> USB_OTG_FS_DP 
	*/

	#define USBPHYC_MISC_SWITHOST_Pos		0
	#define USBPHYC_MISC_SWITHOST_Msk (0x01uL << USBPHYC_MISC_SWITHOST_Pos)
	#define USBPHYC_MISC_PPCKDIS_Pos		1
	#define USBPHYC_MISC_PPCKDIS_Msk (0x03uL << USBPHYC_MISC_PPCKDIS_Pos)

	#define	USBD_EHCI_INITIALIZE() do { \
		RCC->MP_APB4ENSETR = RCC_MP_APB4ENSETR_USBPHYEN; \
		(void) RCC->MP_APB4ENSETR; \
		RCC->MP_APB4LPENSETR = RCC_MP_APB4LPENSETR_USBPHYLPEN; \
		(void) RCC->MP_APB4LPENSETR; \
		/* STM32_USBPHYC_MISC bit fields */ \
		/*	SWITHOST 0: Select OTG controller for 2nd PHY port */ \
		/*	SWITHOST 1: Select Host controller for 2nd PHY port */ \
		/*	EHCI controller hard wired to 1st PHY port */ \
		USBPHYC->MISC = (USBPHYC->MISC & ~ (USBPHYC_MISC_SWITHOST_Msk | USBPHYC_MISC_PPCKDIS_Msk)) | \
			(USBPHYC_MISC_SWITHOST_VAL << USBPHYC_MISC_SWITHOST_Pos) |	/* 0: Select OTG controller for 2nd PHY port, 1: Select Host controller for 2nd PHY port */ \
			(USBPHYC_MISC_PPCKDIS_VAL << USBPHYC_MISC_PPCKDIS_Pos) | \
			0; \
		(void) USBPHYC->MISC; \
		arm_hardware_pioa_outputs(TARGET_USBFS_VBUSON_BIT, TARGET_USBFS_VBUSON_BIT); /* PA11 */ \
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
		/*arm_hardware_pioa_altfn50((1uL << 11) | (1uL << 12), AF_OTGFS);	*/		/* PA10, PA11, PA12 - USB_OTG_FS	*/ \
		/* arm_hardware_pioa_inputs(1uL << 9);	*/	/* PA9 - USB_OTG_FS_VBUS */ \
		} while (0)

	#define	USBD_HS_ULPI_INITIALIZE() do { \
		} while (0)
#endif /* WITHUSBHW */

#if WITHDCDCFREQCTL
	// ST ST1S10 Synchronizable switching frequency from 400 kHz up to 1.2 MHz
	#define WITHHWDCDCFREQMIN 400000L
	#define WITHHWDCDCFREQMAX 1200000L

	// PB9 - DC-DC synchro output
	// TIM17_CH1 AF1
	// TIM4_CH4	AF2	AF_TIM4
	#define	HARDWARE_DCDC_INITIALIZE() do { \
		arm_hardware_piob_altfn2((1U << 9), AF_TIM17); /* PB9 - TIM17_CH1 */ \
		hardware_dcdcfreq_tim17_ch1_initialize(); \
	} while (0)
	#define HARDWARE_DCDC_SETDIV(f) do { \
		hardware_dcdcfreq_tim17_ch1_setdiv(f); \
	} while (0)
#else /* WITHDCDCFREQCTL */
	#define	HARDWARE_DCDC_INITIALIZE() do { \
	} while (0)
	#define HARDWARE_DCDC_SETDIV(f) do { \
		(void) (f); \
	} while (0)
#endif /* WITHDCDCFREQCTL */

	#if LCDMODE_LQ043T3DX02K
		#define WITHLCDBACKLIGHTOFF	1	// Имеется управление включением/выключением подсветки дисплея
		#define WITHLCDBACKLIGHT	1	// Имеется управление яркостью дисплея
		#define WITHLCDBACKLIGHTMIN	0	// Нижний предел регулировки (показываемый на дисплее)
		#define WITHLCDBACKLIGHTMAX	3	// Верхний предел регулировки (показываемый на дисплее)
		//#define WITHKBDBACKLIGHT	1	// Имеется управление подсветкой клавиатуры
	#elif LCDMODE_AT070TN90 || LCDMODE_AT070TNA2
		#define WITHLCDBACKLIGHTOFF	1	// Имеется управление включением/выключением подсветки дисплея
		#define WITHLCDBACKLIGHT	1	// Имеется управление яркостью дисплея
		#define WITHLCDBACKLIGHTMIN	1	// Нижний предел регулировки (показываемый на дисплее)
		#define WITHLCDBACKLIGHTMAX	3	// Верхний предел регулировки (показываемый на дисплее)
		//#define WITHKBDBACKLIGHT	1	// Имеется управление подсветкой клавиатуры
	#else
		/* Заглушка для работы без дисплея */
		#define WITHLCDBACKLIGHTMIN	0
		#define WITHLCDBACKLIGHTMAX	2	// Верхний предел регулировки (показываемый на дисплее)
	#endif

	/* BL0: PA14. BL1: PA15 */
	#define	HARDWARE_BL_INITIALIZE() do { \
		const portholder_t ENmask = (1uL << 9); /* PD9 */ \
		const portholder_t BLpins = (1uL << 7) | (1uL << 6); /* PZ7:PZ6 */ \
		arm_hardware_pioz_opendrain(BLpins, 0); \
		arm_hardware_piod_outputs2m(ENmask, ENmask);  \
		} while (0)

	/* установка яркости и включение/выключение преобразователя подсветки */
	/* BL0: PZ6. BL1: PZ7 */
	// PD9 BL Enable
	#define HARDWARE_BL_SET(en, level) do { \
		const portholder_t Vlevel = (level) & 0x03; \
		const portholder_t ENmask = (1uL << 9); /* PD9 */ \
		const portholder_t BLpins = (1uL << 7) | (1uL << 6); /* PZ7:PZ6 */ \
		const portholder_t BLstate = (~ Vlevel) << 6; \
		GPIOZ->BSRR = \
			BSRR_S((BLstate) & (BLpins)) | /* set bits */ \
			BSRR_C(~ (BLstate) & (BLpins)) | /* reset bits */ \
			0; \
		(void) GPIOZ->BSRR; \
		GPIOD->BSRR = \
			((en) ? BSRR_S(ENmask) : BSRR_C(ENmask)) | /* backlight control on/off */ \
			0; \
			(void) GPIOZ->BSRR; \
	} while (0)

#if WITHLTDCHW
	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_INITIALIZE(demode) do { \
		enum \
		{ \
			GPIO_AF_LTDC14 = 14,  /* LCD-TFT Alternate Function mapping */ \
			GPIO_AF_LTDC9 = 9,  /* LCD-TFT Alternate Function mapping */ \
			GPIO_AF_LTDC3 = 3  /* LCD-TFT Alternate Function mapping */ \
		}; \
		const uint32_t MODEmask = (1U << 10); /* PB10 - FPLCD_CD */ \
		const uint32_t RESETmask = (1U << 13); /* PB13 - FPLCD_RESET */ \
		const uint32_t DEmask = (1U << 13); /* PE13 - DE */ \
		const uint32_t HSmask = (1U << 10); /* PI10 - HSYNC */ \
		const uint32_t VSmask = (1U << 9); 	/* PI9 - VSYNC */ \
		/* LCD RESET */ \
		arm_hardware_piob_outputs2m(RESETmask, RESETmask); /* PD4 - FPLCD_RESET */ \
		/* Bit clock */ \
		arm_hardware_piog_altfn50((1U << 7), GPIO_AF_LTDC14);		/* CLK PG7 AF_14*/ \
		/* Control */ \
		arm_hardware_piob_outputs(MODEmask, (demode != 0) * MODEmask);	/* PD3 MODEmask=state */ \
		/* Synchronisation signals in SYNC mode */ \
		arm_hardware_pioe_outputs((demode == 0) * DEmask, 0);	/* DE=0 (DISP, pin 31) */ \
		arm_hardware_pioi_altfn50((demode == 0) * VSmask, GPIO_AF_LTDC14);	/* VSYNC */ \
		arm_hardware_pioi_altfn50((demode == 0) * HSmask, GPIO_AF_LTDC14);	/* HSYNC */ \
		/* Synchronisation signals in DE mode*/ \
		arm_hardware_pioe_altfn50((demode != 0) * DEmask, GPIO_AF_LTDC14);	/* DE */ \
		arm_hardware_pioi_outputs((demode != 0) * VSmask, VSmask);	/* VSYNC */ \
		arm_hardware_pioi_outputs((demode != 0) * HSmask, HSmask);	/* HSYNC */ \
		/* RED */ \
		arm_hardware_pioh_altfn50((1U << 9), GPIO_AF_LTDC14);		/* PH9 R3 */ \
		arm_hardware_pioh_altfn50((1U << 10), GPIO_AF_LTDC14);		/* PH10 R4 */ \
		arm_hardware_pioh_altfn50((1U << 11), GPIO_AF_LTDC14);		/* PH11 R5 */ \
		arm_hardware_pioh_altfn50((1U << 12), GPIO_AF_LTDC14);		/* PH12 R6 */ \
		arm_hardware_pioe_altfn50((1U << 15), GPIO_AF_LTDC14);		/* PE15 R7 */ \
		/* GREEN */ \
		arm_hardware_pioh_altfn50((1U << 13), GPIO_AF_LTDC14);		/* PH13 G2 */ \
		arm_hardware_piog_altfn50((1U << 10), GPIO_AF_LTDC9);		/* PG10 G3 */ \
		arm_hardware_pioh_altfn50((1U << 15), GPIO_AF_LTDC14);		/* PH15 G4 */ \
		arm_hardware_pioh_altfn50((1U << 11), GPIO_AF_LTDC9);		/* PH4 G5 */ \
		arm_hardware_pioc_altfn50((1U << 7), GPIO_AF_LTDC14);		/* PC7 G6 */ \
		arm_hardware_piob_altfn50((1U << 5), GPIO_AF_LTDC14);		/* PB5 G7 */ \
		/* BLUE */ \
		arm_hardware_piod_altfn50((1U << 10), GPIO_AF_LTDC14);		/* PD10 B3 */ \
		arm_hardware_piog_altfn50((1U << 12), GPIO_AF_LTDC9);		/* PG12 B4 */ \
		arm_hardware_pioi_altfn50((1U << 5), GPIO_AF_LTDC14);		/* PI5 B5 */ \
		arm_hardware_piob_altfn50((1U << 8), GPIO_AF_LTDC14);		/* PB8 B6 */ \
		arm_hardware_piod_altfn50((1U << 8), GPIO_AF_LTDC14);		/* PD8 B7 */ \
	} while (0)

	/* управление состоянием сигнала DISP панели */
	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_SET_DISP(state) do { \
		const uint32_t DEmask = (1U << 13); /* PE13 - DE */ \
		const uint32_t HSmask = (1U << 10); /* PI10 - HSYNC */ \
		const uint32_t VSmask = (1U << 9); 	/* PI9 - VSYNC */ \
		/* while ((GPIOA->IDR & VSmask) != 0) ; */ /* схема синхронизации стоит на плате дисплея. дождаться 0 */ \
		/* while ((GPIOA->IDR & VSmask) == 0) ; */ /* дождаться 1 */ \
		arm_hardware_pioe_outputs(DEmask, ((state) != 0) * DEmask); /* DE=DISP, pin 31 - можно менять только при VSYNC=1 */ \
	} while (0)
#endif /* WITHLTDCHW */

	#if WITHSDRAMHW
		// Bootloader parameters
		#define BOOTLOADER_RAMAREA DRAM_MEM_BASE	/* адрес ОЗУ, куда перемещать application */
		#define BOOTLOADER_RAMSIZE (1024uL * 1024uL * 256)	// 256M
		#define BOOTLOADER_RAMPAGESIZE	(1024uL * 1024)	// при загрузке на исполнение используется размер страницы в 1 мегабайт
		#define USBD_DFU_RAM_XFER_SIZE 4096
	#endif /* WITHSDRAMHW */

	#define BOOTLOADER_FLASHSIZE (1024uL * 1024uL * 16)	// 16M FLASH CHIP
	#define BOOTLOADER_SELFBASE QSPI_MEM_BASE	/* адрес где лежит во FLASH образ application */
	#define BOOTLOADER_SELFSIZE (1024uL * 512)	// 512k

	#define BOOTLOADER_APPBASE (BOOTLOADER_SELFBASE + BOOTLOADER_SELFSIZE)	/* адрес где лежит во FLASH образ application */
	#define BOOTLOADER_APPSIZE (chipsizeDATAFLASH() - BOOTLOADER_SELFSIZE)	// 2048 - 128

	//#define BOOTLOADER_PAGESIZE (1024uL * 64)	// W25Q32FV with 64 KB pages

	#define USBD_DFU_FLASH_XFER_SIZE 256	// match to (Q)SPI FLASH MEMORY page size
	#define USBD_DFU_FLASHNAME "W25Q128JV"

	/* Выводы соединения с QSPI BOOT NOR FLASH */
	#define SPDIF_MISO_BIT (1u << 9)	// PF9	QUADSPI_BK1_IO1
	#define SPDIF_MOSI_BIT (1u << 8)	// PF8	QUADSPI_BK1_IO0
	#define SPDIF_SCLK_BIT (1u << 10)	// PF10	QUADSPI_CLK
	#define SPDIF_NCS_BIT (1u << 6)		// PB6	QUADSPI_BK1_NCS

	#define SPDIF_D2_BIT (1u << 7)		// PF7	QUADSPI_BK1_IO2
	#define SPDIF_D3_BIT (1u << 6)		// PF6	QUADSPI_BK1_IO3
	/* Отсоединить процессор от BOOT ROM - для возможности работы внешнего программатора. */
	#define SPIDF_HANGOFF() do { \
			arm_hardware_piob_inputs(SPDIF_NCS_BIT); \
			arm_hardware_piof_inputs(SPDIF_SCLK_BIT); \
			arm_hardware_piof_inputs(SPDIF_MOSI_BIT); \
			arm_hardware_piof_inputs(SPDIF_MISO_BIT); \
		} while (0)

	#if WIHSPIDFSW || WIHSPIDFHW

		#if WIHSPIDFHW
			#define SPIDF_HARDINITIALIZE() do { \
					arm_hardware_piof_altfn50(SPDIF_D2_BIT, AF_QUADSPI_AF9);  	/* PF7 D2 QUADSPI_BK1_IO2 */ \
					arm_hardware_piof_altfn50(SPDIF_D3_BIT, AF_QUADSPI_AF9);  	/* PF6 D3 QUADSPI_BK1_IO3 */ \
					/*arm_hardware_piof_outputs(SPDIF_D2_BIT, SPDIF_D2_BIT); */ /* PF7 D2 tie-up */ \
					/*arm_hardware_piof_outputs(SPDIF_D3_BIT, SPDIF_D3_BIT); */ /* PF6 D3 tie-up */ \
					arm_hardware_piof_altfn50(SPDIF_SCLK_BIT, AF_QUADSPI_AF9); /* PF10 SCLK */ \
					arm_hardware_piof_altfn50(SPDIF_MOSI_BIT, AF_QUADSPI_AF10); /* PF8 MOSI */ \
					arm_hardware_piof_altfn50(SPDIF_MISO_BIT, AF_QUADSPI_AF10); /* PF9 MISO */ \
					arm_hardware_piob_altfn50(SPDIF_NCS_BIT, AF_QUADSPI_AF10); /* PB6 CS */ \
				} while (0)

		#else /* WIHSPIDFHW */

			#define SPIDF_MISO() ((GPIOF->IDR & SPDIF_MISO_BIT) != 0)
			#define SPIDF_MOSI(v) do { if (v) GPIOF->BSRR = BSRR_S(SPDIF_MOSI_BIT); else GPIOF->BSRR = BSRR_C(SPDIF_MOSI_BIT); } while (0)
			#define SPIDF_SCLK(v) do { if (v) GPIOF->BSRR = BSRR_S(SPDIF_SCLK_BIT); else GPIOF->BSRR = BSRR_C(SPDIF_SCLK_BIT); } while (0)
			#define SPIDF_SOFTINITIALIZE() do { \
					/*arm_hardware_piof_outputs(SPDIF_D2_BIT, SPDIF_D2_BIT); */ /* D2 tie-up */ \
					/*arm_hardware_piof_outputs(SPDIF_D3_BIT, SPDIF_D3_BIT); */ /* D3 tie-up */ \
					arm_hardware_piob_outputs(SPDIF_NCS_BIT, SPDIF_NCS_BIT); \
					arm_hardware_piof_outputs(SPDIF_SCLK_BIT, SPDIF_SCLK_BIT); \
					arm_hardware_piof_outputs(SPDIF_MOSI_BIT, SPDIF_MOSI_BIT); \
					arm_hardware_piof_inputs(SPDIF_MISO_BIT); \
				} while (0)
			#define SPIDF_SELECT() do { \
					arm_hardware_piob_outputs(SPDIF_NCS_BIT, SPDIF_NCS_BIT); \
					arm_hardware_piof_outputs(SPDIF_SCLK_BIT, SPDIF_SCLK_BIT); \
					arm_hardware_piof_outputs(SPDIF_MOSI_BIT, SPDIF_MOSI_BIT); \
					arm_hardware_piof_inputs(SPDIF_MISO_BIT); \
					GPIOB->BSRR = BSRR_C(SPDIF_NCS_BIT); \
					__DSB(); \
				} while (0)
			#define SPIDF_UNSELECT() do { \
					GPIOB->BSRR = BSRR_S(SPDIF_NCS_BIT); \
					arm_hardware_piob_inputs(SPDIF_NCS_BIT); \
					arm_hardware_piof_inputs(SPDIF_SCLK_BIT); \
					arm_hardware_piof_inputs(SPDIF_MOSI_BIT); \
					arm_hardware_piof_inputs(SPDIF_MISO_BIT); \
					__DSB(); \
				} while (0)

		#endif /* WIHSPIDFHW */

	#endif /* WIHSPIDFSW || WIHSPIDFHW */

	#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811)

		//	tsc interrupt XS26, pin 08
		//	tsc/LCD reset, XS26, pin 22
		//	tsc SCL: XS26, pin 01
		//	tsc SDA: XS26, pin 02

		void stmpe811_interrupt_handler(void);

		#define BOARD_STMPE811_INT_PIN (1uL << 12)		/* PE14 : tsc interrupt XS26, pin 08 */
		//#define BOARD_STMPE811_RESET_PIN (1uL << 13)	/* PB13 : tsc/LCD reset, XS26, pin 22 */

		#define BOARD_STMPE811_INT_CONNECT() do { \
			arm_hardware_pioe_inputs(BOARD_STMPE811_INT_PIN); \
			arm_hardware_pioe_updown(BOARD_STMPE811_INT_PIN, 0); \
			arm_hardware_pioe_onchangeinterrupt(BOARD_STMPE811_INT_PIN, 1 * BOARD_STMPE811_INT_PIN, 0 * BOARD_STMPE811_INT_PIN, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM); \
		} while (0)

	#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811) */

	#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_GT911)

		//	tsc interrupt XS26, pin 08
		//	tsc/LCD reset, XS26, pin 22
		//	tsc SCL: XS26, pin 01
		//	tsc SDA: XS26, pin 02

		void gt911_interrupt_handler(void);

		#define BOARD_GT911_INT_PIN (1uL << 14)		/* PE14 : tsc interrupt XS26, pin 08 */
		#define BOARD_GT911_RESET_PIN (1uL << 13)	/* PB13 : tsc/LCD reset, XS26, pin 22 */

		#define BOARD_GT911_RESET_SET(v) do { if (v) GPIOB->BSRR = BSRR_S(BOARD_GT911_RESET_PIN); else GPIOB->BSRR = BSRR_C(BOARD_GT911_RESET_PIN); (void) GPIOB->BSRR; } while (0)
		#define BOARD_GT911_INT_SET(v) do { if (v) GPIOE->BSRR = BSRR_S(BOARD_GT911_INT_PIN); else GPIOE->BSRR = BSRR_C(BOARD_GT911_INT_PIN); (void) GPIOE->BSRR; } while (0)

		#define BOARD_GT911_RESET_INITIO_1() do { \
			arm_hardware_pioe_outputs2m(BOARD_GT911_INT_PIN, 1* BOARD_GT911_INT_PIN); \
			arm_hardware_piob_outputs2m(BOARD_GT911_RESET_PIN, 1 * BOARD_GT911_RESET_PIN); \
			 local_delay_ms(200);  \
		} while (0)

		#define BOARD_GT911_RESET_INITIO_2() do { \
			arm_hardware_piob_inputs(BOARD_GT911_INT_PIN); \
			arm_hardware_piob_updown(BOARD_GT911_INT_PIN, 0); \
		} while (0)

		#define BOARD_GT911_INT_CONNECT() do { \
			arm_hardware_pioe_inputs(BOARD_GT911_INT_PIN); \
			arm_hardware_pioe_updown(BOARD_GT911_INT_PIN, 0); \
			arm_hardware_pioe_onchangeinterrupt(BOARD_GT911_INT_PIN, 1 * BOARD_GT911_INT_PIN, 0 * BOARD_GT911_INT_PIN, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM); \
		} while (0)
		//gt911_interrupt_handler

	#endif

	#define BOARD_BLINK_BIT (1uL << 13)	// PA13 - led on Storch board

	#define BOARD_BLINK_INITIALIZE() do { \
			arm_hardware_pioa_opendrain(BOARD_BLINK_BIT, 0 * BOARD_BLINK_BIT); \
		} while (0)
	#define BOARD_BLINK_SETSTATE(state) do { \
			if (state) \
				(GPIOA)->BSRR = BSRR_C(BOARD_BLINK_BIT); \
			else \
				(GPIOA)->BSRR = BSRR_S(BOARD_BLINK_BIT); \
		} while (0)

	/* запрос на вход в режим загрузчика */
	#define BOARD_USERBOOT_BIT	(1uL << 15)	/* PG15: ~USER_BOOT */
	#define BOARD_IS_USERBOOT() (((GPIOG->IDR) & BOARD_USERBOOT_BIT) == 0 || ((GPIOE->IDR) & TARGET_ENC2BTN_BIT) == 0)
	#define BOARD_USERBOOT_INITIALIZE() do { \
			arm_hardware_piog_inputs(BOARD_USERBOOT_BIT); /* set as input with pull-up */ \
			arm_hardware_pioe_inputs(TARGET_ENC2BTN_BIT); /* set as input with pull-up */ \
		} while (0)

	/* макроопределение, которое должно включить в себя все инициализации */
	#define	HARDWARE_INITIALIZE() do { \
			I2S2HW_POOLDOWN(); \
			BOARD_BLINK_INITIALIZE(); \
			HARDWARE_KBD_INITIALIZE(); \
			HARDWARE_DAC_INITIALIZE(); \
			HARDWARE_BL_INITIALIZE(); \
			HARDWARE_DCDC_INITIALIZE(); \
			TXDISABLE_INITIALIZE(); \
			TUNE_INITIALIZE(); \
			BOARD_USERBOOT_INITIALIZE(); \
			USBD_EHCI_INITIALIZE(); \
		} while (0)

#endif /* ARM_STM32MP1_LFBGA354_CPUSTYLE_STORCH_V9C_H_INCLUDED */
