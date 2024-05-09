/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// mango-pi board

#ifndef ARM_ALLW_F1333_CPUSTYLE_MANGO_PI_H_INCLUDED
#define ARM_ALLW_F1333_CPUSTYLE_MANGO_PI_H_INCLUDED 1

#define WITHSPI16BIT	1	/* возможно использование 16-ти битных слов при обмене по SPI */
#define WITHSPI32BIT	1	/* возможно использование 32-ти битных слов при обмене по SPI */
#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
//#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
//#define WITHSPISW 	1	/* Использование программного управления SPI. Нельзя убирать эту строку - требуется явное отключение из-за конфликта с I2C */

//#define WIHSPIDFSW	1	/* программное обслуживание DATA FLASH */
//#define WIHSPIDFOVERSPI 1	/* Для работы используется один из обычных каналов SPI */
#define WIHSPIDFHW		1	/* аппаратное обслуживание DATA FLASH */
//#define WIHSPIDFHW2BIT	1	/* аппаратное обслуживание DATA FLASH с поддержкой QSPI подключения по 2-м проводам */
#define WIHSPIDFHW4BIT	1	/* аппаратное обслуживание DATA FLASH с поддержкой QSPI подключения по 4-м проводам */

//#define WITHDMA2DHW		1	/* Использование DMA2D для формирования изображений	- у STM32MP1 его нет */

//#define WITHCAN0HW 1
#define WITHCAN1HW 1

#if WITHINTEGRATEDDSP
	#define WITHI2S1HW	1	/* Использование I2S1 - аудиокодек на I2S */
	#define WITHI2S2HW	1	/* Использование I2S2 - FPGA или IF codec	*/
#endif /* WITHINTEGRATEDDSP */

#if WITHDEBUG
	#define WITHUART0HW	1	/* PE2 PE3 Используется периферийный контроллер последовательного порта #0 UART0 */
	//#define WITHUARTFIFO	1	/* испольование FIFO */
#endif /* WITHDEBUG */

//#define WITHCAT_USART0		1
#define WITHDEBUG_UART0	1
//#define WITHTINYUSB 1
#define BOARD_TUH_RHPORT 1

// OHCI at USB1HSFSP2_BASE
////#define WITHUSBHW_OHCI ((struct ohci_registers *) USB1HSFSP2_BASE)

#if WITHISBOOTLOADER

	#define WITHSDRAMHW	1		/* В процессоре есть внешняя память */
	//#define WITHSDRAM_PMC1	1	/* power management chip */

	//#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
	//#define WITHGPUHW	1	/* Graphic processor unit */
	//#define WITHEHCIHW	1	/* USB_EHCI controller */

	////#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */
	#define USBPHYC_MISC_SWITHOST_VAL 0		// 0 or 1 - value for USBPHYC_MISC_SWITHOST field. 0: Select OTG controller for 2nd PHY port, 1: Select Host controller for 2nd PHY port
	#define USBPHYC_MISC_PPCKDIS_VAL 0x00

	////#define WITHUSBHW_DEVICE	USB_OTG_HS	/* на этом устройстве поддерживается функциональность DEVICE	*/
	#define WITHUSBDEV_VBUSSENSE	1		/* используется предопределенный вывод OTG_VBUS */
	#define WITHUSBDEV_HSDESC	1			/* Требуется формировать дескрипторы как для HIGH SPEED */
	//#define WITHUSBDEV_HIGHSPEEDULPI	1
	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USBH_HS_DP & USBH_HS_DM
	//#define WITHUSBDEV_DMAENABLE 1

	//#define WITHUSBHW_HOST		USB_OTG_HS
	#define WITHUSBHOST_HIGHSPEEDPHYC	1	// UTMI -> USB_DP2 & USB_DM2
	#define WITHUSBHOST_DMAENABLE 1

	////#define WITHEHCIHW	1	/* USB_EHCI controller */
	////#define WITHUSBHW_EHCI		USB1_EHCI
	#define WITHEHCIHW_EHCIPORT 0	// 0 - use 1st PHY port
	#define WITHOHCIHW_OHCIPORT 0


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

	//#define WITHLWIP 1
	//#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
	//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */
	//#define WITHUSBRNDIS	1	/* RNDIS использовать Remote NDIS на USB соединении */

#else /* WITHISBOOTLOADER */


	#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
//	#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */
//	#define WITHSDHCHW	1	/* Hardware SD HOST CONTROLLER */
//	#define WITHSDHC0HW	1		/* Hardware SD HOST #0 CONTROLLER */
//	#define WITHSDHC1HW	1		/* SDIO */

	//#define WITHETHHW 1	/* Hardware Ethernet controller */

	//#define WITHDCDCFREQCTL	1		// Имеется управление частотой преобразователей блока питания
	//#define WITHBLPWMCTL	1		// Имеется управление яркостью подсветки дисплея через PWM
	//#define WITHCODEC1_I2S1_DUPLEX_SLAVE	1		/* Обмен с аудиокодеком через I2S1 */
	//#define WITHFPGAIF_I2S2_DUPLEX_SLAVE	1		/* Обмен с FPGA через I2S2 */
	//#define WITHCODEC1_I2S1_DUPLEX_MASTER	1		/* Обмен с аудиокодеком через I2S1 */
	//#define WITHFPGAIF_I2S2_DUPLEX_MASTER	1		/* Обмен с FPGA через I2S2 */
	#define WITHCODEC1_WHBLOCK_DUPLEX_MASTER	1	/* встороенный в процессор кодек */

	#define WITHMDMAHW		1	/* Использование G2D для формирования изображений */
	//#define WITHCPUDACHW	1	/* использование встроенного в процессор DAC */
	#define WITHCPUADCHW 	1	/* использование встроенного в процессор ADC */

	#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
	//#define WITHGPUHW	1	/* Graphic processor unit */
	#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */

	#define WITHUSBHW_DEVICE	USBOTG0	/* на этом устройстве поддерживается функциональность DEVICE	*/
	#define WITHUSBDEV_VBUSSENSE	1		/* используется предопределенный вывод OTG_VBUS */
	#if WITHFUSBDFS
		//#define WITHUSBDEV_HSDESC	1			/* Требуется формировать дескрипторы как для HIGH SPEED */
	#elif WITHFUSBDHS
		#define WITHUSBDEV_HSDESC	1			/* Требуется формировать дескрипторы как для HIGH SPEED */
	#else
		#define WITHUSBDEV_HSDESC	1			/* Требуется формировать дескрипторы как для HIGH SPEED */
	#endif
	//#define WITHUSBDEV_HIGHSPEEDULPI	1	// ULPI
	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USB0_DP & USB0_DM
	//#define WITHUSBDEV_DMAENABLE 1

//	#define WITHUSBHW_HOST		USBOTG0
//	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USB0_DP & USB0_DM
//	#define WITHUSBHOST_DMAENABLE 1

	#define WITHEHCIHW	1	/* USB_EHCI controller */

	#define WITHTINYUSB 1
	#define BOARD_TUH_RHPORT 1

	#define WITHUSBHW_EHCI		USBEHCI1
	#define WITHUSBHW_EHCI_IRQ	USB1_EHCI_IRQn
	#define WITHUSBHW_EHCI_IX	1

	#define WITHUSBHW_OHCI		USBOHCI1
	#define WITHUSBHW_OHCI_IRQ	USB1_OHCI_IRQn
	#define WITHUSBHW_OHCI_IX	1

	#define WITHUSBHOST_HIGHSPEEDPHYC	1	// UTMI -> USB1_DP & USB1_DM
	#define WITHEHCIHW_EHCIPORT 0	// 0 - use 1st PHY port
	#define WITHOHCIHW_OHCIPORT 0

	#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
	#define WITHMODEM_CDC	1

	#if WITHUSBHEADSET
		#define WITHUAC2		WITHUSBDEV_HSDESC //1	/* UAC2 support */
		#define WITHUSBUACINOUT		1	/* совмещённое усройство ввода/вывода (без спектра) */
		#define WITHUSBUACOUT		1	/* использовать виртуальную звуковую плату на USB соединении */
		#define WITHUSBUACIN		1
		#define UACOUT_AUDIO48_SAMPLEBYTES	3	/* должны быть 2, 3 или 4 */
		#define UACIN_AUDIO48_SAMPLEBYTES	3	/* должны быть 2, 3 или 4 */
	#elif WITHINTEGRATEDDSP

		#define UACOUT_AUDIO48_SAMPLEBYTES	2	/* должны быть 2, 3 или 4 */
		#define UACIN_AUDIO48_SAMPLEBYTES	2	/* должны быть 2, 3 или 4 */
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
	#define WITHUSBCDCACM_N		1	/* количество виртуальных последовательных портов */


	#if WITHLWIP
		#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
		//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */
		//#define WITHUSBRNDIS	1	/* RNDIS использовать Remote NDIS на USB соединении */
	#endif /* WITHLWIP */
	//#define WITHUSBHID	1	/* HID использовать Human Interface Device на USB соединении */

	#if WIHSPIDFHW || WIHSPIDFSW
		#define WITHUSBDFU	1	/* DFU USB Device Firmware Upgrade support */
		#define WITHUSBWCID	1
	#endif /* WIHSPIDFHW || WIHSPIDFSW */

	//#define WITHUSBDMTP	1	/* MTP USB Device */
	//#define WITHUSBDMSC	1	/* MSC USB device */

