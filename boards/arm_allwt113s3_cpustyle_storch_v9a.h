/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Трансивер с DSP обработкой "Аист" на процессоре Allwinner t113-s3
// rmainunit_sv9f.pcb Allwinner t113-s2, 2xUSB, NAU88C22 и FPGA EP4CE22E22I7N

#ifndef ARM_ALLWT128S3_CPUSTYLE_STORCH_V9A_H_INCLUDED
#define ARM_ALLWT128S3_CPUSTYLE_STORCH_V9A_H_INCLUDED 1

#define WITHSPI16BIT	1	/* возможно использование 16-ти битных слов при обмене по SPI */
#define WITHSPI32BIT	1	/* возможно использование 32-ти битных слов при обмене по SPI */
#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
//#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
//#define WITHSPISW 	1	/* Использование программного управления SPI. */

//#define WIHSPIDFSW	1	/* программное обслуживание DATA FLASH */
//#define WIHSPIDFOVERSPI 1	/* Для работы используется один из обычных каналов SPI */
#define WIHSPIDFHW		1	/* аппаратное обслуживание DATA FLASH */
//#define WIHSPIDFHW2BIT	1	/* аппаратное обслуживание DATA FLASH с подддержкой QSPI подключения по 2-м проводам */
#define WIHSPIDFHW4BIT	1	/* аппаратное обслуживание DATA FLASH с подддержкой QSPI подключения по 4-м проводам */

//#define WITHDMA2DHW		1	/* Использование DMA2D для формирования изображений	- у STM32MP1 его нет */

//#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */

#if WITHINTEGRATEDDSP
	#define WITHI2S1HW	1	/* Использование I2S1 - аудиокодек на I2S */
	#define WITHI2S2HW	1	/* Использование I2S2 - FPGA или IF codec	*/
#endif /* WITHINTEGRATEDDSP */

//#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
//#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */
//#define WITHETHHW 1	/* Hardware Ethernet controller */

#define WITHUART1HW	1	/* PE2 PE3 Используется периферийный контроллер последовательного порта #1 UART0 */
//#define WITHUARTFIFO	1	/* испольование FIFO */

//#define WITHCAT_USART1		1
#define WITHDEBUG_USART1	1
#define WITHNMEA_USART1		1	/* порт подключения GPS/GLONASS */

#if WITHISBOOTLOADER

	#define WITHSDRAMHW	1		/* В процессоре есть внешняя память */
	//#define WITHSDRAM_PMC1	1	/* power management chip */
//
//	//#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
//	//#define WITHGPUHW	1	/* Graphic processor unit */
//	//#define WITHEHCIHW	1	/* USB_EHCI controller */
//	////#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */
//	#define USBPHYC_MISC_SWITHOST_VAL 0		// 0 or 1 - value for USBPHYC_MISC_SWITHOST field. 0: Select OTG controller for 2nd PHY port, 1: Select Host controller for 2nd PHY port
//	#define USBPHYC_MISC_PPCKDIS_VAL 0x00
//
//	////#define WITHUSBHW_DEVICE	USB_OTG_HS	/* на этом устройстве поддерживается функциональность DEVICE	*/
//	#define WITHUSBDEV_VBUSSENSE	1		/* используется предопределенный вывод OTG_VBUS */
//	#define WITHUSBDEV_HSDESC	1			/* Требуется формировать дескрипторы как для HIGH SPEED */
//	//#define WITHUSBDEV_HIGHSPEEDULPI	1
//	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USBH_HS_DP & USBH_HS_DM
//	#define WITHUSBDEV_DMAENABLE 1
//
//	//#define WITHUSBHW_HOST		USB_OTG_HS
//	#define WITHUSBHOST_HIGHSPEEDPHYC	1	// UTMI -> USB_DP2 & USB_DM2
//	#define WITHUSBHOST_DMAENABLE 1
//
//	////#define WITHEHCIHW	1	/* USB_EHCI controller */
//	////#define WITHUSBHW_EHCI		USB1_EHCI
//	#define WITHEHCIHW_EHCIPORT 0	// 0 - use 1st PHY port
//
//
//	#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
//	#define WITHMODEM_CDC	1
//
//	//#define WITHUSBUAC		1	/* использовать виртуальную звуковую плату на USB соединении */
//	//#define WITHUSBUACIN2		1	/* формируются три канала передачи звука */
//	//#define WITHUABUACOUTAUDIO48MONO	1	/* для уменьшения размера буферов в endpoints */
//
//	//#define WITHUSBCDCACM		1	/* ACM использовать виртуальный последовательный порт на USB соединении */
//	//#define WITHUSBCDCACM_N	1	/* количество виртуальных последовательных портов */
//
//	//#define WITHUSBHID	1	/* HID использовать Human Interface Device на USB соединении */
//	#define WITHUSBDFU	1	/* DFU USB Device Firmware Upgrade support */
//	
//	#define WITHUSBWCID	1
//
//	//#define WITHLWIP 1
//	//#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
//	//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */
//	//#define WITHUSBRNDIS	1	/* RNDIS использовать Remote NDIS на USB соединении */

#else /* WITHISBOOTLOADER */

	#define WITHDCDCFREQCTL	1		// Имеется управление частотой преобразователей блока питания и/или подсветки дисплея
	#define WITHCODEC1_I2S1_DUPLEX_SLAVE	1		/* Обмен с аудиокодеком через I2S1 */
	#define WITHFPGAIF_I2S2_DUPLEX_SLAVE	1		/* Обмен с FPGA через I2S2 */
	//#define WITHCODEC1_I2S1_DUPLEX_MASTER	1		/* Обмен с аудиокодеком через I2S1 */
	//#define WITHFPGAIF_I2S2_DUPLEX_MASTER	1		/* Обмен с FPGA через I2S2 */
	//#define WITHCODEC1_WHBLOCK_DUPLEX_MASTER	1	/* встороенный в процессор кодек */

	#define WITHMDMAHW		1	/* Использование G2D для формирования изображений */
	//#define WITHCPUDACHW	1	/* использование встроенного в процессор DAC */
	#define WITHCPUADCHW 	1	/* использование встроенного в процессор ADC */

	#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
	//#define WITHGPUHW	1	/* Graphic processor unit */
	#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */

	#define WITHUSBHW_DEVICE	USBOTG0	/* на этом устройстве поддерживается функциональность DEVICE	*/
	#define WITHUSBDEV_VBUSSENSE	1		/* используется предопределенный вывод OTG_VBUS */
	//#define WITHUSBDEV_HSDESC	1			/* Требуется формировать дескрипторы как для HIGH SPEED */
	//#define WITHUSBDEV_HIGHSPEEDULPI	1	// ULPI
	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USB0_DP & USB0_DM
	#define WITHUSBDEV_DMAENABLE 1

