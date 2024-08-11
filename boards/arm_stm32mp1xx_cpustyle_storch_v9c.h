/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Трансивер с DSP обработкой "Аист" на процессоре STM32MP1xx
// rmainunit_sv9.pcb STM32MP157AAC - модуль MYC-YA157-V2, 2xUSB, NAU8822L и FPGA EP4CE22E22I7N

#ifndef ARM_STM32MP1_TFBGA361_CPUSTYLE_STORCH_V9C_H_INCLUDED
#define ARM_STM32MP1_TFBGA361_CPUSTYLE_STORCH_V9C_H_INCLUDED 1


#define STM32MP_DDR_DUAL_AXI_PORT 1
#define STM32MP_DDR_32BIT_INTERFACE 1

#define WITHSPI16BIT	1	/* возможно использование 16-ти битных слов при обмене по SPI */
#define WITHSPI32BIT	1	/* возможно использование 32-ти битных слов при обмене по SPI */
#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
//#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
//#define WITHSPISW 	1	/* Использование программного управления SPI. Нельзя убирать эту строку - требуется явное отключение из-за конфликта с I2C */
//#define WITHDMA2DHW		1	/* Использование DMA2D для формирования изображений	- у STM32MP1 его нет */

//#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */
#if WITHINTEGRATEDDSP
	#define WITHI2S2HW	1	/* Использование I2S - аудиокодек на I2S2 */
	#define WITHSAI2HW	1	/* Использование SAI2 - FPGA или IF codec	*/
	//#define WITHSAI3HW	1	/* Использование SAI3 - FPGA скоростной канал записи спктра	*/
#endif /* WITHINTEGRATEDDSP */

//#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
//#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */
#define WITHETHHW 1	/* Hardware Ethernet controller */

// OHCI at USB1HSFSP2_BASE
//#define WITHUSBHW_OHCI ((struct ohci_registers *) USB1HSFSP2_BASE)

#if WITHDEBUG
	#define WITHUART1HW			1	/* PG11, PB2 Используется периферийный контроллер последовательного порта USART1 */
	#define WITHUART1HW_FIFO	1	/* использование FIFO */
	#define WITHDEBUG_UART1		1
#endif /* WITHDEBUG */

#if WITHLFM
	#define WITHUART1HW			1	/* PG11, PB2 Используется периферийный контроллер последовательного порта USART1 */
	#define WITHUART1HW_FIFO	1	/* использование FIFO */
#endif /* WITHDEBUG */

#if 0
	#define WITHUART1HW			1	/* PG11, PB2 Используется периферийный контроллер последовательного порта USART1 */
	#define WITHUART1HW_FIFO	1	/* использование FIFO */
	#define WITHCAT_UART1		1
endif

#define BOARD_TUH_RHPORT 1

#if WITHISBOOTLOADER

	//#define WIHSPIDFSW	1	/* программное обслуживание DATA FLASH */
	#define WIHSPIDFHW		1	/* аппаратное обслуживание DATA FLASH */
	//#define WIHSPIDFHW2BIT	1	/* аппаратное обслуживание DATA FLASH с поддержкой QSPI подключения по 2-м проводам */
	#define WIHSPIDFHW4BIT	1	/* аппаратное обслуживание DATA FLASH с поддержкой QSPI подключения по 4-м проводам */

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
	//#define WITHUSBDEV_HSDESC	1			/* Требуется формировать дескрипторы как для HIGH SPEED */
	//#define WITHUSBDEV_HIGHSPEEDULPI	1
	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USBH_HS_DP & USBH_HS_DM
	//#define WITHUSBDEV_DMAENABLE 1

	//#define WITHUSBHW_HOST		USB_OTG_HS
	#define WITHUSBHOST_HIGHSPEEDPHYC	1	// UTMI -> USB_DP2 & USB_DM2
	//#define WITHUSBHOST_DMAENABLE 1

//	#define WITHEHCIHW	1	/* USB_EHCI controller */
//	#define WITHUSBHW_EHCI		USB1_EHCI
//	#define WITHEHCIHW_EHCIPORT 0	// 0 - use 1st PHY port, 1 - 2nd PHY port (shared with USB_OTG_HS). See also USBPHYC_MISC_SWITHOST_VAL
//	#define WITHOHCIHW_OHCIPORT 0

	#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
	#define WITHMODEM_CDC	1

	//#define WITHUSBUAC		1	/* использовать виртуальную звуковую плату на USB соединении */
	//#define WITHUSBUACIN2		1	/* формируются три канала передачи звука */
	//#define WITHUABUACOUTAUDIO48MONO	1	/* для уменьшения размера буферов в endpoints */

	//#define WITHUSBCDCACM		1	/* ACM использовать виртуальный последовательный порт на USB соединении */
	//#define WITHUSBCDCACM_N	1	/* количество виртуальных последовательных портов */

	//#define WITHUSBHID	1	/* HID использовать Human Interface Device на USB соединении */
	#define WITHUSBDFU	1	/* DFU USB Device Firmware Upgrade support */
	#define WITHUSBWCID	1

	//#define WITHUSBDMTP	1	/* MTP USB Device */

	//#define WITHLWIP 1
	//#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
	//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */
	//#define WITHUSBRNDIS	1	/* RNDIS использовать Remote NDIS на USB соединении */

#else /* WITHISBOOTLOADER */

	#define WITHDCDCFREQCTL	1		// Имеется управление частотой преобразователей блока питания и/или подсветки дисплея

	//#define WITHFPGAPIPE_CODEC1 1	/* Интерфейс к FPGA, транзитом в аудио кодек через I2S0 */
	#define WITHFPGAPIPE_RTS96 WITHRTS96	/* в том же фрейме иут квадратуры RTS96 */
	#define WITHFPGAPIPE_RTS192 WITHRTS192	/* в том же фрейме иут квадратуры RTS192 */
	#define WITHFPGAPIPE_NCORX0 1	/* управление частотой приемника 1 */
	#define WITHFPGAPIPE_NCORX1 1	/* управление частотой приемника 2 */
	#define WITHFPGAPIPE_NCORTS 1	/* управление частотой приемника панорамы */

	#define WITHFPGAIF_SAI2_A_TX_B_RX_MASTER	1		/* Получение квадратур и RTS96 от FPGA через SAI2 */
#if CODEC1_IFC_MASTER
	#define WITHCODEC1_I2S2_DUPLEX_SLAVE	1		/* Обмен с аудиокодеком через I2S2 */
#else /* CODEC1_IFC_MASTER */
	#define WITHCODEC1_I2S2_DUPLEX_MASTER	1		/* Обмен с аудиокодеком через I2S2 */