#endif /* WITHISBOOTLOADER */


#define LS020_RS_INITIALIZE() do { \
		/*arm_hardware_piod_outputs2m(LS020_RS, LS020_RS); *//* PD3 */ \
	} while (0)

#define LS020_RESET_INITIALIZE() do { \
		/*arm_hardware_piod_outputs2m(LS020_RESET, LS020_RESET); *//* PD4 */ \
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
	#define LS020_RS			(UINT32_C(1) << 3)			// PD3 signal

#elif LCDMODE_SPI_RN
	// эти контроллеры требуют только RESET

	#define LS020_RESET_PORT_S(v)		do { } while (0) //do { GPIOD->BSRR = BSRR_S(v); (void) GPIOD->BSRR; } while (0)
	#define LS020_RESET_PORT_C(v)		do { } while (0) //do { GPIOD->BSRR = BSRR_C(v); (void) GPIOD->BSRR; } while (0)
	#define LS020_RESET			0//(UINT32_C(1) << 4)			// PD4 signal

#elif LCDMODE_SPI_RA
	// Эти контроллеры требуют RESET и RS
	// LCDMODE_UC1608

	#define LS020_RS_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); (void) GPIOD->BSRR; } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); (void) GPIOD->BSRR; } while (0)
	#define LS020_RS			(UINT32_C(1) << 3)			// PD3 signal

	#define LS020_RESET_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); (void) GPIOD->BSRR; } while (0)
	#define LS020_RESET_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); (void) GPIOD->BSRR; } while (0)
	#define LS020_RESET			(UINT32_C(1) << 2)			// PD4 signal

#elif LCDMODE_HD44780 && (LCDMODE_SPI == 0)

	#error Unsupported LCDMODE_HD44780

#endif

#if WITHENCODER

	// Выводы подключения енкодера #1
	#define ENCODER_INPUT_PORT	(GPIOG->DATA)
	#define ENCODER_BITA		(UINT32_C(1) << 13)		// PG13
	#define ENCODER_BITB		(UINT32_C(1) << 9)		// PG9

	// Выводы подключения енкодера #2
	#define ENCODER2_INPUT_PORT	(GPIOG->DATA)
	#define ENCODER2_BITA		(UINT32_C(1) << 15)		// PG15
	#define ENCODER2_BITB		(UINT32_C(1) << 14)		// PG14

	/* Определения масок битов для формирования обработчиков прерываний в нужном GPIO */
	#define BOARD_GPIOG_ENCODER_BITS		(ENCODER_BITA | ENCODER_BITB)
	#define BOARD_GPIOG_ENCODER2_BITS		(ENCODER2_BITA | ENCODER2_BITB)

	#define ENCODER_BITS_GET() (((ENCODER_INPUT_PORT & ENCODER_BITA) != 0) * 2 + ((ENCODER_INPUT_PORT & ENCODER_BITB) != 0))
	#define ENCODER2_BITS_GET() (((ENCODER2_INPUT_PORT & ENCODER2_BITA) != 0) * 2 + ((ENCODER2_INPUT_PORT & ENCODER2_BITB) != 0))

	#define ENCODER_INITIALIZE() do { \
			arm_hardware_piog_inputs(BOARD_GPIOG_ENCODER_BITS); \
			arm_hardware_piog_updown(_xMask, BOARD_GPIOG_ENCODER_BITS, 0); \
			arm_hardware_piog_onchangeinterrupt(BOARD_GPIOG_ENCODER_BITS, BOARD_GPIOG_ENCODER_BITS, BOARD_GPIOG_ENCODER_BITS, ARM_OVERREALTIME_PRIORITY, TARGETCPU_OVRT); \
			arm_hardware_piog_inputs(BOARD_GPIOG_ENCODER2_BITS); \
			arm_hardware_piog_updown(_xMask, BOARD_GPIOG_ENCODER2_BITS, 0); \
			arm_hardware_piog_onchangeinterrupt(0 * BOARD_GPIOG_ENCODER2_BITS, BOARD_GPIOG_ENCODER2_BITS, BOARD_GPIOG_ENCODER2_BITS, ARM_OVERREALTIME_PRIORITY, TARGETCPU_OVRT); \
		} while (0)

#endif

	// Инициализируются I2S1 в дуплексном режиме.
	// аудиокодек
	#define I2S1HW_INITIALIZE(master) do { \
		arm_hardware_piog_altfn20(!! (master) * UINT32_C(1) << 11, GPIO_CFG_AF2); /* TEST PG11 I2S1-MCLK	pin P2-7  - wire to pin 8 hseda 24bit vga+audio board */ \
		arm_hardware_piog_altfn20(UINT32_C(1) << 12, GPIO_CFG_AF2); /* PG12 I2S1-LRCK	WL_REG_ON, pin P2-6 - wire to pin 9 */ \
		arm_hardware_piog_altfn20(UINT32_C(1) << 13, GPIO_CFG_AF2); /* PG13 I2S1-BCLK	AP_WAKE_BT, pin P2-5 - wire to pin 11 */ \
		arm_hardware_piog_altfn20(UINT32_C(1) << 14, GPIO_CFG_AF2); /* PG14 I2S1-DIN0 from codec, BT_WAKE_AP, pin P2-4 - wire to pin 4 */ \
		arm_hardware_piog_altfn20(UINT32_C(1) << 15, GPIO_CFG_AF2); /* PG15 I2S1-DOUT0 co codec, BT_EN, pin P2-3 - wire to pin 10 */ \
	} while (0)
	#define HARDWARE_I2S1HW_DIN 0	/* DIN0 used */
	#define HARDWARE_I2S1HW_DOUT 0	/* DOUT0 used */
	// Инициализируются I2S2 в дуплексном режиме.
	// FPGA или IF codec
	#define I2S2HW_INITIALIZE(master) do { \
		arm_hardware_piob_altfn20(0 * UINT32_C(1) << 6,	GPIO_CFG_AF3); /* PB6 I2S2-LRCK	*/ \
		arm_hardware_piob_altfn20(0 * UINT32_C(1) << 5,	GPIO_CFG_AF3); /* PB5 I2S2-BCLK	*/ \
		arm_hardware_piob_altfn20(0 * UINT32_C(1) << 4,	GPIO_CFG_AF3); /* PB4 I2S2-DOUT0 to FPGA */ \
		arm_hardware_piob_altfn20(0 * UINT32_C(1) << 3,	GPIO_CFG_AF5); /* PB3 I2S2-DIN0 from FPGA */ \
	} while (0)
	#define HARDWARE_I2S2HW_DIN 0	/* DIN0 used */
	#define HARDWARE_I2S2HW_DOUT 0	/* DOUT0 used */

/* Распределение битов в ARM контроллерах */