//	#define WITHUSBHW_HOST		USBOTG0
//	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USB0_DP & USB0_DM
//	#define WITHUSBHOST_DMAENABLE 1

	#define WITHEHCIHW	1	/* USB_EHCI controller */

	#define WITHUSBHW_EHCI		USBEHCI1
	#define WITHUSBHW_OHCI		USBOHCI1

	#define WITHUSBHOST_HIGHSPEEDPHYC	1	// UTMI -> USB1_DP & USB1_DM
	#define WITHEHCIHW_EHCIPORT 0	// 0 - use 1st PHY port

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
	#define WITHUSBCDCACM_N	1	/* количество виртуальных последовательных портов */

	#if WITHLWIP
		#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
		//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */
		//#define WITHUSBRNDIS	1	/* RNDIS использовать Remote NDIS на USB соединении */
	#endif /* WITHLWIP */
	//#define WITHUSBHID	1	/* HID использовать Human Interface Device на USB соединении */

	#define WITHUSBDFU	1	/* DFU USB Device Firmware Upgrade support */
	
	#define WITHUSBWCID	1
	//#define WITHUSBDMTP	1	/* MTP USB Device */
	//#define WITHUSBDMSC	1	/* MSC USB device */

#endif /* WITHISBOOTLOADER */

#define LS020_RS_INITIALIZE() do { \
		arm_hardware_piod_outputs2m(LS020_RS, LS020_RS); /* PD0 */ \
	} while (0)

#define LS020_RESET_INITIALIZE() do { \
		arm_hardware_piod_outputs2m(LS020_RESET, LS020_RESET); /* PD22 */ \
	} while (0)

#define LS020_RS_SET(v) 		do { gpioX_setstate(GPIOD, LS020_RS, !! (v) * LS020_RS); } while (0)
#define LS020_RS_PORT_S(v)		do { gpioX_setstate(GPIOD, (v), !! (1) * (v)); } while (0)
#define LS020_RS_PORT_C(v)		do { gpioX_setstate(GPIOD, (v), !! (0) * (v)); } while (0)
#define LS020_RS				(1u << 3)			// PD3 signal

#define LS020_RESET_SET(v) 		do { gpioX_setstate(GPIOD, LS020_RESET, !! (v) * LS020_RESET); } while (0)
#define LS020_RESET_PORT_S(v)		do { gpioX_setstate(GPIOD, (v), !! (1) * (v)); } while (0)
#define LS020_RESET_PORT_C(v)		do { gpioX_setstate(GPIOD, (v), !! (0) * (v)); } while (0)
#define LS020_RESET				(1u << 22)			// PD22 signal

#if WITHENCODER

	// Выводы подключения енкодера #1
	#define ENCODER_INPUT_PORT	(GPIOE->DATA)
	#define ENCODER_BITA		(1u << 8)		// PE8
	#define ENCODER_BITB		(1u << 7)		// PE7

	// Выводы подключения енкодера #2
	#define ENCODER2_INPUT_PORT	(GPIOE->DATA)
	#define ENCODER2_BITA		(1u << 5)		// PE5
	#define ENCODER2_BITB		(1u << 4)		// PE4


	#define ENCODER_BITS		(ENCODER_BITA | ENCODER_BITB)
	#define ENCODER2_BITS		(ENCODER2_BITA | ENCODER2_BITB)

	#define ENCODER_INITIALIZE() do { \
		arm_hardware_pioe_altfn20(ENCODER_BITS, GPIO_CFG_AF14); \
		arm_hardware_pioe_updown(ENCODER_BITS, 0); \
		gpioX_onchangeinterrupt(GPIOE, ENCODER_BITS, ENCODER_BITS, ENCODER_BITS, ARM_OVERREALTIME_PRIORITY, TARGETCPU_OVRT, spool_encinterrupt); \
		arm_hardware_pioe_altfn20(ENCODER2_BITS, GPIO_CFG_AF14); \
		arm_hardware_pioe_updown(ENCODER2_BITS, 0); \
		gpioX_onchangeinterrupt(GPIOE, 0 * ENCODER2_BITS, ENCODER2_BITS, ENCODER2_BITS, ARM_OVERREALTIME_PRIORITY, TARGETCPU_OVRT, spool_encinterrupt2); \
	} while (0)

#endif

	// Инициализируются I2S1 в дуплексном режиме.
	// аудиокодек
	#define I2S1HW_INITIALIZE(master) do { \
		arm_hardware_piog_altfn20(1u << 12, GPIO_CFG_AF2); /* PG12 I2S1-LRCK */ \
		arm_hardware_piog_altfn20(1u << 13, GPIO_CFG_AF2); /* PG13 I2S1-BCLK */ \
		arm_hardware_piog_altfn20(1u << 14, GPIO_CFG_AF2); /* PG14 I2S1-DIN0 from codec */ \
		arm_hardware_piog_altfn20(1u << 15, GPIO_CFG_AF2); /* PG15 I2S1-DOUT0 co codec */ \
	} while (0)
	#define HARDWARE_I2S1HW_DIN 0	/* DIN0 used */
	#define HARDWARE_I2S1HW_DOUT 0	/* DOUT0 used */
	// Инициализируются I2S2 в дуплексном режиме.
	// FPGA или IF codec
	#define I2S2HW_INITIALIZE(master) do { \
		arm_hardware_piob_altfn20(1u << 6,	GPIO_CFG_AF3); /* PB6 I2S2-LRCK	*/ \
		arm_hardware_piob_altfn20(1u << 5,	GPIO_CFG_AF3); /* PB5 I2S2-BCLK	*/ \
		arm_hardware_piob_altfn20(1u << 4,	GPIO_CFG_AF3); /* PB4 I2S2-DOUT0 to FPGA */ \
		arm_hardware_piob_altfn20(1u << 3,	GPIO_CFG_AF5); /* PB3 I2S2-DIN0 from FPGA */ \
	} while (0)
	#define HARDWARE_I2S2HW_DIN 0	/* DIN0 used */
	#define HARDWARE_I2S2HW_DOUT 0	/* DOUT0 used */

/* Распределение битов в ARM контроллерах */

#if (WITHCAT && WITHCAT_USART2)
	// CAT data lites
	// RXD at PA10, TXD at PA9

	// CAT control lines
	//#define FROMCAT_TARGET_PIN_RTS		(GPIOA->DATA)
	//#define FROMCAT_BIT_RTS				(1u << 11)	/* PA11 сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	//#define FROMCAT_TARGET_PIN_DTR		(GPIOA->DATA)
	//#define FROMCAT_BIT_DTR				(1u << 12)	/* PA12 сигнал DTR от FT232RL	*/

	/* манипуляция от порта RS-232 */
	#define FROMCAT_DTR_INITIALIZE() do { \
		} while (0)

	/* переход на передачу от порта RS-232 */
	#define FROMCAT_RTS_INITIALIZE() do { \
		} while (0)

#endif /* (WITHCAT && WITHCAT_USART2) */

#if (WITHCAT && WITHCAT_CDC)

	// CAT data lites
	// RXD at PA10, TXD at PA9

	// CAT control lines
	//#define FROMCAT_TARGET_PIN_RTS		(GPIOA->DATA) // was PINA
	//#define FROMCAT_BIT_RTS				(1u << 11)	/* сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	//#define FROMCAT_TARGET_PIN_DTR		(GPIOA->DATA) // was PINA
	//#define FROMCAT_BIT_DTR				(1u << 12)	/* сигнал DTR от FT232RL	*/

	/* манипуляция от виртуального CDC порта */
	#define FROMCAT_DTR_INITIALIZE() do { \
		} while (0)

	/* переход на передачу от виртуального CDC порта*/
	#define FROMCAT_RTS_INITIALIZE() do { \
		} while (0)