#endif /* CODEC1_IFC_MASTER */

	//#define WIHSPIDFSW	1	/* программное обслуживание DATA FLASH */
	#define WIHSPIDFHW		1	/* аппаратное обслуживание DATA FLASH */
	//#define WIHSPIDFHW2BIT	1	/* аппаратное обслуживание DATA FLASH с поддержкой QSPI подключения по 2-м проводам */
	#define WIHSPIDFHW4BIT	1	/* аппаратное обслуживание DATA FLASH с поддержкой QSPI подключения по 4-м проводам */

	#define WITHMDMAHW		1	/* Использование MDMA для формирования изображений */
	//#define WITHCPUDACHW	1	/* использование встроенного в процессор DAC */
	#define WITHCPUADCHW 	1	/* использование встроенного в процессор ADC */

	#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
	#define WITHGPUHW	1	/* Graphic processor unit */
	#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */
	#define USBPHYC_MISC_SWITHOST_VAL 0		// 0 or 1 - value for USBPHYC_MISC_SWITHOST field. 0: Select OTG controller for 2nd PHY port, 1: Select Host controller for 2nd PHY port
	#define USBPHYC_MISC_PPCKDIS_VAL 0x00

	#define WITHUSBHW_DEVICE	USB_OTG_HS	/* на этом устройстве поддерживается функциональность DEVICE	*/
	#define WITHUSBDEV_VBUSSENSE	1		/* используется предопределенный вывод OTG_VBUS */
	//#define WITHUSBDEV_HSDESC	1			/* Требуется формировать дескрипторы как для HIGH SPEED */
	//#define WITHUSBDEV_HIGHSPEEDULPI	1	// ULPI
	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USB_DP2 & USB_DM2
	//#define WITHUSBDEV_DMAENABLE 1

	//#define WITHUSBHW_HOST		USB_OTG_HS
	#define WITHUSBHOST_HIGHSPEEDPHYC	1	// UTMI -> USB_DP2 & USB_DM2
	//#define WITHUSBHOST_DMAENABLE 1

	#define WITHTINYUSB 1
	#define BOARD_TUH_RHPORT 1
	#define WITHEHCIHW	1	/* USB_EHCI controller */

	#define WITHEHCIHW_EHCIPORT 0	// 0 - use 1st PHY port, 1 - 2nd PHY port (shared with USB_OTG_HS). See also USBPHYC_MISC_SWITHOST_VAL
	#define WITHOHCIHW_OHCIPORT 0

	#define WITHUSBHW_EHCI		USB1_EHCI
	#define WITHUSBHW_EHCI_IRQ	USBH_EHCI_IRQn
	#define WITHUSBHW_EHCI_IX	0

	#define WITHUSBHW_OHCI		USB1HSFSP2_BASE
	#define WITHUSBHW_OHCI_IRQ	USBH_OHCI_IRQn
	#define WITHUSBHW_OHCI_IX	0

	#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
	#define WITHCAT_LWIP		1	/* использовать виртуальный последовательный порт на USB соединении */
	#define WITHMODEM_CDC	1

	#if WITHINTEGRATEDDSP
		#if WITHLWIP

		#elif WITHUSBDEV_HSDESC
			#define WITHUAC2		1	/* UAC2 support */
			#define UACOUT_AUDIO48_SAMPLEBYTES	3	/* должны быть 2, 3 или 4 */
			#define UACIN_AUDIO48_SAMPLEBYTES	3	/* должны быть 2, 3 или 4 */
			#define UACIN_RTS96_SAMPLEBYTES		4	/* должны быть 2, 3 или 4 */
			#define UACIN_RTS192_SAMPLEBYTES	4	/* должны быть 2, 3 или 4 */

			#define WITHUSBUACINOUT	1	/* совмещённое усройство ввода/вывода (без спектра) */
			#define WITHUSBUACOUT		1	/* использовать виртуальную звуковую плату на USB соединении */
			#if WITHRTS96 || WITHRTS192
				#define WITHUSBUACIN	1
				#define WITHUSBUACIN2		1	/* формируются три канала передачи звука */
			#else /* WITHRTS96 || WITHRTS192 */
				#define WITHUSBUACIN	1
			#endif /* WITHRTS96 || WITHRTS192 */
			//#define WITHUABUACOUTAUDIO48MONO	1	/* для уменьшения размера буферов в endpoints */
		#else /* WITHUSBDEV_HSDESC */
			//#define WITHUAC2		1	/* UAC2 support */
			#define UACOUT_AUDIO48_SAMPLEBYTES	2	/* должны быть 2, 3 или 4 */
			#define UACIN_AUDIO48_SAMPLEBYTES	2	/* должны быть 2, 3 или 4 */
			#define WITHUSBUACINOUT	1	/* совмещённое усройство ввода/вывода (без спектра) */
			#define WITHUSBUACOUT		1	/* использовать виртуальную звуковую плату на USB соединении */
			#if WITHRTS96 || WITHRTS192
				#define WITHUSBUACIN	1
				#define WITHUSBUACIN2		1	/* формируются три канала передачи звука */
			#else /* WITHRTS96 || WITHRTS192 */
				#define WITHUSBUACIN	1
			#endif /* WITHRTS96 || WITHRTS192 */
			//#define WITHUABUACOUTAUDIO48MONO	1	/* для уменьшения размера буферов в endpoints */
		#endif /* WITHUSBDEV_HSDESC */

	#endif /* WITHINTEGRATEDDSP */

	#if WITHLWIP
		#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
		//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */
		//#define WITHUSBRNDIS	1	/* RNDIS использовать Remote NDIS на USB соединении */
	#else
		#define WITHUSBCDCACM		1	/* ACM использовать виртуальный последовательный порт на USB соединении */
		#define WITHUSBCDCACM_N	2	/* количество виртуальных последовательных портов */
	#endif /* WITHLWIP */
	//#define WITHUSBHID	1	/* HID использовать Human Interface Device на USB соединении */

	#define WITHUSBDFU	1	/* DFU USB Device Firmware Upgrade support */
	#define WITHUSBWCID	1
	//#define WITHUSBDMTP	1	/* MTP USB Device */

#endif /* WITHISBOOTLOADER */


#define LS020_RS_INITIALIZE() \
	do { \
		arm_hardware_piob_outputs2m(LS020_RS, LS020_RS); /* PB10 */ \
	} while (0)