#if (WITHCAT && WITHCAT_USART2)
	// CAT data lites
	// RXD at PA10, TXD at PA9

	// CAT control lines
	//#define FROMCAT_TARGET_PIN_RTS		(GPIOA->DATA)
	//#define FROMCAT_BIT_RTS				(UINT32_C(1) << 11)	/* PA11 сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	//#define FROMCAT_TARGET_PIN_DTR		(GPIOA->DATA)
	//#define FROMCAT_BIT_DTR				(UINT32_C(1) << 12)	/* PA12 сигнал DTR от FT232RL	*/

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
	//#define FROMCAT_BIT_RTS				(UINT32_C(1) << 11)	/* сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	//#define FROMCAT_TARGET_PIN_DTR		(GPIOA->DATA) // was PINA
	//#define FROMCAT_BIT_DTR				(UINT32_C(1) << 12)	/* сигнал DTR от FT232RL	*/

	/* манипуляция от виртуального CDC порта */
	#define FROMCAT_DTR_INITIALIZE() do { \
		} while (0)

	/* переход на передачу от виртуального CDC порта*/
	#define FROMCAT_RTS_INITIALIZE() do { \
		} while (0)

#endif /* (WITHCAT && WITHCAT_CDC) */

#if WITHSDHCHW && WITHSDHC0HW

	#define	USERFIRSTSBLOCK 0

	#define	SMHCHARD_IX 0	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_PTR SMHC0	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_BASE SMHC0_BASE	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_CCU_CLK_REG (CCU->SMHC0_CLK_REG)	/* 0 - SMHC0, 1: SMHC1... */
	#define SMHCHARD_FREQ (allwnrt113_get_smhc0_freq())
	#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */

	#define HARDWARE_SDIO_INITIALIZE() do { \
		arm_hardware_piof_altfn50(UINT32_C(1) << 3, GPIO_CFG_AF2);	/* PF3 - SDIO_CMD	*/ \
		arm_hardware_piof_altfn50(UINT32_C(1) << 2, GPIO_CFG_AF2);	/* PF2 - SDIO_CK	*/ \
		arm_hardware_piof_altfn50(UINT32_C(1) << 1, GPIO_CFG_AF2);	/* PF1 - SDIO_D0	*/ \
		arm_hardware_piof_altfn50(UINT32_C(1) << 0, GPIO_CFG_AF2);	/* PF0 - SDIO_D1	*/ \
		arm_hardware_piof_altfn50(UINT32_C(1) << 5, GPIO_CFG_AF2);	/* PF5 - SDIO_D2	*/ \
		arm_hardware_piof_altfn50(UINT32_C(1) << 4, GPIO_CFG_AF2);	/* PF4 - SDIO_D3	*/ \
	} while (0)
	/* отключить процессор от SD карты - чтобы при выполнении power cycle не возникало фантомное питание через сигналы управления. */
	#define HARDWARE_SDIO_HANGOFF()	do { \
		arm_hardware_piof_inputs(UINT32_C(1) << 3);	/* PF3 - SDIO_CMD	*/ \
		arm_hardware_piof_inputs(UINT32_C(1) << 2);	/* PF2 - SDIO_CK	*/ \
		arm_hardware_piof_inputs(UINT32_C(1) << 1);	/* PF1 - SDIO_D0	*/ \
		arm_hardware_piof_inputs(UINT32_C(1) << 0);	/* PF0 - SDIO_D1	*/ \
		arm_hardware_piof_inputs(UINT32_C(1) << 5);	/* PF5 - SDIO_D2	*/ \
		arm_hardware_piof_inputs(UINT32_C(1) << 4);	/* PF4 - SDIO_D3	*/ \
		arm_hardware_piof_updown(_xMask, 0, UINT32_C(1) << 3);	/* PF3 - SDIO_CMD	*/ \
		arm_hardware_piof_updown(_xMask, 0, UINT32_C(1) << 2);	/* PF2 - SDIO_CK	*/ \
		arm_hardware_piof_updown(_xMask, 0, UINT32_C(1) << 1);	/* PF1 - SDIO_D0	*/ \
		arm_hardware_piof_updown(_xMask, 0, UINT32_C(1) << 0);	/* PF0 - SDIO_D1	*/ \
		arm_hardware_piof_updown(_xMask, 0, UINT32_C(1) << 5);	/* PF5 - SDIO_D2	*/ \
		arm_hardware_piof_updown(_xMask, 0, UINT32_C(1) << 4);	/* PF4 - SDIO_D3	*/ \
	} while (0)

	#define HARDWARE_SDIO_CD_BIT	(UINT32_C(1) << 6)	/* PF6 - SDC0_DET */

	#define HARDWARE_SDIOSENSE_INITIALIZE()	do { \
			arm_hardware_piof_inputs(HARDWARE_SDIO_CD_BIT); /* PF6 - SDC0_DET */ \
			arm_hardware_piof_updown(HARDWARE_SDIO_CD_BIT, HARDWARE_SDIO_CD_BIT, 0); \
	} while (0)

	// HARDWARE_SDIOSENSE_CD: 1-has disk. 0-no disk
	#define HARDWARE_SDIOSENSE_CD() ((GPIOF->DATA & HARDWARE_SDIO_CD_BIT) == 0)	/* == 0: no disk. получить состояние датчика CARD PRESENT */
	#define HARDWARE_SDIOSENSE_WP() 0//((GPIOG->DATA & HARDWARE_SDIO_WP_BIT) != 0)	/* != 0: write protected получить состояние датчика CARD WRITE PROTECT */

	/* если питание SD CARD управляется прямо с процессора */
	#define HARDWARE_SDIOPOWER_INITIALIZE()	do { \
		} while (0)
	/* parameter on not zero for powering SD CARD */
	#define HARDWARE_SDIOPOWER_SET(on) do { \
	} while (0)

#elif WITHSDHCHW && WITHSDHC1HW
	// SDIO
	#define USERFIRSTSBLOCK 0

	// MangoPI T113-s3 ports:
	// SMHC0: SDC0 - TF CARD
	// SMHC1: SDC1 - SDIO

	// Additional lines to module TL8189FQB2 (Realtek RTL8189FTV based) :
	// PG10 - WL_WAKE_AP 	- pin 13 WL Wake-up host (GPIOB2) - to host
	// PG12 - WL_REG_ON		- pin 12 (tied to +3.3) Power key (L=OFF, H=ON)
	// PG13 - AP_WAKE_BT 	- pin 06 Host wake (GPIOB5) - from host
	// PG14 - BT_WAKE_AP	- pin 07 WL Wake-up (GPIOB2) - to host
	// PG15 - BT_EN			- pin 34 (tied to +3.3) - from host
	// PG11	- LPO			- pin 24 (tied to +3.3) (not connected to CPU)

	#define	SMHCHARD_IX 1	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_PTR SMHC1	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_BASE SMHC1_BASE	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_CCU_CLK_REG (CCU->SMHC1_CLK_REG)	/* 0 - SMHC0, 1: SMHC1... */
	#define SMHCHARD_FREQ (allwnrt113_get_smhc1_freq())
	#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */

	#define HARDWARE_SDIO_INITIALIZE() do { \
		arm_hardware_piog_altfn50(UINT32_C(1) << 1, GPIO_CFG_AF2);	/* PG1 - SDIO_CMD	*/ \
		arm_hardware_piog_altfn50(UINT32_C(1) << 0, GPIO_CFG_AF2);	/* PG0 - SDIO_CK	*/ \
		arm_hardware_piog_altfn50(UINT32_C(1) << 2, GPIO_CFG_AF2);	/* PG2 - SDIO_D0	*/ \
		arm_hardware_piog_altfn50(UINT32_C(1) << 3, GPIO_CFG_AF2);	/* PG3 - SDIO_D1	*/ \
		arm_hardware_piog_altfn50(UINT32_C(1) << 4, GPIO_CFG_AF2);	/* PG4 - SDIO_D2	*/ \
		arm_hardware_piog_altfn50(UINT32_C(1) << 5, GPIO_CFG_AF2);	/* PG5 - SDIO_D3	*/ \
	} while (0)
	/* отключить процессор от SD карты - чтобы при выполнении power cycle не возникало фантомное питание через сигналы управления. */
	#define HARDWARE_SDIO_HANGOFF()	do { \
	} while (0)

	#define HARDWARE_SDIOSENSE_INITIALIZE()	do { \
	} while (0)


	#define HARDWARE_SDIOSENSE_CD() 1 /* == 0: no disk. получить состояние датчика CARD PRESENT */
	#define HARDWARE_SDIOSENSE_WP() 0 /* != 0: write protected получить состояние датчика CARD WRITE PROTECT */

	/* если питание SD CARD управляется прямо с процессора */
	#define HARDWARE_SDIOPOWER_INITIALIZE()	do { \
		} while (0)
	/* parameter on not zero for powering SD CARD */
	#define HARDWARE_SDIOPOWER_SET(on) do { \
	} while (0)