#endif /* (WITHCAT && WITHCAT_CDC) */

#if WITHSDHCHW
	#if WITHSDHCHW4BIT
		#define HARDWARE_SDIO_INITIALIZE()	do { \
			arm_hardware_piod_altfn50(1u << 2, AF_SDIO);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_altfn50(1u << 12, AF_SDIO);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_altfn50(1u << 8, AF_SDIO);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_altfn50(1u << 9, AF_SDIO);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_altfn50(1u << 10, AF_SDIO);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_altfn50(1u << 11, AF_SDIO);	/* PC11 - SDIO_D3	*/ \
		} while (0)
		/* отключить процессор от SD карты - чтобы при выполнении power cycle не возникало фантомное питание через сигналы управления. */
		#define HARDWARE_SDIO_HANGOFF()	do { \
			arm_hardware_piod_inputs(1u << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_inputs(1u << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_inputs(1u << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_inputs(1u << 9);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_inputs(1u << 10);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_inputs(1u << 11);	/* PC11 - SDIO_D3	*/ \
			arm_hardware_piod_updown(0, 1u << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(0, 1u << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(0, 1u << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_updown(0, 1u << 9);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_updown(0, 1u << 10);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_updown(0, 1u << 11);	/* PC11 - SDIO_D3	*/ \
		} while (0)
	#else /* WITHSDHCHW4BIT */
		#define HARDWARE_SDIO_INITIALIZE()	do { \
			arm_hardware_piod_altfn50(1u << 2, AF_SDIO);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_altfn50(1u << 12, AF_SDIO);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_altfn50(1u << 8, AF_SDIO);	/* PC8 - SDIO_D0	*/ \
		} while (0)
		/* отключить процессор от SD карты - чтобы при выполнении power cycle не возникало фантомное питание через сигналы управления. */
		#define HARDWARE_SDIO_HANGOFF()	do { \
			arm_hardware_piod_inputs(1u << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_inputs(1u << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_inputs(1u << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_piod_updown(0, 1u << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(0, 1u << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(0, 1u << 8);	/* PC8 - SDIO_D0	*/ \
		} while (0)
	#endif /* WITHSDHCHW4BIT */

	#define HARDWARE_SDIO_WP_BIT	(1U << 8)	/* PG8 - SDIO_WP */
	#define HARDWARE_SDIO_CD_BIT	(1U << 7)	/* PG7 - SDIO_SENSE */

	#define HARDWARE_SDIOSENSE_INITIALIZE()	do { \
			arm_hardware_piog_inputs(HARDWARE_SDIO_WP_BIT); /* PD1 - SDIO_WP */ \
			arm_hardware_piog_updown(HARDWARE_SDIO_WP_BIT, 0); \
			arm_hardware_piog_inputs(HARDWARE_SDIO_CD_BIT); /* PD0 - SDIO_SENSE */ \
			arm_hardware_piog_updown(HARDWARE_SDIO_CD_BIT, 0); \
	} while (0)

	#define HARDWARE_SDIOSENSE_CD() ((GPIOG->DATA & HARDWARE_SDIO_CD_BIT) == 0)	/* получить состояние датчика CARD PRESENT */
	#define HARDWARE_SDIOSENSE_WP() ((GPIOG->DATA & HARDWARE_SDIO_WP_BIT) != 0)	/* получить состояние датчика CARD WRITE PROTECT */

	#define HARDWARE_SDIOPOWER_C(v)	do { GPIOC->BSRR = BSRR_C(v); (void) GPIOC->BSRR; } while (0)
	#define HARDWARE_SDIOPOWER_S(v)	do { GPIOC->BSRR = BSRR_S(v); (void) GPIOC->BSRR; } while (0)
	#define HARDWARE_SDIOPOWER_BIT (1u << 7)	/* PC7 */
	/* если питание SD CARD управляется прямо с процессора */
	#define HARDWARE_SDIOPOWER_INITIALIZE()	do { \
		arm_hardware_pioc_outputs2m(HARDWARE_SDIOPOWER_BIT, HARDWARE_SDIOPOWER_BIT); /* питание выключено */ \
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

	#define TXPATH_INITIALIZE() do { \
		} while (0)


	// +++
	// TXDISABLE input - PF5
	#define TXDISABLE_TARGET_PIN				(GPIOF->DATA)
	#define TXDISABLE_BIT_TXDISABLE				(1U << 5)		// PF5 - TX INHIBIT
	// получить бит запрета передачи (от усилителя мощности)
	#define HARDWARE_GET_TXDISABLE() ((TXDISABLE_TARGET_PIN & TXDISABLE_BIT_TXDISABLE) != 0)
	#define TXDISABLE_INITIALIZE() do { \
			arm_hardware_piof_inputs(TXDISABLE_BIT_TXDISABLE); \
			arm_hardware_piof_updown(0, TXDISABLE_BIT_TXDISABLE); \
		} while (0)
	// ---

	// +++
	// PTT input - PF3
	// PTT2 input - PF4
	#define PTT_TARGET_PIN				(GPIOF->DATA)
	#define PTT_BIT_PTT					(1u << 3)		// PF3 - PTT
	#define PTT2_TARGET_PIN				(GPIOF->DATA)
	#define PTT2_BIT_PTT				(1u << 4)		// PF4 - PTT2
	// получить бит запроса оператором перехода на пердачу
	#define HARDWARE_GET_PTT() 			((PTT_TARGET_PIN & PTT_BIT_PTT) == 0 || (PTT2_TARGET_PIN & PTT2_BIT_PTT) == 0)
	#define PTT_INITIALIZE() do { \
			arm_hardware_piof_inputs(PTT_BIT_PTT); \
			arm_hardware_piof_updown(PTT_BIT_PTT, 0); \
			arm_hardware_piof_inputs(PTT2_BIT_PTT); \
			arm_hardware_piof_updown(PTT2_BIT_PTT, 0); \
		} while (0)
	// ---
	// TUNE input - PF2
	#define TUNE_TARGET_PIN				(GPIOF->DATA)
	#define TUNE_BIT_TUNE				(1U << 2)		// PF2
	#define HARDWARE_GET_TUNE() 		((TUNE_TARGET_PIN & TUNE_BIT_TUNE) == 0)
	#define TUNE_INITIALIZE() do { \
			arm_hardware_piof_inputs(TUNE_BIT_TUNE); \
			arm_hardware_piof_updown(TUNE_BIT_TUNE, 0); \
		} while (0)

#else /* WITHTX */

	#define TXDISABLE_INITIALIZE() do { \
		} while (0)
	#define PTT_INITIALIZE() do { \
		} while (0)
	#define TUNE_INITIALIZE() do { \
		} while (0)
#endif /* WITHTX */