#define LS020_RESET_INITIALIZE() \
	do { \
		arm_hardware_piob_outputs2m(LS020_RESET, LS020_RESET); /* PB13 */ \
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

	#define LS020_RS_PORT_S(v)		do { GPIOB->BSRR = BSRR_S(v); (void) GPIOB->BSRR; } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOB->BSRR = BSRR_C(v); (void) GPIOB->BSRR; } while (0)
	#define LS020_RS			(UINT32_C(1) << 10)			// PB10 signal

#elif LCDMODE_SPI_RN
	// эти контроллеры требуют только RESET

	#define LS020_RESET_PORT_S(v)		do { GPIOB->BSRR = BSRR_S(v); (void) GPIOB->BSRR; } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOB->BSRR = BSRR_C(v); (void) GPIOB->BSRR; } while (0)
	#define LS020_RESET			(UINT32_C(1) << 13)			// PB13 signal

#elif LCDMODE_SPI_RA
	// Эти контроллеры требуют RESET и RS
	// LCDMODE_UC1608

	#define LS020_RS_PORT_S(v)		do { GPIOB->BSRR = BSRR_S(v); (void) GPIOB->BSRR; } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOB->BSRR = BSRR_C(v); (void) GPIOB->BSRR; } while (0)
	#define LS020_RS			(UINT32_C(1) << 10)			// PB10 signal

	#define LS020_RESET_PORT_S(v)		do { GPIOB->BSRR = BSRR_S(v); (void) GPIOB->BSRR; } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOB->BSRR = BSRR_C(v); (void) GPIOB->BSRR; } while (0)
	#define LS020_RESET			(UINT32_C(1) << 13)			// PB13 signal

#elif LCDMODE_HD44780 && (LCDMODE_SPI == 0)

	#error Unsupported LCDMODE_HD44780

#endif

#if WITHENCODER

	// Выводы подключения енкодера #1
	#define ENCODER_INPUT_PORT	(gpioX_getinputs(GPIOE)) //(gpioX_getinputs(GPIOE))
	#define ENCODER_BITA		(UINT32_C(1) << 1)		// PE1
	#define ENCODER_BITB		(UINT32_C(1) << 0)		// PE0

	// Выводы подключения енкодера #2
	#define ENCODER2_INPUT_PORT	(gpioX_getinputs(GPIOE)) //(gpioX_getinputs(GPIOE))
	#define ENCODER2_BITA		(UINT32_C(1) << 4)		// PE4
	#define ENCODER2_BITB		(UINT32_C(1) << 6)		// PE6


	#define EENCODER_BITS		(ENCODER_BITA | ENCODER_BITB)
	#define EENCODER2_BITS		(ENCODER2_BITA | ENCODER2_BITB)

	/* Определения масок битов для формирования обработчиков прерываний в нужном GPIO */
	#define BOARD_GPIOE_ENCODER_BITS		(ENCODER_BITA | ENCODER_BITB)
	#define BOARD_GPIOE_ENCODER2_BITS		(ENCODER2_BITA | ENCODER2_BITB)

	#define ENCODER_BITS_GET() (((ENCODER_INPUT_PORT & ENCODER_BITA) != 0) * 2 + ((ENCODER_INPUT_PORT & ENCODER_BITB) != 0))
	#define ENCODER2_BITS_GET() (((ENCODER2_INPUT_PORT & ENCODER2_BITA) != 0) * 2 + ((ENCODER2_INPUT_PORT & ENCODER2_BITB) != 0))

	#define ENCODER2_NOSPOOL 1
	#define ENCODER_INITIALIZE() \
		do { \
			static einthandler_t eh1; \
			static einthandler_t eh2; \
			static ticker_t th2; \
			arm_hardware_pioe_inputs(EENCODER_BITS); \
			arm_hardware_pioe_updown(EENCODER_BITS, EENCODER_BITS, 0); \
			einthandler_initialize(& eh1, EENCODER_BITS, spool_encinterrupts, & encoder1); \
			arm_hardware_pioe_onchangeinterrupt(EENCODER_BITS, EENCODER_BITS, EENCODER_BITS, ARM_OVERREALTIME_PRIORITY, TARGETCPU_OVRT, & eh1); \
			arm_hardware_pioe_inputs(EENCODER2_BITS); \
			arm_hardware_pioe_updown(EENCODER2_BITS, EENCODER2_BITS, 0); \
			einthandler_initialize(& eh2, EENCODER2_BITS, spool_encinterrupts, & encoder2); \
			arm_hardware_pioe_onchangeinterrupt(EENCODER2_BITS, EENCODER2_BITS, EENCODER2_BITS, ARM_OVERREALTIME_PRIORITY, TARGETCPU_OVRT, & eh2); \
			/* ticker for spool */ \
			ticker_initialize(& th2, 1, spool_encinterrupts, & encoder2); \
			ticker_add(& th2); \
		} while (0)

#endif

#if WITHI2S2HW

	// Инициализируются I2S2 в дуплексном режиме.
	#define I2S2HW_MASTER_INITIALIZE() do { \
		SPI2->CFG2 |= SPI_CFG2_IOSWP; \
		arm_hardware_pioi_altfn2(UINT32_C(1) << 0,	AF_SPI2); /* PI0 I2S2_WS	*/ \
		arm_hardware_pioi_altfn2(UINT32_C(1) << 1,	AF_SPI2); /* PI1 I2S2_CK	*/ \
		arm_hardware_pioi_altfn2(UINT32_C(1) << 3,	AF_SPI2); /* PI3 I2S2_SDO - приём от кодека */ \
		arm_hardware_pioi_altfn2(UINT32_C(1) << 2,	AF_SPI2); /* PI2 I2S2_SDI, - передача */ \
		arm_hardware_pioc_altfn2(UINT32_C(1) << 6,	AF_SPI2); /* PC6 I2S2_MCK - WS * 256 */ \
		arm_hardware_pioi_altfn20(UINT32_C(1) << 11, AF_SPI1); /* PI11 I2S_CKIN AF_5 */ \
	} while (0)

	// На данной плате не работает - MCK идёт только от процессора
	// Инициализируются I2S2 в дуплексном режиме - WS & BCLK от кодека
	#define I2S2HW_SLAVE_INITIALIZE() do { \
		arm_hardware_pioi_altfn2(UINT32_C(1) << 0,	AF_SPI2); /* PI0 I2S2_WS	*/ \
		arm_hardware_pioi_altfn2(UINT32_C(1) << 1,	AF_SPI2); /* PI1 I2S2_CK	*/ \
		arm_hardware_pioi_altfn2(UINT32_C(1) << 3,	AF_SPI2); /* PI3 I2S2_SDO - приём от кодека */ \
		arm_hardware_pioi_altfn2(UINT32_C(1) << 2,	AF_SPI2); /* PI2 I2S2_SDI, - передача */ \
		arm_hardware_pioc_altfn2(UINT32_C(1) << 6,	AF_SPI2); /* PC6 I2S2_MCK - WS * 256 */ \
		arm_hardware_pioi_altfn20(UINT32_C(1) << 11, AF_SPI1); /* PI11 I2S_CKIN AF_5 */ \
	} while (0)

	#define I2S3HW_SLAVE_RX_INITIALIZE() do { \
	} while (0)

#endif /* WITHI2S2HW */


#if WITHSAI2HW
	/*
	 * SAI2_A - TX, SAI2_B - RX
	 */
	#define SAI2HW_INITIALIZE()	do { \
		arm_hardware_pioi_altfn2(UINT32_C(1) << 7,	AF_SAI2);			/* PI7 - SAI2_FS_A	- 48 kHz	*/ \
		arm_hardware_piod_altfn20(UINT32_C(1) << 13, AF_SAI2);			/* PD13 - SAI2_SCK_A	*/ \
		arm_hardware_pioi_altfn2(UINT32_C(1) << 6,	AF_SAI2);			/* PI6 - SAI2_SD_A	(i2s data to fpga)	*/ \
		arm_hardware_pioe_altfn2(UINT32_C(1) << 11,	AF_SAI2);			/* PE11 - SAI2_SD_B	(i2s data from fpga)	*/ \
	} while (0)
	#define SAI1HW_INITIALIZE()	do { \
	} while (0)
#endif /* WITHSAI2HW */

/* Распределение битов в ARM контроллерах */

#if (WITHCAT && WITHCAT_UART4)
	// CAT data lites
	// RXD at PA10, TXD at PA9

	// CAT control lines
	//#define FROMCAT_TARGET_PIN_RTS		(gpioX_getinputs(GPIOA)) //(gpioX_getinputs(GPIOA))
	//#define FROMCAT_BIT_RTS				(UINT32_C(1) << 11)	/* PA11 сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	//#define FROMCAT_TARGET_PIN_DTR		(gpioX_getinputs(GPIOA)) //(gpioX_getinputs(GPIOA))
	//#define FROMCAT_BIT_DTR				(UINT32_C(1) << 12)	/* PA12 сигнал DTR от FT232RL	*/

	/* манипуляция от порта RS-232 */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
		} while (0)

	/* переход на передачу от порта RS-232 */
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
		} while (0)

#endif /* (WITHCAT && WITHCAT_UART4) */

#if (WITHCAT && WITHCAT_CDC)

	// CAT data lites
	// RXD at PA10, TXD at PA9

	// CAT control lines
	//#define FROMCAT_TARGET_PIN_RTS		(gpioX_getinputs(GPIOA)) //(gpioX_getinputs(GPIOA)) // was PINA
	//#define FROMCAT_BIT_RTS				(UINT32_C(1) << 11)	/* сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	//#define FROMCAT_TARGET_PIN_DTR		(gpioX_getinputs(GPIOA)) //(gpioX_getinputs(GPIOA)) // was PINA
	//#define FROMCAT_BIT_DTR				(UINT32_C(1) << 12)	/* сигнал DTR от FT232RL	*/

	/* манипуляция от виртуального CDC порта */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
		} while (0)

	/* переход на передачу от виртуального CDC порта*/
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
		} while (0)