#endif /* WITHSDHCHW && WITHSDHC0HW */

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

	#define TXPATH_INITIALIZE() do { \
		} while (0)


	// +++
	// TXDISABLE input - PD10
	#define TXDISABLE_TARGET_PIN				(GPIOD->DATA)
	#define TXDISABLE_BIT_TXDISABLE				0//(UINT32_C(1) << 10)		// PD10 - TX INHIBIT
	// получить бит запрета передачи (от усилителя мощности)
	#define HARDWARE_GET_TXDISABLE() (0) //((TXDISABLE_TARGET_PIN & TXDISABLE_BIT_TXDISABLE) != 0)
	#define TXDISABLE_INITIALIZE() do { \
			arm_hardware_piod_inputs(TXDISABLE_BIT_TXDISABLE); \
			arm_hardware_piod_updown(TXDISABLE_BIT_TXDISABLE, 0, TXDISABLE_BIT_TXDISABLE); \
		} while (0)
	// ---

	// +++
	// PTT input - PD10
	// PTT2 input - PD9
	#define PTT_TARGET_PIN				(GPIOD->DATA)
	#define PTT_BIT_PTT					(0 * UINT32_C(1) << 10)		// PD10 - PTT
	#define PTT2_TARGET_PIN				(GPIOD->DATA)
	#define PTT2_BIT_PTT				(0 * UINT32_C(1) << 9)		// PD9 - PTT2
	// получить бит запроса оператором перехода на пердачу
	#define HARDWARE_GET_PTT() (0) //((PTT_TARGET_PIN & PTT_BIT_PTT) == 0 || (PTT2_TARGET_PIN & PTT2_BIT_PTT) == 0)
	#define PTT_INITIALIZE() do { \
			arm_hardware_piod_inputs(PTT_BIT_PTT); \
			arm_hardware_piod_updown(PTT_BIT_PTT, PTT_BIT_PTT, 0); \
			arm_hardware_piod_inputs(PTT2_BIT_PTT); \
			arm_hardware_piod_updown(PTT2_BIT_PTT, PTT2_BIT_PTT, 0); \
		} while (0)
	// ---
	// TUNE input - PD11
	#define TUNE_TARGET_PIN				(GPIOD->DATA)
	#define TUNE_BIT_TUNE				(0 * UINT32_C(1) << 11)		// PD11
	#define HARDWARE_GET_TUNE() 0//((TUNE_TARGET_PIN & TUNE_BIT_TUNE) == 0)
	#define TUNE_INITIALIZE() do { \
			arm_hardware_piod_inputs(TUNE_BIT_TUNE); \
			arm_hardware_piod_updown(TUNE_BIT_TUNE, TUNE_BIT_TUNE, 0); \
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
	#define ELKEY_BIT_LEFT				(UINT32_C(1) << 4)		// PB4
	#define ELKEY_BIT_RIGHT				(UINT32_C(1) << 5)		// PB5

	#define ELKEY_TARGET_PIN			(GPIOB->DATA)

	#define HARDWARE_GET_ELKEY_LEFT() 	((ELKEY_TARGET_PIN & ELKEY_BIT_LEFT) == 0)
	#define HARDWARE_GET_ELKEY_RIGHT() 	((ELKEY_TARGET_PIN & ELKEY_BIT_RIGHT) == 0)


	#define ELKEY_INITIALIZE() do { \
			arm_hardware_piob_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
			arm_hardware_piob_updown(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, 0); \
		} while (0)

#endif /* WITHELKEY */

// IOUPDATE = PA15
//#define SPI_IOUPDATE_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); (void) GPIOA->BSRR; } while (0)
//#define SPI_IOUPDATE_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); (void) GPIOA->BSRR; } while (0)
//#define SPI_IOUPDATE_BIT		(UINT32_C(1) << 15)	// * PA15

#if WITHSPIHW || WITHSPISW
	// Набор определений для работы без внешнего дешифратора

	#define targetdataflash 0xFF
	#define targetnone 0x00

	#define targetext1		(0*UINT32_C(1) << 0)		// PG0 ext1 on front panel CSEXT1
	#define targetnvram		(0*UINT32_C(1) << 7)		// PG7 nvram FM25L16B
	#define targetctl1		(0*UINT32_C(1) << 11)		// PG11 board control registers chain
	#define targetcodec1	(0*UINT32_C(1) << 6)		// PG6 on-board codec1 NAU8822L
	#define targetfpga1		(0*UINT32_C(1) << 2)		// PG2 FPGA control registers CS1
	#define targetrtc1		(0*UINT32_C(1) << 10)		// PG10 RTC DS1305 RTC_CS

	#define targetadc2		(0*UINT32_C(1) << 8)	// PG8 on-board ADC MCP3208-BI/SL chip select (potentiometers) ADCCS1
	#define targetadck		(0*UINT32_C(1) << 9)	// PG9 on-board ADC MCP3208-BI/SL chip select (KEYBOARD) ADCCS2
	#define targetxad2		(0*UINT32_C(1) << 3)	// PG3 external SPI device (PA BOARD ADC) CSEXT2

	#define targetlcd	targetext1 	/* LCD over SPI line devices control */ 
	#define targetuc1608 targetext1	/* LCD with positive chip select signal	*/
	#define targettsc1 		targetext1	/* XPT2046 SPI chip select signal */

	/* Select specified chip. */
	#define SPI_CS_ASSERT(target) do { \
		switch (target) { \
		case targetdataflash: { gpioX_setstate(GPIOC, SPDIF_NCS_BIT, 0 * (SPDIF_NCS_BIT)); local_delay_us(1); } break; /* PC3 SPI0_CS */ \
		case targetrtc1: { gpioX_setstate(GPIOG, (target), 1 * (target)); local_delay_us(1); } break; \
		default: { gpioX_setstate(GPIOG, (target), 0 * (target)); local_delay_us(1); } break; \
		/*case targetnone: break; */\
		} \
	} while (0)

	/* Unelect specified chip. */
	#define SPI_CS_DEASSERT(target)	do { \
		switch (target) { \
		case targetdataflash: { gpioX_setstate(GPIOC, SPDIF_NCS_BIT, 1 * (SPDIF_NCS_BIT)); local_delay_us(1); } break; /* PC3 SPI0_CS */ \
		case targetrtc1: { gpioX_setstate(GPIOG, (target), 0 * (target)); local_delay_us(1); } break; \
		default: { gpioX_setstate(GPIOG, (target), 1 * (target)); local_delay_us(1); } break; \
		/*case targetnone: break; */\
		} \
	} while (0)

	/* Perform delay after assert or de-assert specific CS line */
	#define SPI_CS_DELAY(target) do { \
		switch (target) { \
		default: break; \
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
	#define	SPI_SCLK_BIT			(UINT32_C(1) << 2)	// PC2 SPI0_CLK
	#define	SPI_MOSI_BIT			(UINT32_C(1) << 4)	// PC4 SPI0_MOSI
	#define	SPI_MISO_BIT			(UINT32_C(1) << 5)	// PC5 SPI0_MISO

	/* Выводы соединения с QSPI BOOT NOR FLASH */
	//#define SPDIF_SCLK_BIT (UINT32_C(1) << 2)	// PC2 SPI0_CLK
	#define SPDIF_NCS_BIT (UINT32_C(1) << 3)	// PC3 SPI0_CS
	//#define SPDIF_MOSI_BIT (UINT32_C(1) << 4)	// PC4 SPI0_MOSI
	//#define SPDIF_MISO_BIT (UINT32_C(1) << 5)	// PC5 SPI0_MISO
	#define SPDIF_D2_BIT (UINT32_C(1) << 6)		// PC6 SPI0_WP/D2
	#define SPDIF_D3_BIT (UINT32_C(1) << 7)		// PC7 SPI0_HOLD/D3

//	#define SPI_TARGET_SCLK_PORT_C(v)	do { gpioX_setstate(GPIOC, (v), !! (0) * (v)); local_delay_us(1); } while (0)
//	#define SPI_TARGET_SCLK_PORT_S(v)	do { gpioX_setstate(GPIOC, (v), !! (1) * (v)); local_delay_us(1); } while (0)
//
//	#define SPI_TARGET_MOSI_PORT_C(v)	do { gpioX_setstate(GPIOC, (v), !! (0) * (v)); local_delay_us(1); } while (0)
//	#define SPI_TARGET_MOSI_PORT_S(v)	do { gpioX_setstate(GPIOC, (v), !! (1) * (v)); local_delay_us(1); } while (0)
//
//	#define SPI_TARGET_MISO_PIN		(GPIOC->DATA)

	#define	SPIHARD_IX 0	/* 0 - SPI0, 1: SPI1... */
	#define	SPIHARD_PTR SPI0	/* 0 - SPI0, 1: SPI1... */
	#define	SPIHARD_CCU_CLK_REG (CCU->SPI0_CLK_REG)	/* 0 - SPI0, 1: SPI1... */
	#define BOARD_SPI_FREQ (allwnrt113_get_spi0_freq())
	#define	SPIDFHARD_PTR SPI0	/* 0 - SPI0, 1: SPI1... */

	#define SPIIO_INITIALIZE() do { \
		arm_hardware_pioc_altfn50(SPI_SCLK_BIT, GPIO_CFG_AF2); 	/* PC2 SPI0_CLK */ \
		arm_hardware_pioc_altfn50(SPI_MOSI_BIT, GPIO_CFG_AF2); 	/* PC4 SPI0_MOSI */ \
		arm_hardware_pioc_altfn50(SPI_MISO_BIT, GPIO_CFG_AF2); 	/* PC5 SPI0_MISO */ \
		arm_hardware_pioc_altfn50(SPDIF_D2_BIT, GPIO_CFG_AF2);  /* PC6 SPI0_WP/D2 */ \
		arm_hardware_pioc_altfn50(SPDIF_D3_BIT, GPIO_CFG_AF2);  /* PC7 SPI0_HOLD/D3 */ \
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

// WITHUART0HW
#define HARDWARE_UART0_INITIALIZE() do { \
		const portholder_t TXMASK = (UINT32_C(1) << 2); /* PE2 UART0-TX */ \
		const portholder_t RXMASK = (UINT32_C(1) << 3); /* PE3 UART0-RX - pull-up RX data */  \
		arm_hardware_pioe_altfn2(TXMASK, GPIO_CFG_AF6); \
		arm_hardware_pioe_altfn2(RXMASK, GPIO_CFG_AF6); \
		arm_hardware_pioe_updown(RXMASK, RXMASK, 0); \
	} while (0)