#if WITHELKEY
	// Electronic key inputs
	#define ELKEY_BIT_LEFT				(1u << 0)		// PF0
	#define ELKEY_BIT_RIGHT				(1u << 1)		// PF1

	#define ELKEY_TARGET_PIN			(GPIOF->DATA)

	#define HARDWARE_GET_ELKEY_LEFT() 	((ELKEY_TARGET_PIN & ELKEY_BIT_LEFT) == 0)
	#define HARDWARE_GET_ELKEY_RIGHT() 	((ELKEY_TARGET_PIN & ELKEY_BIT_RIGHT) == 0)


	#define ELKEY_INITIALIZE() do { \
			arm_hardware_piof_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
			arm_hardware_piof_updown(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, 0); \
		} while (0)

#endif /* WITHELKEY */

// IOUPDATE = PA15
//#define SPI_IOUPDATE_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); (void) GPIOA->BSRR; } while (0)
//#define SPI_IOUPDATE_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); (void) GPIOA->BSRR; } while (0)
//#define SPI_IOUPDATE_BIT		(1u << 15)	// * PA15

#if WITHSPIHW || WITHSPISW
	// Набор определений для работы без внешнего дешифратора

	#define targetdataflash 0xFF
	#define targetnone 0x00

	#define targetext1		(1u << 0)		// PG0 ext1 on front panel CSEXT1
	#define targetnvram		(1u << 7)		// PG7 nvram FM25L16B
	#define targetctl1		(1u << 11)		// PG11 board control registers chain
	#define targetcodec1	(1u << 6)		// PG6 on-board codec1 NAU8822L
	#define targetfpga1		(1u << 2)		// PG2 FPGA control registers CS1
	#define targetrtc1		(1u << 10)		// PG10 RTC DS1305 RTC_CS

	#define targetadc2		(1u << 8)	// PG8 on-board ADC MCP3208-BI/SL chip select (potentiometers) ADCCS1
	#define targetadck		(1u << 9)	// PG9 on-board ADC MCP3208-BI/SL chip select (KEYBOARD) ADCCS2
	#define targetxad2		(1u << 3)	// PG3 external SPI device (PA BOARD ADC) CSEXT2

	#define targetlcd	targetext1 	/* LCD over SPI line devices control */ 
	#define targetuc1608 targetext1	/* LCD with positive chip select signal	*/
	#define targettsc1 		targetext1	/* XPT2046 SPI chip select signal */

	/* Select specified chip. */
	#define SPI_CS_ASSERT(target)	do { \
		switch (target) { \
		case targetdataflash: { gpioX_setstate(GPIOC, SPDIF_NCS_BIT, 0 * (SPDIF_NCS_BIT)); } break; /* PC3 SPI0_CS */ \
		case targetrtc1: { gpioX_setstate(GPIOG, (target), 1 * (target)); } break; \
		default: { gpioX_setstate(GPIOG, (target), 0 * (target)); } break; \
		case targetnone: break; \
		} \
	} while (0)

	/* Unelect specified chip. */
	#define SPI_CS_DEASSERT(target)	do { \
		switch (target) { \
		case targetdataflash: { gpioX_setstate(GPIOC, SPDIF_NCS_BIT, 1 * (SPDIF_NCS_BIT)); } break; /* PC3 SPI0_CS */ \
		case targetrtc1: { gpioX_setstate(GPIOG, (target), 0 * (target)); } break; \
		default: { gpioX_setstate(GPIOG, (target), 1 * (target)); } break; \
		case targetnone: break; \
		} \
	} while (0)

	/* инициализация линий выбора периферийных микросхем */
	#define SPI_ALLCS_INITIALIZE() do { \
		arm_hardware_pioc_outputs(SPDIF_NCS_BIT, 1 * SPDIF_NCS_BIT); 	/* PC3 SPI0_CS */ \
		arm_hardware_piog_outputs(targetext1, 1 * targetext1); /*  */ \
		arm_hardware_piog_outputs(targetnvram, 1 * targetnvram); /*  */ \
		arm_hardware_piog_outputs(targetctl1, 1 * targetctl1); /*  */ \
		arm_hardware_piog_outputs(targetcodec1, 1 * targetcodec1); /*  */ \
		arm_hardware_piog_outputs(targetfpga1, 1 * targetfpga1); /*  */ \
		arm_hardware_piog_outputs(targetrtc1, 0 * targetrtc1); /*  */ \
		arm_hardware_piog_outputs(targetadc2, 1 * targetadc2); /*  */ \
		arm_hardware_piog_outputs(targetadck, 1 * targetadck); /*  */ \
		arm_hardware_piog_outputs(targetxad2, 1 * targetxad2); /*  */ \
	} while (0)

	// MOSI & SCK port
	#define	SPI_SCLK_BIT			(1u << 2)	// PC2 SPI0_CLK
	#define	SPI_MOSI_BIT			(1u << 4)	// PC4 SPI0_MOSI
	#define	SPI_MISO_BIT			(1u << 5)	// PC5 SPI0_MISO

	/* Выводы соединения с QSPI BOOT NOR FLASH */
	//#define SPDIF_SCLK_BIT (1u << 2)	// PC2 SPI0_CLK
	#define SPDIF_NCS_BIT (1u << 3)	// PC3 SPI0_CS
	//#define SPDIF_MOSI_BIT (1u << 4)	// PC4 SPI0_MOSI
	//#define SPDIF_MISO_BIT (1u << 5)	// PC5 SPI0_MISO
	#define SPDIF_D2_BIT (1u << 6)		// PC6 SPI0_WP/D2
	#define SPDIF_D3_BIT (1u << 7)		// PC7 SPI0_HOLD/D3

//	#define SPI_TARGET_SCLK_PORT_C(v)	do { gpioX_setstate(GPIOC, (v), !! (0) * (v)); } while (0)
//	#define SPI_TARGET_SCLK_PORT_S(v)	do { gpioX_setstate(GPIOC, (v), !! (1) * (v)); } while (0)
//
//	#define SPI_TARGET_MOSI_PORT_C(v)	do { gpioX_setstate(GPIOC, (v), !! (0) * (v)); } while (0)
//	#define SPI_TARGET_MOSI_PORT_S(v)	do { gpioX_setstate(GPIOC, (v), !! (1) * (v)); } while (0)
//
//	#define SPI_TARGET_MISO_PIN		(GPIOC->DATA)

	#define SPIIO_INITIALIZE() do { \
		arm_hardware_pioc_altfn2(SPI_SCLK_BIT, GPIO_CFG_AF2); 	/* PC2 SPI0_CLK */ \
		arm_hardware_pioc_altfn2(SPI_MOSI_BIT, GPIO_CFG_AF2); 	/* PC4 SPI0_MOSI */ \
		arm_hardware_pioc_altfn2(SPI_MISO_BIT, GPIO_CFG_AF2); 	/* PC5 SPI0_MISO */ \
		arm_hardware_pioc_altfn2(SPDIF_D2_BIT, GPIO_CFG_AF2);  /* PC6 SPI0_WP/D2 */ \
		arm_hardware_pioc_altfn2(SPDIF_D3_BIT, GPIO_CFG_AF2);  /* PC7 SPI0_HOLD/D3 */ \
	} while (0)
	#define HARDWARE_SPI_CONNECT() do { \
	} while (0)
	#define HARDWARE_SPI_DISCONNECT() do { \
	} while (0)
	#define HARDWARE_SPI_CONNECT_MOSI() do { \
	} while (0)
	#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
	} while (0)