#endif /* (WITHCAT && WITHCAT_CDC) */

#if WITHLFM
		#define BOARD_PPSIN_BIT (UINT32_C(1) << 14)		/* PD7 - PPS signal from GPS */

		#define NMEA_INITIALIZE() do { \
			static einthandler_t h; \
			arm_hardware_piod_inputs(BOARD_PPSIN_BIT); \
			arm_hardware_piod_updown(BOARD_PPSIN_BIT, 0, BOARD_PPSIN_BIT); \
			arm_hardware_piod_onchangeinterrupt(BOARD_PPSIN_BIT, 1 * BOARD_PPSIN_BIT, 0 * BOARD_PPSIN_BIT, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM, & h); \
		} while (0)

#endif /* WITHLFM */

#if WITHSDHCHW
	#if WITHSDHCHW4BIT
		#define HARDWARE_SDIO_INITIALIZE()	do { \
			arm_hardware_piod_altfn50(UINT32_C(1) << 2, AF_SDIO);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_altfn50(UINT32_C(1) << 12, AF_SDIO);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_altfn50(UINT32_C(1) << 8, AF_SDIO);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_altfn50(UINT32_C(1) << 9, AF_SDIO);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_altfn50(UINT32_C(1) << 10, AF_SDIO);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_altfn50(UINT32_C(1) << 11, AF_SDIO);	/* PC11 - SDIO_D3	*/ \
			arm_hardware_piod_updown(UINT32_C(1) << 2, UINT32_C(1) << 2, 0);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(UINT32_C(1) << 12, UINT32_C(1) << 12, 0);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(UINT32_C(1) << 8, UINT32_C(1) << 8, 0);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_updown(UINT32_C(1) << 9, UINT32_C(1) << 9, 0);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_updown(UINT32_C(1) << 10, UINT32_C(1) << 10, 0);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_updown(UINT32_C(1) << 11, UINT32_C(1) << 11, 0);	/* PC11 - SDIO_D3	*/ \
		} while (0)
		/* отключить процессор от SD карты - чтобы при выполнении power cycle не возникало фантомное питание через сигналы управления. */
		#define HARDWARE_SDIO_HANGOFF()	do { \
			arm_hardware_piod_updown(UINT32_C(1) << 2, 0, UINT32_C(1) << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(UINT32_C(1) << 12, 0, UINT32_C(1) << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(UINT32_C(1) << 8, 0, UINT32_C(1) << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_updown(UINT32_C(1) << 9, 0, UINT32_C(1) << 9);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_updown(UINT32_C(1) << 10, 0, UINT32_C(1) << 10);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_updown(UINT32_C(1) << 11, 0, UINT32_C(1) << 11);	/* PC11 - SDIO_D3	*/ \
			arm_hardware_piod_inputs(UINT32_C(1) << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_inputs(UINT32_C(1) << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_inputs(UINT32_C(1) << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_inputs(UINT32_C(1) << 9);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_inputs(UINT32_C(1) << 10);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_inputs(UINT32_C(1) << 11);	/* PC11 - SDIO_D3	*/ \
		} while (0)
	#else /* WITHSDHCHW4BIT */
		#define HARDWARE_SDIO_INITIALIZE()	do { \
			arm_hardware_piod_altfn50(UINT32_C(1) << 2, AF_SDIO);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_altfn50(UINT32_C(1) << 12, AF_SDIO);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_altfn50(UINT32_C(1) << 8, AF_SDIO);	/* PC8 - SDIO_D0	*/ \
		} while (0)
		/* отключить процессор от SD карты - чтобы при выполнении power cycle не возникало фантомное питание через сигналы управления. */
		#define HARDWARE_SDIO_HANGOFF()	do { \
			arm_hardware_piod_inputs(UINT32_C(1) << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_inputs(UINT32_C(1) << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_inputs(UINT32_C(1) << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_piod_updown(UINT32_C(1) << 2, 0, UINT32_C(1) << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(UINT32_C(1) << 12, 0, UINT32_C(1) << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(UINT32_C(1) << 8, 0, UINT32_C(1) << 8);	/* PC8 - SDIO_D0	*/ \
		} while (0)
	#endif /* WITHSDHCHW4BIT */

	#define HARDWARE_SDIO_WP_BIT	0//(UINT32_C(1) << 8)	/* Pxx - SDIO_WP */
	#define HARDWARE_SDIO_CD_BIT	(UINT32_C(1) << 7)	/* PB7 - SDIO_SENSE */

	#define HARDWARE_SDIOSENSE_CD() (gpioX_getinputs(GPIOB) & HARDWARE_SDIO_CD_BIT) == 0)	/* получить состояние датчика CARD PRESENT */
	#define HARDWARE_SDIOSENSE_WP() 0//(gpioX_getinputs(GPIOG) & HARDWARE_SDIO_WP_BIT) != 0)	/* получить состояние датчика CARD WRITE PROTECT (0 - write enabled) */

	#define HARDWARE_SDIOSENSE_INITIALIZE()	do { \
			arm_hardware_piog_inputs(HARDWARE_SDIO_WP_BIT); /* Pxx - SDIO_WP */ \
			arm_hardware_piog_updown(HARDWARE_SDIO_WP_BIT, HARDWARE_SDIO_WP_BIT, 0); \
			arm_hardware_piob_inputs(HARDWARE_SDIO_CD_BIT); /* PB7 - SDIO_SENSE */ \
			arm_hardware_piob_updown(HARDWARE_SDIO_CD_BIT, HARDWARE_SDIO_CD_BIT, 0); \
	} while (0)

	#define HARDWARE_SDIOPOWER_BIT (UINT32_C(1) << 12)	/* PB12 */
	#define HARDWARE_SDIOPOWER_C(v)	do { GPIOB->BSRR = BSRR_C(v); (void) GPIOB->BSRR; } while (0)
	#define HARDWARE_SDIOPOWER_S(v)	do { GPIOB->BSRR = BSRR_S(v); (void) GPIOB->BSRR; } while (0)
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
	#define TXGFV_RX		(UINT32_C(1) << 4)
	#define TXGFV_TRANS		0			// переход между режимами приёма и передачи
	#define TXGFV_TX_SSB	(UINT32_C(1) << 0)
	#define TXGFV_TX_CW		(UINT32_C(1) << 1)
	#define TXGFV_TX_AM		(UINT32_C(1) << 2)
	#define TXGFV_TX_NFM	(UINT32_C(1) << 3)

	#define TXPATH_INITIALIZE() \
		do { \
		} while (0)


	// +++
	// TXDISABLE input - PZ3
	#define TXDISABLE_TARGET_PIN				(gpioX_getinputs(GPIOZ))
	#define TXDISABLE_BIT_TXDISABLE				(UINT32_C(1) << 3)		// PZ3 - TX INHIBIT
	// получить бит запрета передачи (от усилителя мощности)
	#define HARDWARE_GET_TXDISABLE() ((TXDISABLE_TARGET_PIN & TXDISABLE_BIT_TXDISABLE) != 0)
	#define TXDISABLE_INITIALIZE() \
		do { \
			arm_hardware_pioz_inputs(TXDISABLE_BIT_TXDISABLE); \
			arm_hardware_pioz_updown(TXDISABLE_BIT_TXDISABLE, 0, TXDISABLE_BIT_TXDISABLE); \
		} while (0)
	// ---

	// +++
	// PTT input - PF2
	// PTT2 input - PF3
	// PTT3 input - PF15
	#define PTT_TARGET_PIN				(gpioX_getinputs(GPIOF))
	#define PTT_BIT_PTT					(UINT32_C(1) << 2)		// PF2 - PTT
	#define PTT2_TARGET_PIN				(gpioX_getinputs(GPIOF))
	#define PTT2_BIT_PTT				(UINT32_C(1) << 3)		// PF3 - PTT2
	#define PTT3_TARGET_PIN				(gpioX_getinputs(GPIOF))
	#define PTT3_BIT_PTT				(UINT32_C(1) << 15)		// PF15 - PTT3
	// получить бит запроса оператором перехода на пердачу
	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0 || (PTT2_TARGET_PIN & PTT2_BIT_PTT) == 0 || (PTT3_TARGET_PIN & PTT3_BIT_PTT) == 0)
	#define PTT_INITIALIZE() \
		do { \
			arm_hardware_piof_inputs(PTT_BIT_PTT); \
			arm_hardware_piof_updown(PTT_BIT_PTT, PTT_BIT_PTT, 0); \
			arm_hardware_piof_inputs(PTT2_BIT_PTT); \
			arm_hardware_piof_updown(PTT2_BIT_PTT, PTT2_BIT_PTT, 0); \
			arm_hardware_piof_inputs(PTT3_BIT_PTT); \
			arm_hardware_piof_updown(PTT3_BIT_PTT, PTT3_BIT_PTT, 0); \
		} while (0)
	// ---
	// TUNE input - PF4
	#define TUNE_TARGET_PIN				(gpioX_getinputs(GPIOF))
	#define TUNE_BIT_TUNE					(UINT32_C(1) << 4)		// PF4
	#define HARDWARE_GET_TUNE() ((TUNE_TARGET_PIN & TUNE_BIT_TUNE) == 0)
	#define TUNE_INITIALIZE() \
		do { \
			arm_hardware_piof_inputs(TUNE_BIT_TUNE); \
			arm_hardware_piof_updown(TUNE_BIT_TUNE, TUNE_BIT_TUNE, 0); \
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
	#define ELKEY_BIT_LEFT				(UINT32_C(1) << 0)		// PF0
	#define ELKEY_BIT_RIGHT				(UINT32_C(1) << 1)		// PF1

	#define ELKEY_TARGET_PIN			(gpioX_getinputs(GPIOF))

	#define HARDWARE_GET_ELKEY_LEFT() 	((ELKEY_TARGET_PIN & ELKEY_BIT_LEFT) == 0)
	#define HARDWARE_GET_ELKEY_RIGHT() 	((ELKEY_TARGET_PIN & ELKEY_BIT_RIGHT) == 0)

	#define ELKEY_INITIALIZE() \
		do { \
			arm_hardware_piof_inputs(ELKEY_BIT_LEFT); \
			arm_hardware_piof_updown(ELKEY_BIT_LEFT, ELKEY_BIT_LEFT, 0); \
			arm_hardware_piof_inputs(ELKEY_BIT_RIGHT); \
			arm_hardware_piof_updown(ELKEY_BIT_RIGHT, ELKEY_BIT_RIGHT, 0); \
		} while (0)

#endif /* WITHELKEY */

// IOUPDATE = PA15
//#define SPI_IOUPDATE_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); (void) GPIOA->BSRR; } while (0)
//#define SPI_IOUPDATE_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); (void) GPIOA->BSRR; } while (0)
//#define SPI_IOUPDATE_BIT		(UINT32_C(1) << 15)	// * PA15

#if WITHSPIHW || WITHSPISW
	// Набор определений для работы без внешнего дешифратора
	#define SPI_ALLCS_PORT_S(v)	do { GPIOH->BSRR = BSRR_S(v); (void) GPIOH->BSRR; } while (0)
	#define SPI_ALLCS_PORT_C(v)	do { GPIOH->BSRR = BSRR_C(v); (void) GPIOH->BSRR; } while (0)

	#define targetext1		(UINT32_C(1) << 7)		// PH7 ext1 on front panel
	#define targetxad2		(UINT32_C(1) << 2)		// PH2 ext2 двунаправленный SPI для подключения внешних устройств - например тюнера
	#define targetnvram		(UINT32_C(1) << 8)		// PH8 nvram FM25L16B
	#define targetctl1		(UINT32_C(1) << 5)		// PH5 board control registers chain
	#define targetcodec1	(UINT32_C(1) << 3)		// PH3 on-board codec1 NAU8822L
	#define targetadc2		(UINT32_C(1) << 6) 		// PH6 ADC MCP3208-BI/SL chip select (potentiometers)
	#define targetfpga1		(UINT32_C(1) << 14)		// PH14 FPGA control registers CS1

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
	#define targetnone 0				/* FPGA image loader pseudo chip select signal */

	#define SPI_ALLCS_BITSNEG 0		// Выходы, активные при "1"

	//#define SPI_NAEN_PORT_S(v)	do { GPIOx->BSRR = BSRR_S(v); (void) GPIOx->BSRR; } while (0)
	//#define SPI_NAEN_PORT_C(v)	do { GPIOx->BSRR = BSRR_C(v); (void) GPIOx->BSRR; } while (0)

	//#define SPI_NAEN_BIT (UINT32_C(1) << xx7)		// Pxx used

	/* Perform delay after assert or de-assert specific CS line */
	#define SPI_CS_DELAY(target) do { \
		switch (target) { \
		case targetxad2: local_delay_us(5); break; /* external SPI device (PA BOARD ADC) */ \
		case targetctl1: local_delay_us(5); break; /* board control registers chain */ \
		default: break; \
		} \
	} while (0)

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
	#define	SPI_SCLK_BIT			(UINT32_C(1) << 0)	// PZ0 бит, через который идет синхронизация SPI

	#define SPI_TARGET_MOSI_PORT_C(v)	do { GPIOZ->BSRR = BSRR_C(v); (void) GPIOZ->BSRR; } while (0)
	#define SPI_TARGET_MOSI_PORT_S(v)	do { GPIOZ->BSRR = BSRR_S(v); (void) GPIOZ->BSRR; } while (0)
	#define	SPI_MOSI_BIT			(UINT32_C(1) << 2)	// PZ2 бит, через который идет вывод

	#define SPI_TARGET_MISO_PIN		(gpioX_getinputs(GPIOZ))
	#define	SPI_MISO_BIT			(UINT32_C(1) << 1)	// PZ1 бит, через который идет ввод с SPI.

	#define SPIIO_INITIALIZE() do { \
			arm_hardware_pioz_outputs50m(SPI_SCLK_BIT, SPI_SCLK_BIT); /* PZ0 */ \
			arm_hardware_pioz_outputs50m(SPI_MOSI_BIT, SPI_MOSI_BIT); /* PZ2 */ \
			arm_hardware_pioz_inputs(SPI_MISO_BIT); /* PZ1 */ \
		} while (0)
	#define HARDWARE_SPI_CONNECT() do { \
			arm_hardware_pioz_altfn20(SPI_SCLK_BIT, AF_SPI1); /* AF_5 В этих процессорах и входы и выходы переключаются на ALT FN */ \
			arm_hardware_pioz_altfn20(SPI_MOSI_BIT, AF_SPI1); /* AF_5 В этих процессорах и входы и выходы переключаются на ALT FN */ \
			arm_hardware_pioz_altfn20(SPI_MISO_BIT, AF_SPI1); /* AF_5 В этих процессорах и входы и выходы переключаются на ALT FN */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT() do { \
			arm_hardware_pioz_outputs50m(SPI_SCLK_BIT, SPI_SCLK_BIT); \
			arm_hardware_pioz_outputs50m(SPI_MOSI_BIT, SPI_MOSI_BIT); \
			arm_hardware_pioz_inputs(SPI_MISO_BIT); \
		} while (0)
	#define HARDWARE_SPI_CONNECT_MOSI() do { \
			arm_hardware_pioz_altfn20(SPI_MOSI_BIT, AF_SPI1);	/* AF_5 PIO disable for MOSI bit */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
			arm_hardware_pioz_outputs50m(SPI_MOSI_BIT, SPI_MOSI_BIT);	/* PIO enable for MOSI bit */ \
		} while (0)

#endif /* WITHSPIHW || WITHSPISW */

// WITHUART1HW
#define HARDWARE_UART1_INITIALIZE() do { \
		const uint_fast32_t TXMASK = (UINT32_C(1) << 11); /* PG11: USART1_TX TX DATA line (2 MHz) */ \
		const uint_fast32_t RXMASK = (UINT32_C(1) << 2); /* PB2: USART1_RX RX DATA line (2 MHz) - pull-up RX data */  \
		arm_hardware_piog_altfn2(TXMASK, 4); /* PG11 AF4 */ \
		arm_hardware_piob_altfn2(RXMASK, 4); /* PB2 AF4 */ \
		arm_hardware_piob_updown(RXMASK, RXMASK, 0); \
	} while (0)

#define TARGET_ENC2BTN_BIT (UINT32_C(1) << 12)	// PE12 - second encoder button with pull-up

#if WITHKEYBOARD
	/* PE12: pull-up second encoder button */

	#define TARGET_POWERBTN_BIT (UINT32_C(1) << 5)	// PF5 - ~CPU_POWER_SW signal

#if WITHENCODER2
	// P7_8
	#define TARGET_ENC2BTN_GET	(((gpioX_getinputs(GPIOE)) & TARGET_ENC2BTN_BIT) == 0)
#endif /* WITHENCODER2 */

#if WITHPWBUTTON
	// P5_3 - ~CPU_POWER_SW signal
	#define TARGET_POWERBTN_GET	(((gpioX_getinputs(GPIOF)) & TARGET_POWERBTN_BIT) == 0)
#endif /* WITHPWBUTTON */

	#define HARDWARE_KBD_INITIALIZE() do { \
			arm_hardware_pioe_inputs(TARGET_ENC2BTN_BIT); \
			arm_hardware_pioe_updown(TARGET_ENC2BTN_BIT, TARGET_ENC2BTN_BIT, 0); /* PE12: pull-up second encoder button */ \
			arm_hardware_piof_inputs(TARGET_POWERBTN_BIT); \
			arm_hardware_piof_updown(TARGET_POWERBTN_BIT, TARGET_POWERBTN_BIT, 0);	/* PF5: pull-up second encoder button */ \
		} while (0)

#else /* WITHKEYBOARD */

	#define HARDWARE_KBD_INITIALIZE() do { \
		} while (0)

#endif /* WITHKEYBOARD */

#if 1 // WITHTWISW
	// I2C2_SDA	PZ5
	// I2C2_SCL	PZ4
	#define TARGET_TWI_TWCK		(UINT32_C(1) << 4)		// I2C2_SCL	PZ4
	#define TARGET_TWI_TWCK_PIN		(gpioX_getinputs(GPIOZ))
	#define TARGET_TWI_TWCK_PORT_C(v) do { GPIOZ->BSRR = BSRR_C(v); (void) GPIOZ->BSRR; } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do { GPIOZ->BSRR = BSRR_S(v); (void) GPIOZ->BSRR; } while (0)

	#define TARGET_TWI_TWD		(UINT32_C(1) << 5)		// I2C2_SDA	PZ5
	#define TARGET_TWI_TWD_PIN		(gpioX_getinputs(GPIOZ))
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
	#define FPGA_NCONFIG_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); (void) GPIOA->BSRR; } while (0)
	#define FPGA_NCONFIG_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); (void) GPIOA->BSRR; } while (0)
	#define FPGA_NCONFIG_BIT		(UINT32_C(1) << 10)	/* PA10 bit connected to nCONFIG pin ALTERA FPGA */

	/* inputs */
	#define FPGA_CONF_DONE_INPUT	(gpioX_getinputs(GPIOA))
	#define FPGA_CONF_DONE_BIT		(UINT32_C(1) << 15)	/* PA15 bit connected to CONF_DONE pin ALTERA FPGA */

	#define FPGA_NSTATUS_INPUT		(gpioX_getinputs(GPIOA))
	#define FPGA_NSTATUS_BIT		(UINT32_C(1) << 14)	/* PA14 bit connected to NSTATUS pin ALTERA FPGA */

	#define FPGA_INIT_DONE_INPUT	(gpioX_getinputs(GPIOF))
	#define FPGA_INIT_DONE_BIT		(UINT32_C(1) << 11)	/* PF11 bit connected to INIT_DONE pin ALTERA FPGA */

	/* Инициадизация выводов GPIO процессора для получения состояния и управлением загрузкой FPGA */
	#define HARDWARE_FPGA_LOADER_INITIALIZE() do { \
			arm_hardware_pioa_outputs(FPGA_NCONFIG_BIT, FPGA_NCONFIG_BIT); \
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

	#define TARGET_FPGA_FIR_CS_PORT_C(v)	do { GPIOG->BSRR = BSRR_C(v); (void) GPIOG->BSRR; } while (0)
	#define TARGET_FPGA_FIR_CS_PORT_S(v)	do { GPIOG->BSRR = BSRR_S(v); (void) GPIOG->BSRR; } while (0)
	#define TARGET_FPGA_FIR_CS_BIT (UINT32_C(1) << 8)	/* PG8 - fir CS ~FPGA_FIR_CLK */

	#define TARGET_FPGA_FIR1_WE_PORT_C(v)	do { GPIOG->BSRR = BSRR_C(v); (void) GPIOG->BSRR; } while (0)
	#define TARGET_FPGA_FIR1_WE_PORT_S(v)	do { GPIOG->BSRR = BSRR_S(v); (void) GPIOG->BSRR; } while (0)
	#define TARGET_FPGA_FIR1_WE_BIT (UINT32_C(1) << 2)	/* PG2 - fir1 WE */

	#define TARGET_FPGA_FIR2_WE_PORT_C(v)	do { GPIOG->BSRR = BSRR_C(v); (void) GPIOG->BSRR; } while (0)
	#define TARGET_FPGA_FIR2_WE_PORT_S(v)	do { GPIOG->BSRR = BSRR_S(v); (void) GPIOG->BSRR; } while (0)
	#define TARGET_FPGA_FIR2_WE_BIT (UINT32_C(1) << 3)	/* PG3 - fir2 WE */

	#define TARGET_FPGA_FIR_INITIALIZE() do { \
			arm_hardware_piog_outputs2m(TARGET_FPGA_FIR1_WE_BIT, TARGET_FPGA_FIR1_WE_BIT); \
			arm_hardware_piog_outputs2m(TARGET_FPGA_FIR2_WE_BIT, TARGET_FPGA_FIR2_WE_BIT); \
			arm_hardware_piog_outputs2m(TARGET_FPGA_FIR_CS_BIT, TARGET_FPGA_FIR_CS_BIT); \
		} while (0)