// WITHCAN0HW
#define HARDWARE_CAN0_INITIALIZE() do { \
		const portholder_t TXMASK = UINT32_C(1) << 2; /* PB2 CAN0_TX */ \
		const portholder_t RXMASK = UINT32_C(1) << 3; /* PB3 CAN0_RX - pull-up RX data */  \
		arm_hardware_piob_altfn2(TXMASK, GPIO_CFG_AF8); \
		arm_hardware_piob_altfn2(RXMASK, GPIO_CFG_AF8); \
		arm_hardware_piob_updown(RXMASK, RXMASK, 0); \
	} while (0)

// WITHCAN1HW
#define HARDWARE_CAN1_INITIALIZE() do { \
		const portholder_t TXMASK = UINT32_C(1) << 4; /* PB4 CAN1_TX - Header P2 pins */ \
		const portholder_t RXMASK = UINT32_C(1) << 5; /* PB5 CAN1_RX - pull-up RX data */  \
		arm_hardware_piob_altfn2(TXMASK, GPIO_CFG_AF8); \
		arm_hardware_piob_altfn2(RXMASK, GPIO_CFG_AF8); \
		arm_hardware_piob_updown(RXMASK, RXMASK, 0); \
	} while (0)

#define TARGET_ENC2BTN_BIT (UINT32_C(1) << 15)	// PE15 - second encoder button with pull-up

#if WITHKEYBOARD
	/* PE15: pull-up second encoder button */

	//#define TARGET_ENC2BTN_BIT (UINT32_C(1) << 15)	// PE15 - second encoder button with pull-up
	#define TARGET_POWERBTN_BIT 0//(UINT32_C(1) << 8)	// PAxx - ~CPU_POWER_SW signal

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
			arm_hardware_pioe_updown(TARGET_ENC2BTN_BIT, TARGET_ENC2BTN_BIT, 0); /* PE15: pull-up second encoder button */ \
			/*arm_hardware_pioa_inputs(TARGET_POWERBTN_BIT); */ \
			arm_hardware_pioa_updown(TARGET_POWERBTN_BIT, TARGET_POWERBTN_BIT, 0);	/* PAxx: pull-up second encoder button */ \
		} while (0)

#else /* WITHKEYBOARD */

	#define HARDWARE_KBD_INITIALIZE() do { \
		} while (0)

#endif /* WITHKEYBOARD */

#if (WITHTWISW || WITHTWIHW) && (WITHFLATLINK | WITHMIPIDSISHW)
	// RaspberyPI mipi-dsi board diagnostics:
	//	addr8bit=0x70, addr7bit=0x38 - looks like Capacitive Touch Panel Controller FT5406 www.focaltech-systems.com
	//	addr8bit=0x8A, addr7bit=0x45

	// TWI2-SCK PD20 SCL
	// TWI2-SDA PD21 SDA
	#define TARGET_TWI_TWCK		(UINT32_C(1) << 20)		// TWI2-SCK PD20 SCL
	#define TARGET_TWI_TWCK_PIN		(GPIOD->DATA)
	#define TARGET_TWI_TWCK_PORT_C(v) do { gpioX_setopendrain(GPIOD, (v), 0 * (v)); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do { gpioX_setopendrain(GPIOD, (v), 1 * (v)); } while (0)

	#define TARGET_TWI_TWD		(UINT32_C(1) << 21)		// TWI2-SDA PD21 SDA
	#define TARGET_TWI_TWD_PIN		(GPIOD->DATA)
	#define TARGET_TWI_TWD_PORT_C(v) do { gpioX_setopendrain(GPIOD, (v), 0 * (v)); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v) do { gpioX_setopendrain(GPIOD, (v), 1 * (v)); } while (0)

	// Инициализация битов портов ввода-вывода для программной реализации I2C
	#define	TWISOFT_INITIALIZE() do { \
			arm_hardware_piod_opendrain(TARGET_TWI_TWCK, TARGET_TWI_TWCK); /* SCL */ \
			arm_hardware_piod_opendrain(TARGET_TWI_TWD, TARGET_TWI_TWD);  	/* SDA */ \
		} while (0)
	#define	TWISOFT_DEINITIALIZE() do { \
			arm_hardware_piod_inputs(TARGET_TWI_TWCK); 	/* TWI2-SCK PD20 SCL */ \
			arm_hardware_piod_inputs(TARGET_TWI_TWD);	/* TWI2-SDA PD21 SDA */ \
		} while (0)
	// Инициализация битов портов ввода-вывода для аппаратной реализации I2C
	// присоединение выводов к периферийному устройству
	#define	TWIHARD_INITIALIZE() do { \
		arm_hardware_piod_altfn2(TARGET_TWI_TWCK, GPIO_CFG_AF3);	/* TWI2-SCK PD20 SCL */ \
		arm_hardware_piod_altfn2(TARGET_TWI_TWD, GPIO_CFG_AF3);		/* TWI2-SDA PD21 SDA */ \
		arm_hardware_piod_updown(TARGET_TWI_TWCK, TARGET_TWI_TWCK, 0); \
		arm_hardware_piod_updown(TARGET_TWI_TWD, TARGET_TWI_TWD, 0); \
		} while (0)
	#define	TWIHARD_IX 2	/* 0 - TWI0, 1: TWI1... */
	#define	TWIHARD_PTR TWI2	/* 0 - TWI0, 1: TWI1... */
	#define	TWIHARD_FREQ (allwnrt113_get_twi_freq()) // APBS2_CLK allwnr_t507_get_apb2_freq() or allwnr_t507_get_apbs2_freq()