#else /* WITHSPIHW || WITHSPISW */

	#define targetext1		(0)		// PE8 ext1 on front panel
	#define targetxad2		(0)		// PE7 ext2 двунаправленный SPI для подключения внешних устройств - например тюнера
	#define targetnvram		(0)		// PE0 nvmem FM25L16B
	#define targetctl1		(0)		// PE1 board control registers chain
	#define targetcodec1	(0)		// PE2 on-board codec1 NAU8822L
	#define targetadc2		(0) 		// PE9 ADC MCP3208-BI/SL chip select (potentiometers)
	#define targetfpga1		(0)		// PE10 FPGA control registers CS1

#endif /* WITHSPIHW || WITHSPISW */

// WITHUART1HW
#define HARDWARE_UART1_INITIALIZE() do { \
		const portholder_t TXMASK = (1u << 2); /* PE2 UART0-TX */ \
		const portholder_t RXMASK = (1u << 3); /* PE3 UART0-RX - pull-up RX data */  \
		arm_hardware_pioe_altfn2(TXMASK, GPIO_CFG_AF6); \
		arm_hardware_pioe_altfn2(RXMASK, GPIO_CFG_AF6); \
		arm_hardware_pioe_updown(RXMASK, 0); \
	} while (0)



#if WITHKEYBOARD
	/* PE15: pull-up second encoder button */

	#define TARGET_ENC2BTN_BIT (1U << 6)	// PE6 - second encoder button with pull-up
	#define TARGET_POWERBTN_BIT 0//(1U << 8)	// PAxx - ~CPU_POWER_SW signal

#if WITHENCODER2
	// P7_8
	#define TARGET_ENC2BTN_GET	(((GPIOE->DATA) & TARGET_ENC2BTN_BIT) == 0)
#endif /* WITHENCODER2 */

#if WITHPWBUTTON
	// P5_3 - ~CPU_POWER_SW signal
	#define TARGET_POWERBTN_GET	0//(((GPIOx->DATA) & TARGET_POWERBTN_BIT) == 0)
#endif /* WITHPWBUTTON */

	#define HARDWARE_KBD_INITIALIZE() do { \
			arm_hardware_pioe_inputs(TARGET_ENC2BTN_BIT); \
			arm_hardware_pioe_updown(TARGET_ENC2BTN_BIT, 0); /* PE6: pull-up second encoder button */ \
			arm_hardware_pioa_inputs(TARGET_POWERBTN_BIT); \
			arm_hardware_pioa_updown(TARGET_POWERBTN_BIT, 0);	/* PAxx: pull-up second encoder button */ \
		} while (0)

#else /* WITHKEYBOARD */

	#define HARDWARE_KBD_INITIALIZE() do { \
		} while (0)

#endif /* WITHKEYBOARD */

#if WITHTWISW
	// TWI1-SCK PE0
	// TWI1-SDA PE1
	#define TARGET_TWI_TWCK		(1u << 0)		// TWI1-SCK PE0
	#define TARGET_TWI_TWCK_PIN		(GPIOE->DATA)
	#define TARGET_TWI_TWCK_PORT_C(v) do { arm_hardware_pioe_outputs((v), 0); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do { arm_hardware_pioe_inputs(v); } while (0)

	#define TARGET_TWI_TWD		(1u << 1)		// TWI1-SDA PE1
	#define TARGET_TWI_TWD_PIN		(GPIOE->DATA)
	#define TARGET_TWI_TWD_PORT_C(v) do { arm_hardware_pioe_outputs((v), 0); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v) do { arm_hardware_pioe_inputs(v); } while (0)

	// Инициализация битов портов ввода-вывода для программной реализации I2C
	#define	TWISOFT_INITIALIZE() do { \
			arm_hardware_pioe_inputs(TARGET_TWI_TWCK); /* SCL */ \
			arm_hardware_pioe_inputs(TARGET_TWI_TWD);  	/* SDA */ \
		} while (0) 
	#define	TWISOFT_DEINITIALIZE() do { \
			arm_hardware_pioe_inputs(TARGET_TWI_TWCK); 	/* SCL */ \
			arm_hardware_pioe_inputs(TARGET_TWI_TWD);	/* SDA */ \
		} while (0)
	// Инициализация битов портов ввода-вывода для аппаратной реализации I2C
	// присоединение выводов к периферийному устройству
	#define	TWIHARD_INITIALIZE() do { \
		arm_hardware_pioe_altfn2(TARGET_TWI_TWCK, GPIO_CFG_AF4);	/* TWI1-SCK PE0 */ \
		arm_hardware_pioe_altfn2(TARGET_TWI_TWD, GPIO_CFG_AF4);		/* TWI1-SDA PE1 */ \
		} while (0) 


#endif // WITHTWISW

#if WITHFPGAWAIT_AS || WITHFPGALOAD_PS

	/* outputs */
	#define FPGA_NCONFIG_PORT_S(v)	do { gpioX_setstate(GPIOE, (v), !! (1) * (v)); } while (0)
	#define FPGA_NCONFIG_PORT_C(v)	do { gpioX_setstate(GPIOE, (v), !! (0) * (v)); } while (0)
	#define FPGA_NCONFIG_BIT		(1u << 12)	/* PE12 bit conneced to nCONFIG pin ALTERA FPGA */

	/* inputs */
	#define FPGA_CONF_DONE_INPUT	(GPIOE->DATA)
	#define FPGA_CONF_DONE_BIT		(1u << 13)	/* PE13 bit conneced to CONF_DONE pin ALTERA FPGA */

	#define FPGA_NSTATUS_INPUT		(GPIOB->DATA)
	#define FPGA_NSTATUS_BIT		(1u << 2)	/* PB2 bit conneced to NSTATUS pin ALTERA FPGA */

	#define FPGA_INIT_DONE_INPUT	(GPIOE->DATA)
	#define FPGA_INIT_DONE_BIT		(1u << 10)	/* PE10 bit conneced to INIT_DONE pin ALTERA FPGA */

	/* Инициадизация выводов GPIO процессора для получения состояния и управлением загрузкой FPGA */
	#define HARDWARE_FPGA_LOADER_INITIALIZE() do { \
		arm_hardware_pioe_outputs(FPGA_NCONFIG_BIT, FPGA_NCONFIG_BIT); \
		arm_hardware_piob_inputs(FPGA_NSTATUS_BIT); \
		arm_hardware_pioe_inputs(FPGA_CONF_DONE_BIT); \
		arm_hardware_pioe_inputs(FPGA_INIT_DONE_BIT); \
	} while (0)

	/* необходимость функции под вопросом (некоторый FPGA не нрузятся с этой процедурой) */
	#define HARDWARE_FPGA_RESET() do { \
		/* board_fpga_reset(); */ \
	} while (0)

	/* Проверяем, проинициализировалась ли FPGA (вошла в user mode). */
	/*
		After the option bit to enable INIT_DONE is programmed into the device (during the first
		frame of configuration data), the INIT_DONE pin goes low.
		When initialization is complete, the INIT_DONE pin is released and pulled high. 
		This low-to-high transition signals that the device has entered user mode.
	*/
	#define HARDWARE_FPGA_IS_USER_MODE() (local_delay_ms(100), (FPGA_INIT_DONE_INPUT & FPGA_INIT_DONE_BIT) != 0)