#endif /* WITHDSPEXTFIR */

#if 1
	/* получение состояния переполнения АЦП */
	// PI8
	#define TARGET_FPGA_OVF_BIT			(UINT32_C(1) << 8)	// PI8
	#define TARGET_FPGA_OVF_GET			((((gpioX_getinputs(GPIOI))) & TARGET_FPGA_OVF_BIT) == 0)	// 1 - overflow active
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

#if WITHETHHW

	/* From MYC-YA157C-V3 Product Manual */
	#define HARDWARE_ETH_INITIALIZE() do { \
		arm_hardware_pioa_altfn50((UINT32_C(1) << 1), AF_ETH);		/* ETH_RX_CLK PA1 */ \
		arm_hardware_pioa_altfn50((UINT32_C(1) << 7), AF_ETH);		/* ETH_RX_DV PA7 */ \
		arm_hardware_pioc_altfn50((UINT32_C(1) << 4), AF_ETH);		/* ETH_RXD0 PC4 */ \
		arm_hardware_pioc_altfn50((UINT32_C(1) << 5), AF_ETH);		/* ETH_RXD1 PC5 */ \
		arm_hardware_piob_altfn50((UINT32_C(1) << 0), AF_ETH);		/* ETH_RXD2 PB0 */ \
		arm_hardware_piob_altfn50((UINT32_C(1) << 1), AF_ETH);		/* ETH_RXD3 PB1 */ \
		arm_hardware_piob_altfn50((UINT32_C(1) << 11), AF_ETH);		/* ETH_TX_EN PB11 */ \
		arm_hardware_piog_altfn50((UINT32_C(1) << 13), AF_ETH);		/* ETH_TXD0 PG13 */ \
		arm_hardware_piog_altfn50((UINT32_C(1) << 14), AF_ETH);		/* ETH_TXD1 PG14 */ \
		arm_hardware_pioc_altfn50((UINT32_C(1) << 2), AF_ETH);		/* ETH_TXD2 PC2 */ \
		arm_hardware_pioe_altfn50((UINT32_C(1) << 2), AF_ETH);		/* ETH_TXD3 PE2 */ \
		arm_hardware_piog_altfn50((UINT32_C(1) << 4), AF_ETH);		/* ETH_GTX_CLK PG4 */ \
		arm_hardware_piog_altfn50((UINT32_C(1) << 0), AF_ETH);		/* ETH_RST PG0 */ \
		arm_hardware_pioa_altfn50((UINT32_C(1) << 2), AF_ETH);		/* ETH_MDIO PA2 */ \
		arm_hardware_pioc_altfn50((UINT32_C(1) << 1), AF_ETH);		/* ETH_MDC PC1 */ \
		arm_hardware_piog_altfn50((UINT32_C(1) << 5), AF_ETH);		/* CLK125_NDO PG5 */ \
	} while (0)