#elif WITHTWISW || WITHTWIHW
	// TWI1-SCK PG8 SCL
	// TWI1-SDA PG9 SDA
	#define TARGET_TWI_TWCK		(UINT32_C(1) << 8)		// TWI1-SCK PG8 SCL
	#define TARGET_TWI_TWCK_PIN		(GPIOG->DATA)
	#define TARGET_TWI_TWCK_PORT_C(v) do { gpioX_setopendrain(GPIOG, (v), 0 * (v)); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do { gpioX_setopendrain(GPIOG, (v), 1 * (v)); } while (0)

	#define TARGET_TWI_TWD		(UINT32_C(1) << 9)		// TWI1-SDA PG9 SDA
	#define TARGET_TWI_TWD_PIN		(GPIOG->DATA)
	#define TARGET_TWI_TWD_PORT_C(v) do { gpioX_setopendrain(GPIOG, (v), 0 * (v)); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v) do { gpioX_setopendrain(GPIOG, (v), 1 * (v)); } while (0)

	// Инициализация битов портов ввода-вывода для программной реализации I2C
	#define	TWISOFT_INITIALIZE() do { \
			arm_hardware_piog_opendrain(TARGET_TWI_TWCK, TARGET_TWI_TWCK); /* SCL */ \
			arm_hardware_piog_opendrain(TARGET_TWI_TWD, TARGET_TWI_TWD);  	/* SDA */ \
		} while (0) 
	#define	TWISOFT_DEINITIALIZE() do { \
			arm_hardware_piog_inputs(TARGET_TWI_TWCK); 	/* TWI1-SCK PG8 SCL */ \
			arm_hardware_piog_inputs(TARGET_TWI_TWD);	/* TWI1-SDA PG9 SDA */ \
		} while (0)
	// Инициализация битов портов ввода-вывода для аппаратной реализации I2C
	// присоединение выводов к периферийному устройству
	#define	TWIHARD_INITIALIZE() do { \
		arm_hardware_piog_altfn2(TARGET_TWI_TWCK, GPIO_CFG_AF3);	/* TWI1-SCK PG8 SCL */ \
		arm_hardware_piog_altfn2(TARGET_TWI_TWD, GPIO_CFG_AF3);		/* TWI1-SDA PG9 SDA */ \
		arm_hardware_piog_updown(TARGET_TWI_TWCK, TARGET_TWI_TWCK, 0); \
		arm_hardware_piog_updown(TARGET_TWI_TWD, TARGET_TWI_TWD, 0); \
		} while (0) 
	#define	TWIHARD_IX 1	/* 0 - TWI0, 1: TWI1... */
	#define	TWIHARD_PTR TWI1	/* 0 - TWI0, 1: TWI1... */
	#define	TWIHARD_FREQ (allwnrt113_get_twi_freq()) // APBS2_CLK allwnr_t507_get_apb2_freq() or allwnr_t507_get_apbs2_freq()

#endif /* WITHTWISW || WITHTWIHW */

#if WITHFPGAWAIT_AS || WITHFPGALOAD_PS

	/* outputs */
	#define FPGA_NCONFIG_PORT_S(v)	do { gpioX_setstate(GPIOC, (v), !! (1) * (v)); } while (0)
	#define FPGA_NCONFIG_PORT_C(v)	do { gpioX_setstate(GPIOC, (v), !! (0) * (v)); } while (0)
	#define FPGA_NCONFIG_BIT		(0 * UINT32_C(1) << 11)	/* PC11 bit connected to nCONFIG pin ALTERA FPGA */

	/* inputs */
	#define FPGA_CONF_DONE_INPUT	(GPIOC->DATA)
	#define FPGA_CONF_DONE_BIT		(0 * UINT32_C(1) << 10)	/* PC10 bit connected to CONF_DONE pin ALTERA FPGA */

	#define FPGA_NSTATUS_INPUT		(GPIOC->DATA)
	#define FPGA_NSTATUS_BIT		(0 * UINT32_C(1) << 9)	/* PC9 bit connected to NSTATUS pin ALTERA FPGA */

	#define FPGA_INIT_DONE_INPUT	(GPIOC->DATA)
	#define FPGA_INIT_DONE_BIT		(0 * UINT32_C(1) << 12)	/* PC12 bit connected to INIT_DONE pin ALTERA FPGA */

	/* Инициадизация выводов GPIO процессора для получения состояния и управлением загрузкой FPGA */
	#define HARDWARE_FPGA_LOADER_INITIALIZE() do { \
			arm_hardware_pioc_outputs(FPGA_NCONFIG_BIT, FPGA_NCONFIG_BIT); \
			arm_hardware_pioc_inputs(FPGA_NSTATUS_BIT); \
			arm_hardware_pioc_inputs(FPGA_CONF_DONE_BIT); \
			arm_hardware_pioc_inputs(FPGA_INIT_DONE_BIT); \
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

	// FPGA PIN_23
	#define TARGET_FPGA_FIR_CS_PORT_C(v)	do { gpioX_setstate(GPIOC, (v), !! (0) * (v)); } while (0) // do { GPIOC->BSRR = BSRR_C(v); (void) GPIOC->BSRR; } while (0)
	#define TARGET_FPGA_FIR_CS_PORT_S(v)	do { gpioX_setstate(GPIOC, (v), !! (1) * (v)); } while (0) // do { GPIOC->BSRR = BSRR_S(v); (void) GPIOC->BSRR; } while (0)
	#define TARGET_FPGA_FIR_CS_BIT 0//(UINT32_C(1) << 13)	/* PC13 - fir CS ~FPGA_FIR_CLK */

	// FPGA PIN_8
	#define TARGET_FPGA_FIR1_WE_PORT_C(v)	do { gpioX_setstate(GPIOD, (v), !! (0) * (v)); } while (0) // do { GPIOD->BSRR = BSRR_C(v); (void) GPIOD->BSRR; } while (0)
	#define TARGET_FPGA_FIR1_WE_PORT_S(v)	do { gpioX_setstate(GPIOD, (v), !! (1) * (v)); } while (0) // do { GPIOD->BSRR = BSRR_S(v); (void) GPIOD->BSRR; } while (0)
	#define TARGET_FPGA_FIR1_WE_BIT 0//(UINT32_C(1) << 1)	/* PD1 - fir1 WE */

	// FPGA PIN_7
	#define TARGET_FPGA_FIR2_WE_PORT_C(v)	do { gpioX_setstate(GPIOD, (v), !! (0) * (v)); } while (0) // do { GPIOD->BSRR = BSRR_C(v); (void) GPIOD->BSRR; } while (0)
	#define TARGET_FPGA_FIR2_WE_PORT_S(v)	do { gpioX_setstate(GPIOD, (v), !! (1) * (v)); } while (0) // do { GPIOD->BSRR = BSRR_S(v); (void) GPIOD->BSRR; } while (0)
	#define TARGET_FPGA_FIR2_WE_BIT 0//(UINT32_C(1) << 0)	/* PD0 - fir2 WE */

	#define TARGET_FPGA_FIR_INITIALIZE() do { \
			arm_hardware_piod_outputs2m(TARGET_FPGA_FIR1_WE_BIT, TARGET_FPGA_FIR1_WE_BIT); \
			arm_hardware_piod_outputs2m(TARGET_FPGA_FIR2_WE_BIT, TARGET_FPGA_FIR2_WE_BIT); \
			arm_hardware_pioc_outputs2m(TARGET_FPGA_FIR_CS_BIT, TARGET_FPGA_FIR_CS_BIT); \
		} while (0)
#endif /* WITHDSPEXTFIR */