#else /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS */

	/* необходимость функции под вопросом (некоторый FPGA не нрузятся с этой процедурой) */
	#define HARDWARE_FPGA_RESET() do { \
		/* board_fpga_reset(); */ \
	} while (0)

#endif /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS */

#if WITHDSPEXTFIR
	// Биты доступа к массиву коэффициентов FIR фильтра в FPGA

	// FPGA PIN_23
	#define TARGET_FPGA_FIR_CS_PORT_C(v)	do { gpioX_setstate(GPIOD, (v), !! (0) * (v)); } while (0)
	#define TARGET_FPGA_FIR_CS_PORT_S(v)	do { gpioX_setstate(GPIOD, (v), !! (1) * (v)); } while (0)
	#define TARGET_FPGA_FIR_CS_BIT (1u << 12)	/* PD12 - fir CS ~FPGA_FIR_CLK */
	// FPGA PIN_8
	#define TARGET_FPGA_FIR1_WE_PORT_C(v)	do { gpioX_setstate(GPIOG, (v), !! (0) * (v)); } while (0)
	#define TARGET_FPGA_FIR1_WE_PORT_S(v)	do { gpioX_setstate(GPIOG, (v), !! (1) * (v)); } while (0)
	#define TARGET_FPGA_FIR1_WE_BIT (1u << 4)	/* PG4 - fir1 WE */

	// FPGA PIN_7
	#define TARGET_FPGA_FIR2_WE_PORT_C(v)	do { gpioX_setstate(GPIOG, (v), !! (0) * (v)); } while (0)
	#define TARGET_FPGA_FIR2_WE_PORT_S(v)	do { gpioX_setstate(GPIOG, (v), !! (1) * (v)); } while (0)
	#define TARGET_FPGA_FIR2_WE_BIT (1u << 5)	/* PG5 - fir2 WE */

	#define TARGET_FPGA_FIR_CS_SET() do { TARGET_FPGA_FIR_CS_PORT_S(TARGET_FPGA_FIR_CS_BIT); } while (0)
	#define TARGET_FPGA_FIR_CS_CLR() do { TARGET_FPGA_FIR_CS_PORT_C(TARGET_FPGA_FIR_CS_BIT); } while (0)
	#define TARGET_FPGA_FIR1_WE_SET() do { TARGET_FPGA_FIR1_WE_PORT_S(TARGET_FPGA_FIR1_WE_BIT); } while (0)
	#define TARGET_FPGA_FIR1_WE_CLR() do { TARGET_FPGA_FIR1_WE_PORT_C(TARGET_FPGA_FIR1_WE_BIT); } while (0)
	#define TARGET_FPGA_FIR2_WE_SET() do { TARGET_FPGA_FIR2_WE_PORT_S(TARGET_FPGA_FIR1_WE_BIT); } while (0)
	#define TARGET_FPGA_FIR2_WE_CLR() do { TARGET_FPGA_FIR2_WE_PORT_C(TARGET_FPGA_FIR1_WE_BIT); } while (0)

	#define TARGET_FPGA_FIR_INITIALIZE() do { \
			arm_hardware_piog_outputs2m(TARGET_FPGA_FIR1_WE_BIT, TARGET_FPGA_FIR1_WE_BIT); \
			arm_hardware_piog_outputs2m(TARGET_FPGA_FIR2_WE_BIT, TARGET_FPGA_FIR2_WE_BIT); \
			arm_hardware_piod_outputs2m(TARGET_FPGA_FIR_CS_BIT, TARGET_FPGA_FIR_CS_BIT); \
		} while (0)
#endif /* WITHDSPEXTFIR */

#if 1
	/* получение состояния переполнения АЦП */
	#define TARGET_FPGA_OVF_INPUT		(GPIOB->DATA)
	#define TARGET_FPGA_OVF_BIT			(1u << 7)	// PB7
	#define TARGET_FPGA_OVF_GET			((TARGET_FPGA_OVF_INPUT & TARGET_FPGA_OVF_BIT) == 0)	// 1 - overflow active
	#define TARGET_FPGA_OVF_INITIALIZE() do { \
			arm_hardware_piob_inputs(TARGET_FPGA_OVF_BIT); \
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
		} while (0)
#endif /* WITHCPUADCHW */

#if WITHUSBHW || 1
	// PE11
	#define TARGET_USBFS_VBUSON_PORT_C(v)	do { gpioX_setstate(GPIOE, (v), !! (0) * (v)); } while (0) //do { GPIOD->BSRR = BSRR_C(v); (void) GPIOD->BSRR; } while (0)
	#define TARGET_USBFS_VBUSON_PORT_S(v)	do { gpioX_setstate(GPIOE, (v), !! (0) * (v)); } while (0) //do { GPIOD->BSRR = BSRR_S(v); (void) GPIOD->BSRR; } while (0)
	#define TARGET_USBFS_VBUSON_BIT (1u << 11)	// PE11 - нулём включение питания для device
	/**USB_OTG_FS GPIO Configuration    
	PA9     ------> USB_OTG_FS_VBUS
	PA10     ------> USB_OTG_FS_ID
	PA11     ------> USB_OTG_FS_DM
	PA12     ------> USB_OTG_FS_DP 
	*/

	#define	USBD_EHCI_INITIALIZE() do { \
			arm_hardware_pioe_outputs(TARGET_USBFS_VBUSON_BIT, 1 * TARGET_USBFS_VBUSON_BIT); /* PE11 */ \
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
		/*arm_hardware_pioa_altfn50((1u << 11) | (1u << 12), AF_OTGFS);	*/		/* PA10, PA11, PA12 - USB_OTG_FS	*/ \
		/* arm_hardware_pioa_inputs(1u << 9);	*/	/* PA9 - USB_OTG_FS_VBUS */ \
		} while (0)

	#define	USBD_HS_ULPI_INITIALIZE() do { \
		} while (0)
#endif /* WITHUSBHW */

#if WITHDCDCFREQCTL
	// ST ST1S10 Synchronizable switching frequency from 400 kHz up to 1.2 MHz
	#define WITHHWDCDCFREQMIN 400000L
	#define WITHHWDCDCFREQMAX 1200000L
	#define HARDWARE_DCDC_PWMCH 5	/* PWM5 */

	// PF6 - DC-DC synchro output
	// PWM5 AF6
	#define	HARDWARE_DCDC_INITIALIZE() do { \
		hardware_dcdcfreq_pwm5_initialize(HARDWARE_DCDC_PWMCH); \
		arm_hardware_piof_altfn2((1U << 6), GPIO_CFG_AF6); /* PF6 - PWM5 */ \
	} while (0)
	#define HARDWARE_DCDC_SETDIV(f) do { \
		hardware_dcdcfreq_pwm5_setdiv(HARDWARE_DCDC_PWMCH, f); \
	} while (0)