#endif /* WITHETHHW */

#if WITHUSBHW

	#define TARGET_USBFS_VBUSON_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); (void) GPIOA->BSRR; } while (0)
	#define TARGET_USBFS_VBUSON_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); (void) GPIOA->BSRR; } while (0)
	#define TARGET_USBFS_VBUSON_BIT (UINT32_C(1) << 11)	// PA11 - нулём включение питания для device

	/**USB_OTG_FS GPIO Configuration    
	PA9     ------> USB_OTG_FS_VBUS
	PA10     ------> USB_OTG_FS_ID
	PA11     ------> USB_OTG_FS_DM
	PA12     ------> USB_OTG_FS_DP 
	*/

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
		/*arm_hardware_pioa_altfn50((UINT32_C(1) << 11) | (UINT32_C(1) << 12), AF_OTGFS);	*/		/* PA10, PA11, PA12 - USB_OTG_FS	*/ \
		/* arm_hardware_pioa_inputs(UINT32_C(1) << 9);	*/	/* PA9 - USB_OTG_FS_VBUS */ \
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
		arm_hardware_piob_altfn2((UINT32_C(1) << 9), AF_TIM17); /* PB9 - TIM17_CH1 */ \
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

	/* BL0: PZ6. BL1: PZ7 */
	#define	HARDWARE_BL_INITIALIZE() do { \
		const portholder_t ENmask = (UINT32_C(1) << 9); /* PD9 */ \
		const portholder_t BLpins = (UINT32_C(1) << 7) | (UINT32_C(1) << 6); /* PZ7:PZ6 */ \
		arm_hardware_pioz_opendrain(BLpins, 0); \
		arm_hardware_piod_outputs2m(ENmask, ENmask);  \
		} while (0)

	/* установка яркости и включение/выключение преобразователя подсветки */
	/* BL0: PZ6. BL1: PZ7 */
	// PD9 BL Enable
	#define HARDWARE_BL_SET(en, level) do { \
		const portholder_t Vlevel = (level) & 0x03; \
		const portholder_t ENmask = (UINT32_C(1) << 9); /* PD9 */ \
		const portholder_t BLpins = (UINT32_C(1) << 7) | (UINT32_C(1) << 6); /* PZ7:PZ6 */ \
		const portholder_t BLstate = (~ Vlevel) << 6; \
		gpioX_setstate(GPIOZ, BLpins, BLstate); \
		gpioX_setstate(GPIOD, ENmask, !! (en) * ENmask); \
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
		const uint32_t MODEmask = (UINT32_C(1) << 10); /* PB10 - FPLCD_CD */ \
		const uint32_t RESETmask = (UINT32_C(1) << 13); /* PB13 - FPLCD_RESET */ \
		const uint32_t DEmask = (UINT32_C(1) << 13); /* PE13 - DE */ \
		const uint32_t HSmask = (UINT32_C(1) << 10); /* PI10 - HSYNC */ \
		const uint32_t VSmask = (UINT32_C(1) << 9); 	/* PI9 - VSYNC */ \
		/* LCD RESET */ \
		arm_hardware_piob_outputs2m(RESETmask, RESETmask); /* PD4 - FPLCD_RESET */ \
		/* Bit clock */ \
		arm_hardware_piog_altfn50((UINT32_C(1) << 7), GPIO_AF_LTDC14);		/* CLK PG7 AF_14*/ \
		/* Control */ \
		arm_hardware_piob_outputs(MODEmask, ((demode) != 0) * MODEmask);	/* PD3 MODEmask=state */ \
		/* Synchronisation signals in SYNC mode */ \
		arm_hardware_pioe_outputs(((demode) == 0) * DEmask, 0);	/* DE=0 (DISP, pin 31) */ \
		arm_hardware_pioi_altfn50(((demode) == 0) * VSmask, GPIO_AF_LTDC14);	/* VSYNC */ \
		arm_hardware_pioi_altfn50(((demode) == 0) * HSmask, GPIO_AF_LTDC14);	/* HSYNC */ \
		/* Synchronisation signals in DE mode*/ \
		arm_hardware_pioe_altfn50(((demode) != 0) * DEmask, GPIO_AF_LTDC14);	/* DE */ \
		arm_hardware_pioi_outputs(((demode) != 0) * VSmask, VSmask);	/* VSYNC */ \
		arm_hardware_pioi_outputs(((demode) != 0) * HSmask, HSmask);	/* HSYNC */ \
		/* RED */ \
		arm_hardware_pioh_altfn50((UINT32_C(1) << 9), GPIO_AF_LTDC14);		/* PH9 R3 */ \
		arm_hardware_pioh_altfn50((UINT32_C(1) << 10), GPIO_AF_LTDC14);		/* PH10 R4 */ \
		arm_hardware_pioh_altfn50((UINT32_C(1) << 11), GPIO_AF_LTDC14);		/* PH11 R5 */ \
		arm_hardware_pioh_altfn50((UINT32_C(1) << 12), GPIO_AF_LTDC14);		/* PH12 R6 */ \
		arm_hardware_pioe_altfn50((UINT32_C(1) << 15), GPIO_AF_LTDC14);		/* PE15 R7 */ \
		/* GREEN */ \
		arm_hardware_pioh_altfn50((UINT32_C(1) << 13), GPIO_AF_LTDC14);		/* PH13 G2 */ \
		arm_hardware_piog_altfn50((UINT32_C(1) << 10), GPIO_AF_LTDC9);		/* PG10 G3 */ \
		arm_hardware_pioh_altfn50((UINT32_C(1) << 15), GPIO_AF_LTDC14);		/* PH15 G4 */ \
		arm_hardware_pioh_altfn50((UINT32_C(1) << 4), GPIO_AF_LTDC9);		/* PH4 G5 */ \
		arm_hardware_pioc_altfn50((UINT32_C(1) << 7), GPIO_AF_LTDC14);		/* PC7 G6 */ \
		arm_hardware_piob_altfn50((UINT32_C(1) << 5), GPIO_AF_LTDC14);		/* PB5 G7 */ \
		/* BLUE */ \
		arm_hardware_piod_altfn50((UINT32_C(1) << 10), GPIO_AF_LTDC14);		/* PD10 B3 */ \
		arm_hardware_piog_altfn50((UINT32_C(1) << 12), GPIO_AF_LTDC9);		/* PG12 B4 */ \
		arm_hardware_pioi_altfn50((UINT32_C(1) << 5), GPIO_AF_LTDC14);		/* PI5 B5 */ \
		arm_hardware_piob_altfn50((UINT32_C(1) << 8), GPIO_AF_LTDC14);		/* PB8 B6 */ \
		arm_hardware_piod_altfn50((UINT32_C(1) << 8), GPIO_AF_LTDC14);		/* PD8 B7 */ \
	} while (0)

	/* управление состоянием сигнала DISP панели */
	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_SET_DISP(state) do { \
		const uint32_t DEmask = (UINT32_C(1) << 13); /* PE13 - DE */ \
		const uint32_t HSmask = (UINT32_C(1) << 10); /* PI10 - HSYNC */ \
		const uint32_t VSmask = (UINT32_C(1) << 9); 	/* PI9 - VSYNC */ \
		/* while ((GPIOA->IDR & VSmask) != 0) ; */ /* схема синхронизации стоит на плате дисплея. дождаться 0 */ \
		/* while ((GPIOA->IDR & VSmask) == 0) ; */ /* дождаться 1 */ \
		arm_hardware_pioe_outputs(DEmask, ((state) != 0) * DEmask); /* DE=DISP, pin 31 - можно менять только при VSYNC=1 */ \
	} while (0)