#if 0
	/* получение состояния переполнения АЦП */
	#define TARGET_FPGA_OVF_INPUT		(GPIOC->DATA)
	#define TARGET_FPGA_OVF_BIT			(UINT32_C(1) << 8)	// PC8
	#define TARGET_FPGA_OVF_GET			((TARGET_FPGA_OVF_INPUT & TARGET_FPGA_OVF_BIT) == 0)	// 1 - overflow active
	#define TARGET_FPGA_OVF_INITIALIZE() do { \
				arm_hardware_pioc_inputs(TARGET_FPGA_OVF_BIT); \
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
	#define TARGET_USBFS_VBUSON_PORT_C(v)	do { } while (0) //do { GPIOD->BSRR = BSRR_C(v); (void) GPIOD->BSRR; } while (0)
	#define TARGET_USBFS_VBUSON_PORT_S(v)	do { } while (0) //do { GPIOD->BSRR = BSRR_S(v); (void) GPIOD->BSRR; } while (0)
	#define TARGET_USBFS_VBUSON_BIT (0 * UINT32_C(1) << 2)	// PD2 - нулём включение питания для device
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
		arm_hardware_piod_outputs(TARGET_USBFS_VBUSON_BIT, TARGET_USBFS_VBUSON_BIT); /* PD2 */ \
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

#if 0
	/* установка яркости и включение/выключение преобразователя подсветки */
	/* Яркость Управлятся через PWM */
	#define HARDWARE_BL_PWMCH 7	/* PWM7 */
	#define HARDWARE_BL_FREQ 	10000	/* Частота PWM управления подсветкой */
	#define	HARDWARE_BL_INITIALIZE() do { \
		const portholder_t ENmask = (UINT32_C(1) << 22); /* PD22 (PWM7, Alt FN 5) */ \
		hardware_dcdcfreq_pwm_initialize(HARDWARE_BL_PWMCH); \
		arm_hardware_piod_altfn2(ENmask, GPIO_CFG_AF5); /* PD22 - PWM7 */ \
	} while (0)
	// en: 0/1, level=WITHLCDBACKLIGHTMIN..WITHLCDBACKLIGHTMAX
	// level=WITHLCDBACKLIGHTMIN не приводит к выключениию подсветки
	#define HARDWARE_BL_SET(en, level) do { \
		hardware_bl_pwm_set_duty(HARDWARE_BL_PWMCH, HARDWARE_BL_FREQ, !! en * ((level) - WITHLCDBACKLIGHTMIN + 1) * 100 / (WITHLCDBACKLIGHTMAX - WITHLCDBACKLIGHTMIN + 1)); \
	} while (0)
#endif

#if WITHLTDCHW

	// Sochip_VE_S3_Datasheet_V1.0.pdf
	// TCON0_TRM_CTL_REG offset 0x0010
	// User manual:
	// LCD FRM Control Register (Default Value: 0x0000_0000)
	//#define TCON_FRM_MODE_VAL 0//((UINT32_C(1) << 31) | (UINT32_C(1) << 6) | (UINT32_C(0) << 5) | (UINT32_C(1) << 4))	// 16 bit panel connected
	#define TCON_FRM_MODE_VAL 0//((UINT32_C(1) << 31) | (UINT32_C(0) << 6) | (UINT32_C(0) << 5)| (UINT32_C(0) << 4))	// 18 bit panel connected
	//#define TCON_FRM_MODE_VAL 0	// 24 bit panel

	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_INITIALIZE(demode) do { \
		const portholder_t VSmask = (UINT32_C(1) << 21); 	/* PD21 LCD_VSYNC */ \
		const portholder_t HSmask = (UINT32_C(1) << 20); 	/* PD20 LCD_HSYNC */ \
		const portholder_t DEmask = (UINT32_C(1) << 19); 	/* PD19 LCD_DE */ \
		/* synchro signals - sync mode */ \
		arm_hardware_piod_outputs(((demode) == 0) * DEmask, 0 * DEmask); /* PD19 LCD_DE */ \
		arm_hardware_piod_altfn50(((demode) == 0) * VSmask, GPIO_CFG_AF2); /* PD21 LCD_VSYNC */ \
		arm_hardware_piod_altfn50(((demode) == 0) * HSmask, GPIO_CFG_AF2); /* PD20 LCD_HSYNC */ \
		/* synchro signals - DE mode */ \
		arm_hardware_piod_altfn50(((demode) != 0) * DEmask, GPIO_CFG_AF2); /* PD19 LCD_DE */ \
		arm_hardware_piod_outputs(((demode) != 0) * VSmask, 1 * VSmask); /* PD21 LCD_VSYNC */ \
		arm_hardware_piod_outputs(((demode) != 0) * HSmask, 1 * HSmask); /* PD20 LCD_HSYNC */ \
		/* pixel clock */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 18, GPIO_CFG_AF2); /* PD18 LCD_CLK */ \
		/* RED */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 12, GPIO_CFG_AF2); /* R2 PD12 LCD_D18 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 13, GPIO_CFG_AF2); /* R3 PD13 LCD_D19 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 14, GPIO_CFG_AF2); /* R4 PD14 LCD_D20 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 15, GPIO_CFG_AF2); /* R5 PD15 LCD_D21 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 16, GPIO_CFG_AF2); /* R6 PD16 LCD_D22 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 17, GPIO_CFG_AF2); /* R7 PD17 LCD_D23 */ \
		/* GREEN */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 6, GPIO_CFG_AF2); 	/* G2 PD6 LCD_D10 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 7, GPIO_CFG_AF2); 	/* G3 PD7 LCD_D11 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 8, GPIO_CFG_AF2); 	/* G4 PD8 LCD_D12 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 9, GPIO_CFG_AF2); 	/* G5 PD9 LCD_D13 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 10, GPIO_CFG_AF2); /* G6 PD10 LCD_D14 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 11, GPIO_CFG_AF2); /* G7 PD11 LCD_D15 */ \
		/* BLUE  */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 0, GPIO_CFG_AF2); 	/* B2 PD0 LCD_D2 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 1, GPIO_CFG_AF2); 	/* B3 PD1 LCD_D3 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 2, GPIO_CFG_AF2); 	/* B4 PD2 LCD_D4 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 3, GPIO_CFG_AF2); 	/* B5 PD3 LCD_D5 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 4, GPIO_CFG_AF2); 	/* B6 PD4 LCD_D6 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 5, GPIO_CFG_AF2); 	/* B7 PD5 LCD_D7 */ \
	} while (0)

	/* управление состоянием сигнала DISP панели */
	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_SET_DISP(state) do { \
		const portholder_t VSmask = (UINT32_C(1) << 21); 	/* PD21 LCD_VSYNCC */ \
		const portholder_t HSmask = (UINT32_C(1) << 20); 	/* PD20 LCD_HSYNC */ \
		const portholder_t DEmask = (UINT32_C(1) << 19); 	/* PD19 LCD_DE */ \
		/* while ((GPIOA->DATA & VSmask) != 0) ; */ /* схема синхронизации стоит на плате дисплея. дождаться 0 */ \
		/* while ((GPIOA->DATA & VSmask) == 0) ; */ /* дождаться 1 */ \
		arm_hardware_piod_outputs(0*DEmask, ((state) != 0) * DEmask); /* DE=DISP, pin 31 - можно менять только при VSYNC=1 */ \
	} while (0)

	#define LCD_LVDS_IF_REG_VALUE ( \
		1 * (UINT32_C(1) << 31) |	/* LCD_LVDS_EN */ \
		0 * (UINT32_C(1) << 30) |	/* LCD_LVDS_LINK: 0: single link */ \
		0 * (UINT32_C(1) << 27) |	/* LCD_LVDS_MODE 1: JEIDA mode (0 for THC63LVDF84B converter) */ \
		0 * (UINT32_C(1) << 26) |	/* LCD_LVDS_BITWIDTH 0: 24-bit, 1: 18-bit */ \
		1 * (UINT32_C(1) << 20) |	/* LCD_LVDS_CLK_SEL 1: LCD CLK */ \
		0 * (UINT32_C(1) << 25) |		/* LCD_LVDS_DEBUG_EN */ \
		0 * (UINT32_C(1) << 24) |		/* LCD_LVDS_DEBUG_MODE */ \
		0 * (UINT32_C(1) << 4) |				/* LCD_LVDS_CLK_POL: 1: reverse, 0: normal = document error */ \
		0 * (UINT32_C(0x0F) << 0) |		/* LCD_LVDS_DATA_POL: 1: reverse, 0: normal = document error*/ \
		0)

	#define HARDWARE_LVDS_INITIALIZE() do { \
		arm_hardware_piod_altfn50(UINT32_C(1) << 0, GPIO_CFG_AF3); 	/* PD0 LVDS0_V0P */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 1, GPIO_CFG_AF3); 	/* PD1 LVDS0_V0N */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 2, GPIO_CFG_AF3); 	/* PD2 LVDS0_V1P */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 3, GPIO_CFG_AF3); 	/* PD3 LVDS0_V1N */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 4, GPIO_CFG_AF3); 	/* PD4 LVDS0_V2P */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 5, GPIO_CFG_AF3); 	/* PD5 LVDS0_V2N */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 6, GPIO_CFG_AF3); 	/* PD6 LVDS0_CKP */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 7, GPIO_CFG_AF3); 	/* PD7 LVDS0_CKN */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 8, GPIO_CFG_AF3); 	/* PD8 LVDS0_V3P */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 9, GPIO_CFG_AF3); 	/* PD9 LVDS0_V3N */ \
	} while (0)

	// PD0..PD9: mipi-dsi connect
	// PD20 - TWI2-SCK
	// PD21 - TWI2-SDA
	#define HARDWARE_MIPIDSI_INITIALIZE() do { \
		arm_hardware_piod_altfn50(UINT32_C(1) << 0, GPIO_CFG_AF4); 	/* PD0 DSI-D0P */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 1, GPIO_CFG_AF4); 	/* PD1 DSI-D0N */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 2, GPIO_CFG_AF4); 	/* PD2 DSI-D1P */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 3, GPIO_CFG_AF4); 	/* PD3 DSI-D1N */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 4, GPIO_CFG_AF4); 	/* PD4 DSI-CKP */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 5, GPIO_CFG_AF4); 	/* PD5 DSI-CKN */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 6, GPIO_CFG_AF4); 	/* PD6 DSI-D2P */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 7, GPIO_CFG_AF4); 	/* PD7 DSI-D2N */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 8, GPIO_CFG_AF4); 	/* PD8 DSI-D3P */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 9, GPIO_CFG_AF4); 	/* PD9 DSI-D3N */ \
		/* I2C control */ \
		/*arm_hardware_piod_altfn2(UINT32_C(1) << 20, GPIO_CFG_AF3);*/	/* PD20 TWI2-SCK SCL */ \
		/*arm_hardware_piod_altfn2(UINT32_C(1) << 21, GPIO_CFG_AF3);	*//* PD21 TWI2-SDA SDA */ \
	} while (0)