#else /* WITHDCDCFREQCTL */
	#define	HARDWARE_DCDC_INITIALIZE() do { \
		arm_hardware_piof_outputs((1U << 6), 0 * (1U << 6)); /* PF6 */ \
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
		#define WITHLCDBACKLIGHTMIN	0	// Нижний предел регулировки (показываемый на дисплее)
		#define WITHLCDBACKLIGHTMAX	3	// Верхний предел регулировки (показываемый на дисплее)
		//#define WITHKBDBACKLIGHT	1	// Имеется управление подсветкой клавиатуры
	#else
		/* Заглушка для работы без дисплея */
		#define WITHLCDBACKLIGHTMIN	0
		#define WITHLCDBACKLIGHTMAX	2	// Верхний предел регулировки (показываемый на дисплее)
	#endif

	#if 0
		/* BL0: PA14. BL1: PA15 */
		#define	HARDWARE_BL_INITIALIZE() do { \
			const portholder_t BLpins = (1U << 15) | (1U << 14); /* PA15:PA14 */ \
			const portholder_t ENmask = 0 * (1U << 1); /* PF1 - not in this hardware  */ \
			arm_hardware_pioa_opendrain(BLpins, 0); \
			} while (0)

		/* установка яркости и включение/выключение преобразователя подсветки */
		/* BL0: PA14. BL1: PA15 */
		#define HARDWARE_BL_SET(en, level) do { \
			const portholder_t Vlevel = (level) & 0x03; \
			const portholder_t ENmask = 0 * (1U << 1); /* PF1 - not in this hardware */ \
			const portholder_t BLpins = (1U << 15) | (1U << 14); /* PA15:PA14 */ \
			const portholder_t BLstate = (~ Vlevel) << 14; \
			GPIOA->BSRR = \
				BSRR_S((BLstate) & (BLpins)) | /* set bits */ \
				BSRR_C(~ (BLstate) & (BLpins)) | /* reset bits */ \
				0; \
			__DSB(); \
		} while (0)
	#endif

#if WITHLTDCHW

	// Sochip_VE_S3_Datasheet_V1.0.pdf
	// TCON0_TRM_CTL_REG offset 0x0010
	// User manual:
	// LCD FRM Control Register (Default Value: 0x0000_0000)
	#define TCON_FRM_MODE_VAL ((1u << 6) | (0u << 5) | (1u << 4))	// 16 bit panel connected
	//#define TCON_FRM_MODE_VAL ((0u << 6) | (0u << 5)| (0u << 4))	// 18 bit panel connected

	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_INITIALIZE(demode) do { \
		const portholder_t VSmask = (1U << 21); 	/* PD21 LCD_VSYNC */ \
		const portholder_t HSmask = (1U << 20); 	/* PD20 LCD_HSYNC */ \
		const portholder_t DEmask = (1U << 19); 	/* PD19 LCD_DE */ \
		const portholder_t MODEmask = (1U << 0); 	/* PD0 mode */ \
		/* set LCD DE/SYNC mode */ \
		arm_hardware_piod_outputs(MODEmask, (demode != 0) * MODEmask);	/* PD0 = state */ \
		/* synchro signals - sync mode */ \
		arm_hardware_piod_outputs((demode == 0) * DEmask, 0 * DEmask); /* PD19 LCD_DE */ \
		arm_hardware_piod_altfn20((demode == 0) * VSmask, GPIO_CFG_AF2); /* PD21 LCD_VSYNC */ \
		arm_hardware_piod_altfn20((demode == 0) * HSmask, GPIO_CFG_AF2); /* PD20 LCD_HSYNC */ \
		/* synchro signals - DE mode */ \
		arm_hardware_piod_altfn20((demode != 0) * DEmask, GPIO_CFG_AF2); /* PD19 LCD_DE */ \
		arm_hardware_piod_outputs((demode != 0) * VSmask, 1 * VSmask); /* PD21 LCD_VSYNC */ \
		arm_hardware_piod_outputs((demode != 0) * HSmask, 1 * HSmask); /* PD20 LCD_HSYNC */ \
		/* pixel clock */ \
		arm_hardware_piod_altfn20(1u << 18, GPIO_CFG_AF2); /* PD18 LCD_CLK */ \
		/* RED */ \
		arm_hardware_piod_altfn20(1u << 13, GPIO_CFG_AF2); /* R3 PD13 LCD_D19 */ \
		arm_hardware_piod_altfn20(1u << 14, GPIO_CFG_AF2); /* R4 PD14 LCD_D20 */ \
		arm_hardware_piod_altfn20(1u << 15, GPIO_CFG_AF2); /* R5 PD15 LCD_D21 */ \
		arm_hardware_piod_altfn20(1u << 16, GPIO_CFG_AF2); /* R6 PD16 LCD_D22 */ \
		arm_hardware_piod_altfn20(1u << 17, GPIO_CFG_AF2); /* R7 PD17 LCD_D23 */ \
		/* GREEN */ \
		arm_hardware_piod_altfn20(1u << 6, GPIO_CFG_AF2); 	/* G2 PD6 LCD_D10 */ \
		arm_hardware_piod_altfn20(1u << 7, GPIO_CFG_AF2); 	/* G3 PD7 LCD_D11 */ \
		arm_hardware_piod_altfn20(1u << 8, GPIO_CFG_AF2); 	/* G4 PD8 LCD_D12 */ \
		arm_hardware_piod_altfn20(1u << 9, GPIO_CFG_AF2); 	/* G5 PD9 LCD_D13 */ \
		arm_hardware_piod_altfn20(1u << 10, GPIO_CFG_AF2); /* G6 PD10 LCD_D14 */ \
		arm_hardware_piod_altfn20(1u << 11, GPIO_CFG_AF2); /* G7 PD11 LCD_D15 */ \
		/* BLUE  */ \
		arm_hardware_piod_altfn20(1u << 1, GPIO_CFG_AF2); 	/* B3 PD1 LCD_D3 */ \
		arm_hardware_piod_altfn20(1u << 2, GPIO_CFG_AF2); 	/* B4 PD2 LCD_D4 */ \
		arm_hardware_piod_altfn20(1u << 3, GPIO_CFG_AF2); 	/* B5 PD3 LCD_D5 */ \
		arm_hardware_piod_altfn20(1u << 4, GPIO_CFG_AF2); 	/* B6 PD4 LCD_D6 */ \
		arm_hardware_piod_altfn20(1u << 5, GPIO_CFG_AF2); 	/* B7 PD5 LCD_D7 */ \
	} while (0)

	/* управление состоянием сигнала DISP панели */
	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_SET_DISP(state) do { \
		const portholder_t VSmask = (1U << 21); 	/* PD21 LCD_VSYNCC */ \
		const portholder_t HSmask = (1U << 20); 	/* PD20 LCD_HSYNC */ \
		const portholder_t DEmask = (1U << 19); 	/* PD19 LCD_DE */ \
		/* while ((GPIOD->DATA & VSmask) != 0) ; */ /* схема синхронизации стоит на плате дисплея. дождаться 0 */ \
		/* while ((GPIOD->DATA & VSmask) == 0) ; */ /* дождаться 1 */ \
		arm_hardware_piod_outputs(DEmask, ((state) != 0) * DEmask); /* DE=DISP, pin 31 - можно менять только при VSYNC=1 */ \
	} while (0)

	#define HARDWARE_LVDS_INITIALIZE() do { \
		arm_hardware_piod_altfn50(1u << 0, GPIO_CFG_AF3); 	/* PD0 LVDS0_V0P */ \
		arm_hardware_piod_altfn50(1u << 1, GPIO_CFG_AF3); 	/* PD1 LVDS0_V0N */ \
		arm_hardware_piod_altfn50(1u << 2, GPIO_CFG_AF3); 	/* PD2 LVDS0_V1P */ \
		arm_hardware_piod_altfn50(1u << 3, GPIO_CFG_AF3); 	/* PD3 LVDS0_V1N */ \
		arm_hardware_piod_altfn50(1u << 4, GPIO_CFG_AF3); 	/* PD4 LVDS0_V2P */ \
		arm_hardware_piod_altfn50(1u << 5, GPIO_CFG_AF3); 	/* PD5 LVDS0_V2N */ \
		arm_hardware_piod_altfn50(1u << 6, GPIO_CFG_AF3); 	/* PD6 LVDS0_CKP */ \
		arm_hardware_piod_altfn50(1u << 7, GPIO_CFG_AF3); 	/* PD7 LVDS0_CKN */ \
		arm_hardware_piod_altfn50(1u << 8, GPIO_CFG_AF3); 	/* PD8 LVDS0_V3P */ \
		arm_hardware_piod_altfn50(1u << 9, GPIO_CFG_AF3); 	/* PD9 LVDS0_V3N */ \
	} while (0)