#endif /* WITHLTDCHW */

	/* Выводы соединения с QSPI BOOT NOR FLASH */
	#define SPDIF_MISO_BIT (UINT32_C(1) << 9)	// PF9	QUADSPI_BK1_IO1
	#define SPDIF_MOSI_BIT (UINT32_C(1) << 8)	// PF8	QUADSPI_BK1_IO0
	#define SPDIF_SCLK_BIT (UINT32_C(1) << 10)	// PF10	QUADSPI_CLK
	#define SPDIF_NCS_BIT (UINT32_C(1) << 6)		// PB6	QUADSPI_BK1_NCS

	#define SPDIF_D2_BIT (UINT32_C(1) << 7)		// PF7	QUADSPI_BK1_IO2
	#define SPDIF_D3_BIT (UINT32_C(1) << 6)		// PF6	QUADSPI_BK1_IO3
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

			#define SPIDF_MISO() (((gpioX_getinputs(GPIOF)) & SPDIF_MISO_BIT) != 0)
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

		void stmpe811_interrupt_handler(void * ctx);

		#define BOARD_STMPE811_INT_PIN (UINT32_C(1) << 14)		/* PE14 : tsc interrupt XS26, pin 08 */

		#define BOARD_STMPE811_INT_CONNECT() do { \
			arm_hardware_pioe_inputs(BOARD_STMPE811_INT_PIN); \
			arm_hardware_pioe_updown(BOARD_STMPE811_INT_PIN, BOARD_STMPE811_INT_PIN, 0); \
			arm_hardware_pioe_onchangeinterrupt(BOARD_STMPE811_INT_PIN, 1 * BOARD_STMPE811_INT_PIN, 0 * BOARD_STMPE811_INT_PIN, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM); \
		} while (0)

	#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811) */

	#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_GT911)

		//	tsc interrupt XS26, pin 08
		//	tsc/LCD reset, XS26, pin 22
		//	tsc SCL: XS26, pin 01
		//	tsc SDA: XS26, pin 02

		void gt911_interrupt_handler(void * ctx);

		#define BOARD_GT911_INT_PIN (UINT32_C(1) << 14)		/* PE14 : tsc interrupt XS26, pin 08 */
		#define BOARD_GT911_RESET_PIN (UINT32_C(1) << 13)	/* PB13 : tsc/LCD reset, XS26, pin 22 */

		#define BOARD_GT911_RESET_SET(v) do { if (v) GPIOB->BSRR = BSRR_S(BOARD_GT911_RESET_PIN); else GPIOB->BSRR = BSRR_C(BOARD_GT911_RESET_PIN); (void) GPIOB->BSRR; } while (0)
		#define BOARD_GT911_INT_SET(v) do { if (v) GPIOE->BSRR = BSRR_S(BOARD_GT911_INT_PIN); else GPIOE->BSRR = BSRR_C(BOARD_GT911_INT_PIN); (void) GPIOE->BSRR; } while (0)

		#define BOARD_GT911_RESET_INITIO_1() do { \
			arm_hardware_pioe_outputs2m(BOARD_GT911_INT_PIN, 1* BOARD_GT911_INT_PIN); \
			arm_hardware_piob_outputs2m(BOARD_GT911_RESET_PIN, 1 * BOARD_GT911_RESET_PIN); \
			 local_delay_ms(200);  \
		} while (0)

		#define BOARD_GT911_RESET_INITIO_2() do { \
			arm_hardware_piob_inputs(BOARD_GT911_INT_PIN); \
			arm_hardware_piob_updown(BOARD_GT911_INT_PIN, BOARD_GT911_INT_PIN, 0); \
		} while (0)

		#define BOARD_GT911_INT_CONNECT() do { \
			arm_hardware_pioe_inputs(BOARD_GT911_INT_PIN); \
			arm_hardware_pioe_updown(BOARD_GT911_INT_PIN, BOARD_GT911_INT_PIN, 0); \
			arm_hardware_pioe_onchangeinterrupt(BOARD_GT911_INT_PIN, 1 * BOARD_GT911_INT_PIN, 0 * BOARD_GT911_INT_PIN, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM); \
		} while (0)
		//gt911_interrupt_handler

	#endif

	#define BOARD_BLINK_BIT (UINT32_C(1) << 13)	// PA13 - led on Storch board

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
	#define BOARD_GPIOG_USERBOOT_BIT	(UINT32_C(1) << 15)	/* PG15: ~USER_BOOT */
	#define BOARD_IS_USERBOOT() (((gpioX_getinputs(GPIOG)) & BOARD_GPIOG_USERBOOT_BIT) == 0 || ((gpioX_getinputs(GPIOE)) & TARGET_ENC2BTN_BIT) == 0)
	#define BOARD_USERBOOT_INITIALIZE() do { \
			arm_hardware_piog_inputs(BOARD_GPIOG_USERBOOT_BIT); /* set as input with pull-up */ \
			arm_hardware_pioe_inputs(TARGET_ENC2BTN_BIT); /* set as input with pull-up */ \
		} while (0)

	/* макроопределение, которое должно включить в себя все инициализации */
	#define	HARDWARE_INITIALIZE() do { \
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

	#define BOARD_BITIMAGE_NAME "rbf/rbfimage_v9c_2ch.h"

	#define USERFIRSTSBLOCK 0	/* виртуальный раздел на SD CARD начинается с этого блока. */

	// TUSB parameters
	#define TUP_DCD_ENDPOINT_MAX    7

#endif /* ARM_STM32MP1_TFBGA361_CPUSTYLE_STORCH_V9C_H_INCLUDED */