#endif /* WITHLTDCHW */

	#define	TCONLCD_IX 0	/* 0 - TCON_LCD0, 1: TCON_LCD1 */
	#define	TCONLCD_PTR TCON_LCD0	/* 0 - TCON_LCD0, 1: TCON_LCD1 */
	#define	TCONLCD_CCU_CLK_REG (CCU->TCONLCD_CLK_REG)	/* 0 - TCON_LCD0, 1: TCON_LCD1 */
	#define BOARD_TCONLCDFREQ (allwnrt113_get_tconlcd_freq())
	#define TCONLCD_LVDSIX 0	/* 0 -LVDS0 */


	#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811)

		//	tsc interrupt XS26, pin 08
		//	tsc/LCD reset, XS26, pin 22
		//	tsc SCL: XS26, pin 01
		//	tsc SDA: XS26, pin 02

		void stmpe811_interrupt_handler(void * ctx);

		#define BOARD_STMPE811_INT_PIN (UINT32_C(1) << 12)		/* PA12 : tsc interrupt XS26, pin 08 */

		#define BOARD_STMPE811_INT_CONNECT() do { \
			arm_hardware_pioa_inputs(BOARD_STMPE811_INT_PIN); \
			arm_hardware_pioa_updown(BOARD_STMPE811_INT_PIN, BOARD_STMPE811_INT_PIN, 0); \
			arm_hardware_pioa_onchangeinterrupt(BOARD_STMPE811_INT_PIN, 1 * BOARD_STMPE811_INT_PIN, 0 * BOARD_STMPE811_INT_PIN, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM); \
		} while (0)
#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811) */

	#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_GT911)

		//	tsc interrupt XS26, pin 08
		//	tsc/LCD reset, XS26, pin 22
		//	tsc SCL: XS26, pin 01
		//	tsc SDA: XS26, pin 02

		void gt911_interrupt_handler(void * ctx);

		#define BOARD_GT911_INT_PIN (UINT32_C(1) << 12)		/* PA12 : tsc interrupt XS26, pin 08 */
		#define BOARD_GT911_RESET_PIN (UINT32_C(1) << 4)	/* PD4 : tsc/LCD reset, XS26, pin 22 */

		#define BOARD_GT911_RESET_SET(v) do { if (v) GPIOD->BSRR = BSRR_S(BOARD_GT911_RESET_PIN); else GPIOD->BSRR = BSRR_C(BOARD_GT911_RESET_PIN); (void) GPIOD->BSRR; } while (0)
		#define BOARD_GT911_INT_SET(v) do { if (v) GPIOA->BSRR = BSRR_S(BOARD_GT911_INT_PIN); else GPIOA->BSRR = BSRR_C(BOARD_GT911_INT_PIN); (void) GPIOA->BSRR; } while (0)

		#define BOARD_GT911_RESET_INITIO_1() do { \
			arm_hardware_pioa_outputs2m(BOARD_GT911_INT_PIN, 1* BOARD_GT911_INT_PIN); \
			arm_hardware_piod_outputs2m(BOARD_GT911_RESET_PIN, 1 * BOARD_GT911_RESET_PIN); \
			 local_delay_ms(200);  \
		} while (0)

		#define BOARD_GT911_RESET_INITIO_2() do { \
			arm_hardware_pioa_inputs(BOARD_GT911_INT_PIN); \
			arm_hardware_pioa_updown(BOARD_GT911_INT_PIN, BOARD_GT911_INT_PIN, 0); \
		} while (0)

		#define BOARD_GT911_INT_CONNECT() do { \
			arm_hardware_pioa_inputs(BOARD_GT911_INT_PIN); \
			arm_hardware_pioa_updown(BOARD_GT911_INT_PIN, BOARD_GT911_INT_PIN, 0); \
			/*arm_hardware_pioa_onchangeinterrupt(BOARD_GT911_INT_PIN, 1 * BOARD_GT911_INT_PIN, 0 * BOARD_GT911_INT_PIN, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM); */ \
		} while (0)
		//gt911_interrupt_handler

	#endif

	#define BOARD_BLINK_BIT (UINT32_C(1) << 22)	// PD22 (PWM7) - led on mq-r board (from VCC33

#if 1
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
	#define BOARD_USERBOOT_BIT	(UINT32_C(1) << 1)	/* PB1: ~USER_BOOT */
	#define BOARD_IS_USERBOOT() 0//(((GPIOB->DATA) & BOARD_USERBOOT_BIT) == 0 || ((GPIOE->DATA) & TARGET_ENC2BTN_BIT) == 0)
	#define BOARD_USERBOOT_INITIALIZE() do { \
			/*arm_hardware_piob_inputs(BOARD_USERBOOT_BIT); *//* set as input with pull-up */ \
			/*arm_hardware_pioe_inputs(TARGET_ENC2BTN_BIT);*/ /* set as input with pull-up */ \
		} while (0)
	#define BOARD_USERBOOT_INITIALIZE() do { \
		} while (0)

	/* макроопределение, которое должно включить в себя все инициализации */
	#define	HARDWARE_INITIALIZE() do { \
		BOARD_BLINK_INITIALIZE(); \
		/*HARDWARE_KBD_INITIALIZE(); */\
		/*HARDWARE_DAC_INITIALIZE(); */\
		/*HARDWARE_BL_INITIALIZE(); */\
		HARDWARE_DCDC_INITIALIZE(); \
		TXDISABLE_INITIALIZE(); \
		TUNE_INITIALIZE(); \
		BOARD_USERBOOT_INITIALIZE(); \
		/*USBD_EHCI_INITIALIZE(); */\
	} while (0)

	// TUSB parameters
	#define TUP_DCD_ENDPOINT_MAX    6

#endif /* ARM_ALLW_F1333_CPUSTYLE_MANGO_PI_H_INCLUDED */