#endif /* WITHLTDCHW */

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811)

	//	tsc interrupt XS26, pin 08
	//	tsc/LCD reset, XS26, pin 22
	//	tsc SCL: XS26, pin 01
	//	tsc SDA: XS26, pin 02

	void stmpe811_interrupt_handler(void);

	#define BOARD_STMPE811_INT_PIN (1u << 9)		/* PE9 : tsc interrupt XS26, pin 08 */
	#define BOARD_STMPE811_RESET_PIN (1u << 22)	/* PD22 : tsc/LCD reset, XS26, pin 22 */

	#define BOARD_STMPE811_INT_CONNECT() do { \
		arm_hardware_pioe_altfn20(BOARD_STMPE811_INT_PIN, GPIO_CFG_AF14); \
		arm_hardware_pioe_updown(BOARD_STMPE811_INT_PIN, 0); \
		/*arm_hardware_pioe_onchangeinterrupt(0 * BOARD_STMPE811_INT_PIN, 1 * BOARD_STMPE811_INT_PIN, 0 * BOARD_STMPE811_INT_PIN, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM); */ \
		gpioX_onchangeinterrupt(GPIOE, 0 * BOARD_STMPE811_INT_PIN, 1 * BOARD_STMPE811_INT_PIN, 0 * BOARD_STMPE811_INT_PIN, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM, gt911_interrupt_handler); \
	} while (0)

#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811) */

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_GT911)

	/* PE9 TS_INT */
	/* PD22 FPLCD_RESET */

	void gt911_interrupt_handler(void);

	#define BOARD_GT911_INT_PIN (1u << 9)		/* PE9 : tsc interrupt XS26, pin 08 */
	#define BOARD_GT911_RESET_PIN (1u << 22)	/* PD22 : tsc/LCD reset, XS26, pin 22 */

	#define BOARD_GT911_RESET_SET(v) do { gpioX_setstate(GPIOD, (v), !! (0) * (v)); } while (0)
	#define BOARD_GT911_INT_SET(v) do { gpioX_setstate(GPIOE, (v), !! (0) * (v)); } while (0)

	#define BOARD_GT911_RESET_INITIO_1() do { \
		arm_hardware_pioe_outputs2m(BOARD_GT911_INT_PIN, 1* BOARD_GT911_INT_PIN); \
		arm_hardware_piod_outputs2m(BOARD_GT911_RESET_PIN, 1 * BOARD_GT911_RESET_PIN); \
		 local_delay_ms(200);  \
	} while (0)

	#define BOARD_GT911_RESET_INITIO_2() do { \
		arm_hardware_pioe_inputs(BOARD_GT911_INT_PIN); \
		arm_hardware_pioe_updown(BOARD_GT911_INT_PIN, 0); \
	} while (0)

	#define BOARD_GT911_INT_CONNECT() do { \
		arm_hardware_pioe_altfn20(BOARD_GT911_INT_PIN, GPIO_CFG_AF14); \
		arm_hardware_pioe_updown(BOARD_GT911_INT_PIN, 0); \
		gpioX_onchangeinterrupt(GPIOE, 0 * BOARD_GT911_INT_PIN, 1 * BOARD_GT911_INT_PIN, 0 * BOARD_GT911_INT_PIN, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM, gt911_interrupt_handler); \
	} while (0)
	//gt911_interrupt_handler

#endif


#if 0
	#define BOARD_BLINK_BIT (0 * 1u << 22)	// PD22 (PWM7) - led on mq-r board (from VCC33
	#define BOARD_BLINK_INITIALIZE() do { \
			arm_hardware_piod_outputs(BOARD_BLINK_BIT, 0 * BOARD_BLINK_BIT); \
		} while (0)
	#define BOARD_BLINK_SETSTATE(state) do { \
			if (state) \
				gpioX_setstate(GPIOD, BOARD_BLINK_BIT, 0 * BOARD_BLINK_BIT); \
			else \
				gpioX_setstate(GPIOD, BOARD_BLINK_BIT, 1 * BOARD_BLINK_BIT); \
		} while (0)
#endif

	/* запрос на вход в режим загрузчика */
	#define BOARD_USERBOOT_BIT	(1u << 1)	/* PB1: ~USER_BOOT */
	#define BOARD_IS_USERBOOT() 0//(((GPIOB->DATA) & BOARD_USERBOOT_BIT) == 0 || ((GPIOE->DATA) & TARGET_ENC2BTN_BIT) == 0)
	#define BOARD_USERBOOT_INITIALIZE() do { \
			/*arm_hardware_piob_inputs(BOARD_USERBOOT_BIT); *//* set as input with pull-up */ \
			/*arm_hardware_pioe_inputs(TARGET_ENC2BTN_BIT);*/ /* set as input with pull-up */ \
		} while (0)
	#define BOARD_USERBOOT_INITIALIZE() do { \
		} while (0)

	/* макроопределение, которое должно включить в себя все инициализации */
	#define	HARDWARE_INITIALIZE() do { \
		HARDWARE_FPGA_RESET(); \
		/*BOARD_BLINK_INITIALIZE(); */\
		HARDWARE_KBD_INITIALIZE(); \
		/*HARDWARE_DAC_INITIALIZE(); */\
		/*HARDWARE_BL_INITIALIZE(); */\
		HARDWARE_DCDC_INITIALIZE(); \
		TXDISABLE_INITIALIZE(); \
		TUNE_INITIALIZE(); \
		BOARD_USERBOOT_INITIALIZE(); \
		USBD_EHCI_INITIALIZE(); \
	} while (0)

#endif /* ARM_ALLWT128S3_CPUSTYLE_STORCH_V9A_H_INCLUDED */
