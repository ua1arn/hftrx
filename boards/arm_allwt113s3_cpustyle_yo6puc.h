/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef ARM_ALWT113S3_CPUSTYLE_YO6POC_H_INCLUDED
#define ARM_ALWT113S3_CPUSTYLE_YO6POC_H_INCLUDED 1

#define WITHSPI16BIT	1	/* возможно использование 16-ти битных слов при обмене по SPI */
#define WITHSPI32BIT	1	/* возможно использование 32-ти битных слов при обмене по SPI */
#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
//#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
//#define WITHSPISW 	1	/* Использование программного управления SPI. */

//#define WIHSPIDFSW	1	/* программное обслуживание DATA FLASH */
//#define WIHSPIDFOVERSPI 1	/* Для работы используется один из обычных каналов SPI */
#define WIHSPIDFHW		1	/* аппаратное обслуживание DATA FLASH */
//#define WIHSPIDFHW2BIT	1	/* аппаратное обслуживание DATA FLASH с поддержкой QSPI подключения по 2-м проводам */
//#define WIHSPIDFHW4BIT	1	/* аппаратное обслуживание DATA FLASH с поддержкой QSPI подключения по 4-м проводам */

//#define WITHDMA2DHW		1	/* Использование DMA2D для формирования изображений	- у STM32MP1 его нет */

//#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
//#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */
//#define WITHETHHW 1	/* Hardware Ethernet controller */

#if WITHDEBUG
	#define WITHUART0HW	1	/* PE2 PE3 Используется периферийный контроллер последовательного порта UART0 */
	//#define WITHUART0HW_FIFO	1	/* испольование FIFO */
#endif /* WITHDEBUG */

//#define WITHCAT_UART1		1
#define WITHDEBUG_UART0	1


#if WITHISBOOTLOADER

	//#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
	//#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */

	#define WITHSDRAMHW	1		/* В процессоре есть внешняя память */
	//#define WITHSDRAM_PMC1	1	/* power management chip */

	//#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
	//#define WITHGPUHW	1	/* Graphic processor unit */
	
	#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */
	#define USBPHYC_MISC_SWITHOST_VAL 0		// 0 or 1 - value for USBPHYC_MISC_SWITHOST field. 0: Select OTG controller for 2nd PHY port, 1: Select Host controller for 2nd PHY port
	#define USBPHYC_MISC_PPCKDIS_VAL 0x00

	#define WITHUSBHW_DEVICE	USBOTG0	/* на этом устройстве поддерживается функциональность DEVICE	*/
	#define WITHUSBDEV_VBUSSENSE	1		/* используется предопределенный вывод OTG_VBUS */
	#define WITHUSBDEV_HSDESC	1			/* Требуется формировать дескрипторы как для HIGH SPEED */
	//#define WITHUSBDEV_HIGHSPEEDULPI	1
	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USBH_HS_DP & USBH_HS_DM
	//#define WITHUSBDEV_DMAENABLE 1

	//#define WITHUSBHW_HOST		USB_OTG_HS
	#define WITHUSBHOST_HIGHSPEEDPHYC	1	// UTMI -> USB_DP2 & USB_DM2
	#define WITHUSBHOST_DMAENABLE 1

	
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
	//#define WITHUSBDMTP	1	/* MTP USB Device */
	//#define WITHUSBDMSC	1	/* MSC USB device */

#else /* WITHISBOOTLOADER */

	#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
	//#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */

	//#define WITHDCDCFREQCTL	1		// Имеется управление частотой преобразователей блока питания
	//#define WITHBLPWMCTL	1		// Имеется управление яркостью подсветки дисплея через PWM

	#if WITHINTEGRATEDDSP

		//#define WITHFPGAPIPE_CODEC1 1	/* Интерфейс к FPGA, транзитом в аудио кодек через I2S0 */
		#define WITHFPGAPIPE_RTS96 WITHRTS96	/* в том же фрейме идут квадратуры RTS96 */
		#define WITHFPGAPIPE_RTS192 WITHRTS192	/* в том же фрейме идут квадратуры RTS192 */
		#define WITHFPGAPIPE_NCORX0 1	/* управление частотой приемника 1 */
		#define WITHFPGAPIPE_NCORX1 1	/* управление частотой приемника 2 */
		#define WITHFPGAPIPE_NCORTS 1	/* управление частотой приемника панорамы */

		#define WITHI2S1HW	1	/* Использование I2S1 - аудиокодек на I2S */
		#define WITHI2S2HW	1	/* Использование I2S2 - FPGA или IF codec	*/
		#define WITHCODEC1_I2S1_DUPLEX_SLAVE	1		/* Обмен с аудиокодеком через I2S1 */
		#define WITHFPGAIF_I2S2_DUPLEX_SLAVE	1		/* Обмен с FPGA через I2S2 */
		//#define WITHCODEC1_I2S1_DUPLEX_MASTER	1		/* Обмен с аудиокодеком через I2S1 */
		//#define WITHFPGAIF_I2S2_DUPLEX_MASTER	1		/* Обмен с FPGA через I2S2 */
		//#define WITHCODEC1_WHBLOCK_DUPLEX_MASTER	1	/* встороенный в процессор кодек */
	#endif /* WITHINTEGRATEDDSP */

	#if ! LCDMODE_DUMMY
		#define WITHMDMAHW		1	/* Использование G2D для формирования изображений */
		#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
		//#define WITHGPUHW	1	/* Graphic processor unit */
	#endif

	//#define WITHCPUDACHW	1	/* использование встроенного в процессор DAC */
	#define WITHCPUADCHW 	1	/* использование встроенного в процессор ADC */

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

	

	#define WITHTINYUSB 1
	
	#if WITHTINYUSB
		#define BOARD_TUH_RHPORT 1
		#define CFG_TUH_ENABLED 1
		//#define TUP_USBIP_OHCI 1
		#define TUP_USBIP_EHCI 1
	#endif /* WITHTINYUSB */

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

	#if WITHINTEGRATEDDSP
		#if WITHUSBDEV_HSDESC

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
			#define UACIN_AUDIO48_SAMPLEBYTES	3	/* должны быть 2, 3 или 4 */
			#define UACIN_RTS96_SAMPLEBYTES		3	/* должны быть 2, 3 или 4 */
			#define UACIN_RTS192_SAMPLEBYTES	3	/* должны быть 2, 3 или 4 */

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

	#define WITHUSBCDCACM		1	/* ACM использовать виртуальный последовательный порт на USB соединении */
	#define WITHUSBCDCACM_N	1	/* количество виртуальных последовательных портов */

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

#define LS020_RS				0//(UINT32_C(1) << 11)			// PE11 signal
#define LS020_RS_SET(v) 		do { gpioX_setstate(GPIOE, LS020_RS, !! (v) * LS020_RS); } while (0)

#define LS020_RS_INITIALIZE() do { \
		arm_hardware_pioe_outputs2m(LS020_RS, LS020_RS); /* PE11 */ \
	} while (0)

#define LS020_RESET				(UINT32_C(1) << 22)			// PD22 TFT_RST signal
#define LS020_RESET_SET(v) 		do { gpioX_setstate(GPIOD, LS020_RESET, !! (v) * LS020_RESET); } while (0)

#define LS020_RESET_INITIALIZE() do { \
		arm_hardware_piod_outputs2m(LS020_RESET, LS020_RESET); /* PD22 */ \
	} while (0)


#if WITHENCODER

	// Выводы подключения енкодера #1
	#define ENCODER_INPUT_PORT	(gpioX_getinputs(GPIOE))
	#define ENCODER_BITA		(UINT32_C(1) << 8)		// PE8
	#define ENCODER_BITB		(UINT32_C(1) << 7)		// PE7

	/* Определения масок битов для формирования обработчиков прерываний в нужном GPIO */
	#define BOARD_GPIOE_ENCODER_BITS		(ENCODER_BITA | ENCODER_BITB)

	#define ENCODER_INITIALIZE() do { \
		static einthandler_t h1; \
		arm_hardware_pioe_altfn2(BOARD_GPIOE_ENCODER_BITS, GPIO_CFG_EINT); \
		arm_hardware_pioe_updown(BOARD_GPIOE_ENCODER_BITS, BOARD_GPIOE_ENCODER_BITS, 0); \
		einthandler_initialize(& h1, BOARD_GPIOE_ENCODER_BITS, spool_encinterrupts, & encoder1); \
		arm_hardware_pioe_onchangeinterrupt(BOARD_GPIOE_ENCODER_BITS, BOARD_GPIOE_ENCODER_BITS, BOARD_GPIOE_ENCODER_BITS, ENCODER_PRIORITY, ENCODER_TARGETCPU, & h1); \
	} while (0)

	#define ENCODER_BITS_GET() (((ENCODER_INPUT_PORT & ENCODER_BITA) != 0) * 2 + ((ENCODER_INPUT_PORT & ENCODER_BITB) != 0))

#endif

	// Инициализируются I2S1 в дуплексном режиме.
	// аудиокодек
	#define I2S1HW_INITIALIZE(master) do { \
		arm_hardware_piog_altfn20(UINT32_C(1) << 11, GPIO_CFG_AF2); /* PG15 I2S1-MCLK co codec - added */ \
		arm_hardware_piog_altfn20(UINT32_C(1) << 12, GPIO_CFG_AF2); /* PG12 I2S1-LRCK */ \
		arm_hardware_piog_altfn20(UINT32_C(1) << 13, GPIO_CFG_AF2); /* PG13 I2S1-BCLK */ \
		arm_hardware_piog_altfn20(UINT32_C(1) << 15, GPIO_CFG_AF5); /* PG14 I2S1-DIN1 from codec - modified */ \
		arm_hardware_piog_altfn20(UINT32_C(1) << 14, GPIO_CFG_AF5); /* PG15 I2S1-DOUT1 co codec - modified */ \
	} while (0)
	#define HARDWARE_I2S1HW_DIN 1	/* DIN1 used - modified */
	#define HARDWARE_I2S1HW_DOUT 1	/* DOUT1 used - modified */
	// Инициализируются I2S2 в дуплексном режиме.
	// FPGA или IF codec
	#define I2S2HW_INITIALIZE(master) do { \
		arm_hardware_piob_altfn20(UINT32_C(1) << 6,	GPIO_CFG_AF3); /* PB6 I2S2-LRCK	*/ \
		arm_hardware_piob_altfn20(UINT32_C(1) << 5,	GPIO_CFG_AF3); /* PB5 I2S2-BCLK	*/ \
		arm_hardware_piob_altfn20(UINT32_C(1) << 4,	GPIO_CFG_AF5); /* PB4 I2S2-DIN1 from FPGA  - modified */ \
		arm_hardware_piob_altfn20(UINT32_C(1) << 2,	GPIO_CFG_AF5); /* PB2 I2S2-DIN2 from FPGA  - modified - can be 2 inputs at the same time ?*/ \
		arm_hardware_piob_altfn20(UINT32_C(1) << 3,	GPIO_CFG_AF3); /* PB3 I2S2-DOUT1 to FPGA - modified */ \
		arm_hardware_piob_altfn20(UINT32_C(1) << 7,	GPIO_CFG_AF3); /* PB7 I2S2-MCLK to FPGA - modified - not used inside FPGA, but connected - the FPGA makes local MCLK in sync with ARM CLK/PLLs */ \
	} while (0)
	#define HARDWARE_I2S2HW_DIN 1	/* DIN1,2 used - modified - see WITHFPGAIF_USE_I2S_DIN12 */
	#define HARDWARE_I2S2HW_DOUT 1	/* DOUT1 used - modified */

/* Распределение битов в ARM контроллерах */

#if (WITHCAT && WITHCAT_UART2)
	// CAT data lites
	// RXD at PA10, TXD at PA9

	// CAT control lines
	//#define FROMCAT_TARGET_PIN_RTS		gpioX_getinputs(GPIOA)
	//#define FROMCAT_BIT_RTS				(UINT32_C(1) << 11)	/* PA11 сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	//#define FROMCAT_TARGET_PIN_DTR		gpioX_getinputs(GPIOA)
	//#define FROMCAT_BIT_DTR				(UINT32_C(1) << 12)	/* PA12 сигнал DTR от FT232RL	*/

	/* манипуляция от порта RS-232 */
	#define FROMCAT_DTR_INITIALIZE() do { \
		} while (0)

	/* переход на передачу от порта RS-232 */
	#define FROMCAT_RTS_INITIALIZE() do { \
		} while (0)

#endif /* (WITHCAT && WITHCAT_UART2) */

#if (WITHCAT && WITHCAT_CDC)

	// CAT data lites
	// RXD at PA10, TXD at PA9

	// CAT control lines
	//#define FROMCAT_TARGET_PIN_RTS		gpioX_getinputs(GPIOA) // was PINA
	//#define FROMCAT_BIT_RTS				(UINT32_C(1) << 11)	/* сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	//#define FROMCAT_TARGET_PIN_DTR		gpioX_getinputs(GPIOA) // was PINA
	//#define FROMCAT_BIT_DTR				(UINT32_C(1) << 12)	/* сигнал DTR от FT232RL	*/

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
			arm_hardware_piod_altfn50(UINT32_C(1) << 2, AF_SDIO);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_altfn50(UINT32_C(1) << 12, AF_SDIO);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_altfn50(UINT32_C(1) << 8, AF_SDIO);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_altfn50(UINT32_C(1) << 9, AF_SDIO);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_altfn50(UINT32_C(1) << 10, AF_SDIO);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_altfn50(UINT32_C(1) << 11, AF_SDIO);	/* PC11 - SDIO_D3	*/ \
		} while (0)
		/* отключить процессор от SD карты - чтобы при выполнении power cycle не возникало фантомное питание через сигналы управления. */
		#define HARDWARE_SDIO_HANGOFF()	do { \
			arm_hardware_piod_inputs(UINT32_C(1) << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_inputs(UINT32_C(1) << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_inputs(UINT32_C(1) << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_inputs(UINT32_C(1) << 9);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_inputs(UINT32_C(1) << 10);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_inputs(UINT32_C(1) << 11);	/* PC11 - SDIO_D3	*/ \
			arm_hardware_piod_updown(_xMask, 0, UINT32_C(1) << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(_xMask, 0, UINT32_C(1) << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(_xMask, 0, UINT32_C(1) << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_updown(_xMask, 0, UINT32_C(1) << 9);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_updown(_xMask, 0, UINT32_C(1) << 10);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_updown(_xMask, 0, UINT32_C(1) << 11);	/* PC11 - SDIO_D3	*/ \
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
			arm_hardware_piod_updown(_xMask, 0, UINT32_C(1) << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(_xMask, 0, UINT32_C(1) << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(_xMask, 0, UINT32_C(1) << 8);	/* PC8 - SDIO_D0	*/ \
		} while (0)
	#endif /* WITHSDHCHW4BIT */

	#define HARDWARE_SDIO_WP_BIT	(UINT32_C(1) << 8)	/* PG8 - SDIO_WP */
	#define HARDWARE_SDIO_CD_BIT	(UINT32_C(1) << 7)	/* PG7 - SDIO_SENSE */

	#define HARDWARE_SDIOSENSE_INITIALIZE()	do { \
			arm_hardware_piog_inputs(HARDWARE_SDIO_WP_BIT); /* PD1 - SDIO_WP */ \
			arm_hardware_piog_updown(HARDWARE_SDIO_WP_BIT, HARDWARE_SDIO_WP_BIT, 0); \
			arm_hardware_piog_inputs(HARDWARE_SDIO_CD_BIT); /* PD0 - SDIO_SENSE */ \
			arm_hardware_piog_updown(HARDWARE_SDIO_CD_BIT, HARDWARE_SDIO_CD_BIT, 0); \
	} while (0)

	#define HARDWARE_SDIOSENSE_CD() ((gpioX_getinputs(GPIOG) & HARDWARE_SDIO_CD_BIT) == 0)	/* получить состояние датчика CARD PRESENT */
	#define HARDWARE_SDIOSENSE_WP() ((gpioX_getinputs(GPIOG) & HARDWARE_SDIO_WP_BIT) != 0)	/* получить состояние датчика CARD WRITE PROTECT */

	#define HARDWARE_SDIOPOWER_C(v)	do { GPIOC->BSRR = BSRR_C(v); (void) GPIOC->BSRR; } while (0)
	#define HARDWARE_SDIOPOWER_S(v)	do { GPIOC->BSRR = BSRR_S(v); (void) GPIOC->BSRR; } while (0)
	#define HARDWARE_SDIOPOWER_BIT (UINT32_C(1) << 7)	/* PC7 */
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
	#define TXGFV_RX		(UINT32_C(1) << 4)
	#define TXGFV_TRANS		0			// переход между режимами приёма и передачи
	#define TXGFV_TX_SSB	(UINT32_C(1) << 0)
	#define TXGFV_TX_CW		(UINT32_C(1) << 1)
	#define TXGFV_TX_AM		(UINT32_C(1) << 2)
	#define TXGFV_TX_NFM	(UINT32_C(1) << 3)

	#define TXPATH_INITIALIZE() do { \
		} while (0)


	// +++
	// TXDISABLE input - PF5
	//#define TXDISABLE_TARGET_PIN				gpioX_getinputs(GPIOF)
	//#define TXDISABLE_BIT_TXDISABLE				(UINT32_C(1) << 5)		// PF5 - TX INHIBIT
	// получить бит запрета передачи (от усилителя мощности)
	#define HARDWARE_GET_TXDISABLE() 0//((TXDISABLE_TARGET_PIN & TXDISABLE_BIT_TXDISABLE) != 0)
	#define TXDISABLE_INITIALIZE() do { \
			/* arm_hardware_piof_inputs(TXDISABLE_BIT_TXDISABLE); */ \
			/* arm_hardware_piof_updown(TXDISABLE_BIT_TXDISABLE, 0, TXDISABLE_BIT_TXDISABLE); */ \
		} while (0)
	// ---

	// +++
	// PTT1 input - PF5
	// PTT2 input - PF6
	#define PTT_TARGET_PIN				gpioX_getinputs(GPIOF)
	#define PTT_BIT_PTT					(UINT32_C(1) << 5)		// PF5 - PTT1
	#define PTT2_TARGET_PIN				gpioX_getinputs(GPIOF)
	#define PTT2_BIT_PTT				(UINT32_C(1) << 6)		// PF6 - PTT2
	// получить бит запроса оператором перехода на пердачу
	#define HARDWARE_GET_PTT() 			((PTT_TARGET_PIN & PTT_BIT_PTT) == 0 || (PTT2_TARGET_PIN & PTT2_BIT_PTT) == 0)
	#define PTT_INITIALIZE() do { \
			arm_hardware_piof_inputs(PTT_BIT_PTT); \
			arm_hardware_piof_updown(PTT_BIT_PTT, PTT_BIT_PTT, 0); \
			arm_hardware_piof_inputs(PTT2_BIT_PTT); \
			arm_hardware_piof_updown(PTT2_BIT_PTT, PTT2_BIT_PTT, 0); \
		} while (0)

	// ---
	// TUNE input - PF2
	/*  @NOTE - same pin as the TWI0 SCK - @ TODO for this can be use a reserved i/o */
	#define TUNE_TARGET_PIN				gpioX_getinputs(GPIOF)
	#define TUNE_BIT_TUNE				(UINT32_C(1) << 2)		// PF2
	#define HARDWARE_GET_TUNE() 		((TUNE_TARGET_PIN & TUNE_BIT_TUNE) == 0)
	#define TUNE_INITIALIZE() do {  \
			/* arm_hardware_piof_inputs(TUNE_BIT_TUNE); */ \
			/* arm_hardware_piof_updown(TUNE_BIT_TUNE, TUNE_BIT_TUNE, 0); */ \
		} while (0)
#else /* WITHTX */

	#define TXDISABLE_INITIALIZE() do { \
		} while (0)
	#define TUNE_INITIALIZE() do { \
		} while (0)
#endif /* WITHTX */

#if WITHELKEY
	// Electronic key inputs
	#define ELKEY_BIT_LEFT				(UINT32_C(1) << 0)		// PF0
	#define ELKEY_BIT_RIGHT				(UINT32_C(1) << 1)		// PF1

	#define ELKEY_TARGET_PIN			gpioX_getinputs(GPIOF)

	#define HARDWARE_GET_ELKEY_LEFT() 	((ELKEY_TARGET_PIN & ELKEY_BIT_LEFT) == 0)
	#define HARDWARE_GET_ELKEY_RIGHT() 	((ELKEY_TARGET_PIN & ELKEY_BIT_RIGHT) == 0)


	#define ELKEY_INITIALIZE() do { \
			arm_hardware_piof_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
			arm_hardware_piof_updown(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, 0); \
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

	#define targetext1		(UINT32_C(1) << 0)		// PG0 ext1 on front panel CSEXT1
	#define targetnvram		(UINT32_C(1) << 7)		// PG7 nvram FM25L16B
	#define targetfpga1		(UINT32_C(1) << 8)		// PG8 FPGA control registers CS1
	#define targetctl1		(UINT32_C(1) << 11)		// PG11 board control registers chain
	//#define targetctl1OE	(UINT32_C(1) << 1)		// PG1 board control registers chain output enable
	//#define targetcodec1	(UINT32_C(1) << 6)		// PG6 on-board codec1 NAU8822L

	//#define targetadc2		(UINT32_C(1) << 8)	// PG8 on-board ADC MCP3208-BI/SL chip select (potentiometers) ADCCS1
	#define targetadck		(UINT32_C(1) << 9)	// PG9 on-board ADC MCP3208-BI/SL chip select (KEYBOARD) ADCCS2
	#define targetxad2		(UINT32_C(1) << 3)	// PG3 external SPI device (PA BOARD ADC) CSEXT2

	#define targetlcd	targetext1 	/* LCD over SPI line devices control */ 
	#define targetuc1608 targetext1	/* LCD with positive chip select signal	*/
	#define targettsc1 	targetext1	/* XPT2046 SPI chip select signal */

	/* Select specified chip. - select the NVRAM here ? */
	#define SPI_CS_ASSERT(target)	do { \
		switch (target) { \
		case targetdataflash: { /* SET the MUX */ \
			gpioX_setstate(GPIOG, targetfpga1, 1 * targetfpga1); \
			gpioX_setstate(GPIOC, SPDIF_NCS_BIT, 0 * (SPDIF_NCS_BIT)); \
			} break; /* PC3 SPI0_CS - FLASH NCS */ \
		case targetnvram: { /* SET the MUX */  \
			gpioX_setstate(GPIOG, targetfpga1, 1 * targetfpga1); \
			gpioX_setstate(GPIOG, targetnvram, 0 * (targetnvram)); \
			} break; \
		case targetfpga1: \
			gpioX_setstate(GPIOG, targetfpga1, 0 * targetfpga1); /* Set MUX SEL (FPGA_CS) pin to L --> DATA to FPGA */ \
			break; \
		default: { gpioX_setstate(GPIOG, (target), 0 * (target)); } break; \
		case targetnone: break; \
		} \
	} while (0)

	/* Unelect specified chip. */
	#define SPI_CS_DEASSERT(target)	do { \
		switch (target) { \
		case targetdataflash: { \
				/* SET the MUX */ \
				gpioX_setstate(GPIOG, targetfpga1, 1 * targetfpga1); \
				gpioX_setstate(GPIOC, SPDIF_NCS_BIT, 1 * (SPDIF_NCS_BIT)); \
			} break; /* PC3 SPI0_CS - FLASH NCS */ \
		case targetnvram: { \
			/* SET the MUX */ \
			gpioX_setstate(GPIOG, targetfpga1, 1 * targetfpga1); \
			gpioX_setstate(GPIOG, targetnvram, 1 * (targetnvram)); \
		} break; \
		case targetfpga1: \
			gpioX_setstate(GPIOG, targetfpga1, 1 * targetfpga1); /* Set MUX SEL (FPGA_CS) pin to H --> DATA to FLASH or NVRAM*/\
			gpioX_setstate(GPIOG, (target), 1 * (target)); \
			break; \
		default: { gpioX_setstate(GPIOG, (target), 1 * (target)); } break; \
		case targetnone: break; \
		} \
	} while (0)

	/* Perform delay after assert or de-assert specific CS line */
	#define SPI_CS_DELAY(target) do { \
		switch (target) { \
		default: break; \
		} \
	} while (0)

	/* инициализация линий выбора периферийных микросхем */
	/* data to bus flash/nvram -->  bus connected to flash */
	#define SPI_ALLCS_INITIALIZE() do { \
		arm_hardware_pioc_outputs(SPDIF_NCS_BIT, 1 * SPDIF_NCS_BIT); 	/* PC3 SPI0_CS */ \
		arm_hardware_piog_outputs(targetfpga1, 1 * targetfpga1); /*  */ \
		arm_hardware_piog_outputs(targetnvram, 1 * targetnvram); /*  */ \
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

//	#define SPI_TARGET_SCLK_PORT_C(v)	do { gpioX_setstate(GPIOC, (v), !! (0) * (v)); } while (0)
//	#define SPI_TARGET_SCLK_PORT_S(v)	do { gpioX_setstate(GPIOC, (v), !! (1) * (v)); } while (0)
//
//	#define SPI_TARGET_MOSI_PORT_C(v)	do { gpioX_setstate(GPIOC, (v), !! (0) * (v)); } while (0)
//	#define SPI_TARGET_MOSI_PORT_S(v)	do { gpioX_setstate(GPIOC, (v), !! (1) * (v)); } while (0)
//
//	#define SPI_TARGET_MISO_PIN		gpioX_getinputs(GPIOC)

	#define	SPIHARD_IX 0	/* 0 - SPI0, 1: SPI1... */
	#define	SPIHARD_PTR SPI0	/* 0 - SPI0, 1: SPI1... */
	#define	SPIHARD_CCU_CLK_REG (CCU->SPI0_CLK_REG)	/* 0 - SPI0, 1: SPI1... */
	#define BOARD_SPI_FREQ (allwnr_t113_get_spi0_freq())
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
	arm_hardware_pioe_updown(RXMASK | TXMASK, RXMASK, 0); \
} while (0)


#define TARGET_ENC2BTN_BIT (UINT32_C(1) << 6)	// PE6 - second encoder button with pull-up

#if WITHKEYBOARD
	/* PE15: pull-up second encoder button */

	//#define TARGET_POWERBTN_BIT 0//(UINT32_C(1) << 8)	// PAxx - ~CPU_POWER_SW signal

#if WITHENCODER2
	// P7_8
	#define TARGET_ENC2BTN_GET	(((gpioX_getinputs(GPIOE)) & TARGET_ENC2BTN_BIT) == 0)
#endif /* WITHENCODER2 */

#if WITHPWBUTTON
	// P5_3 - ~CPU_POWER_SW signal
	//#define TARGET_POWERBTN_GET	((gpioX_getinputs(GPIOx) & TARGET_POWERBTN_BIT) == 0)
#endif /* WITHPWBUTTON */

	#define HARDWARE_KBD_INITIALIZE() do { \
		arm_hardware_pioe_inputs(TARGET_ENC2BTN_BIT); \
		/*arm_hardware_pioa_inputs(TARGET_POWERBTN_BIT); */ /* PE6: pull-up second encoder button */ \
		/*arm_hardware_pioa_inputs(TARGET_POWERBTN_BIT); */\
		/*arm_hardware_pioa_updown(TARGET_POWERBTN_BIT, TARGET_POWERBTN_BIT, 0);	*//* PAxx: pull-up second encoder button */ \
	} while (0)

#else /* WITHKEYBOARD */

	#define HARDWARE_KBD_INITIALIZE() do { \
		} while (0)

#endif /* WITHKEYBOARD */

#if WITHTWISW || WITHTWIHW
	// TWI1-SCK PE0
	// TWI1-SDA PE1
	#define TARGET_TWI_TWCK		(UINT32_C(1) << 0)		// TWI1-SCK PE0
	#define TARGET_TWI_TWCK_PIN		(gpioX_getinputs(GPIOE))
	#define TARGET_TWI_TWCK_PORT_C(v) do { gpioX_setopendrain(GPIOE, (v), 0 * (v)); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do { gpioX_setopendrain(GPIOE, (v), 1 * (v)); } while (0)

	#define TARGET_TWI_TWD		(UINT32_C(1) << 1)		// TWI1-SDA PE1
	#define TARGET_TWI_TWD_PIN		(gpioX_getinputs(GPIOE))
	#define TARGET_TWI_TWD_PORT_C(v) do { gpioX_setopendrain(GPIOE, (v), 0 * (v)); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v) do { gpioX_setopendrain(GPIOE, (v), 1 * (v)); } while (0)

	// Инициализация битов портов ввода-вывода для программной реализации I2C
	#define	TWISOFT_INITIALIZE() do { \
		arm_hardware_pioe_opendrain(TARGET_TWI_TWCK, TARGET_TWI_TWCK); /* SCL */ \
		arm_hardware_pioe_opendrain(TARGET_TWI_TWD, TARGET_TWI_TWD);  	/* SDA */ \
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
	#define	TWIHARD_IX 1	/* 0 - TWI0, 1: TWI1... */
	#define	TWIHARD_PTR TWI1	/* 0 - TWI0, 1: TWI1... */
	#define	TWIHARD_FREQ (allwnr_t113_get_twi_freq()) // APBS2_CLK allwnr_t507_get_apb2_freq() or allwnr_t507_get_apbs2_freq()

// @! TODO - will be used to control external HWs
// @! TODO - AFs for twi 0
// arm_hardware_pioe_altfn2(TARGET_TWI_TWCK, GPIO_CFG_AF4);	// TWI1-SCK PF2
// arm_hardware_pioe_altfn2(TARGET_TWI_TWD, GPIO_CFG_AF4);		// TWI1-SDA PF4

	// TWI0-SCK PF2
	// TWI0-SDA PF4
	#define TARGET_TWI0_TWCK		(UINT32_C(1) << 2)		// TWI1-SCK PF2
	#define TARGET_TWI0_TWCK_PIN		(gpioX_getinputs(GPIOF))
	#define TARGET_TWI0_TWCK_PORT_C(v) do { gpioX_setopendrain(GPIOF, (v), 0 * (v)); } while (0)
	#define TARGET_TWI0_TWCK_PORT_S(v) do { gpioX_setopendrain(GPIOF, (v), 1 * (v)); } while (0)

	#define TARGET_TWI0_TWD		(UINT32_C(1) << 4)		// TWI1-SDA PF4
	#define TARGET_TWI0_TWD_PIN		(gpioX_getinputs(GPIOF))
	#define TARGET_TWI0_TWD_PORT_C(v) do { gpioX_setopendrain(GPIOF, (v), 0 * (v)); } while (0)
	#define TARGET_TWI0_TWD_PORT_S(v) do { gpioX_setopendrain(GPIOF, (v), 1 * (v)); } while (0)

	// Инициализация битов портов ввода-вывода для программной реализации I2C
	#define	TWISOFT0_INITIALIZE() do { \
		arm_hardware_piof_opendrain(TARGET_TWI0_TWCK, TARGET_TWI0_TWCK); /* SCL */ \
		arm_hardware_piof_opendrain(TARGET_TWI0_TWD, TARGET_TWI0_TWD);  /* SDA */ \
	} while (0)
	#define	TWISOFT0_DEINITIALIZE() do { \
		arm_hardware_piof_inputs(TARGET_TWI0_TWCK); /* SCL */ \
		arm_hardware_piof_inputs(TARGET_TWI0_TWD);	/* SDA */ \
	} while (0)
	// Инициализация битов портов ввода-вывода для аппаратной реализации I2C
	// присоединение выводов к периферийному устройству
	#define	TWIHARD0_INITIALIZE() do { \
		arm_hardware_piof_altfn2(TARGET_TWI0_TWCK, GPIO_CFG_AF4);	/* TWI1-SCK PF2 */ \
		arm_hardware_piof_altfn2(TARGET_TWI0_TWD, GPIO_CFG_AF4);	/* TWI1-SDA PF4 */ \
	} while (0)
	#define	TWIHARD0_IX 0	/* 0 - TWI0, 1: TWI1... */
	#define	TWIHARD0_PTR TWI0	/* 0 - TWI0, 1: TWI1... */
	#define	TWIHARD0_FREQ (allwnr_t113_get_twi_freq()) // APBS2_CLK allwnr_t507_get_apb2_freq() or allwnr_t507_get_apbs2_freq()
#endif // WITHTWISW || WITHTWIHW

#if WITHFPGAWAIT_AS || WITHFPGALOAD_PS

	/* outputs */
	#define FPGA_NCONFIG_PORT_S(v)	do { gpioX_setstate(GPIOE, (v), !! (1) * (v)); } while (0)
	#define FPGA_NCONFIG_PORT_C(v)	do { gpioX_setstate(GPIOE, (v), !! (0) * (v)); } while (0)
	#define FPGA_NCONFIG_BIT		(UINT32_C(1) << 12)	/* PE12 bit connected to nCONFIG pin ALTERA FPGA */

	/* inputs */
	#define FPGA_CONF_DONE_INPUT	(gpioX_getinputs(GPIOE))
	#define FPGA_CONF_DONE_BIT		(UINT32_C(1) << 13)	/* PE13 bit connected to CONF_DONE pin ALTERA FPGA */

	#define FPGA_NSTATUS_INPUT		gpioX_getinputs(GPIOB)
	#define FPGA_NSTATUS_BIT		(UINT32_C(1) << 2)	/* PB2 bit connected to NSTATUS pin ALTERA FPGA */

	#define FPGA_INIT_DONE_INPUT	(gpioX_getinputs(GPIOG))
	#define FPGA_INIT_DONE_BIT		(UINT32_C(1) << 10)	/* PG10 bit connected to INIT_DONE pin ALTERA FPGA */

	/* Инициадизация выводов GPIO процессора для получения состояния и управлением загрузкой FPGA */
	#define HARDWARE_FPGA_LOADER_INITIALIZE() do { \
		arm_hardware_pioe_outputs(FPGA_NCONFIG_BIT, FPGA_NCONFIG_BIT); \
		arm_hardware_piob_inputs(FPGA_NSTATUS_BIT); \
		arm_hardware_pioe_inputs(FPGA_CONF_DONE_BIT); \
		arm_hardware_piog_inputs(FPGA_INIT_DONE_BIT); \
	} while (0)

	/* необходимость функции под вопросом (некоторые FPGA не грузятся с этой процедурой) */
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

	/* необходимость функции под вопросом (некоторые FPGA не грузятся с этой процедурой) */
	#define HARDWARE_FPGA_RESET() do { \
		/* board_fpga_reset(); */ \
	} while (0)

#endif /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS */

#if WITHDSPEXTFIR
	// Биты доступа к массиву коэффициентов FIR фильтра в FPGA

	// FPGA PIN_23
	#define TARGET_FPGA_FIR_CS_PORT_C(v)	do { gpioX_setstate(GPIOD, (v), !! (0) * (v)); } while (0)
	#define TARGET_FPGA_FIR_CS_PORT_S(v)	do { gpioX_setstate(GPIOD, (v), !! (1) * (v)); } while (0)
	#define TARGET_FPGA_FIR_CS_BIT (UINT32_C(1) << 12)	/* PD12 - fir CS ~FPGA_FIR_CLK */
	// FPGA PIN_8
	#define TARGET_FPGA_FIR1_WE_PORT_C(v)	do { gpioX_setstate(GPIOG, (v), !! (0) * (v)); } while (0)
	#define TARGET_FPGA_FIR1_WE_PORT_S(v)	do { gpioX_setstate(GPIOG, (v), !! (1) * (v)); } while (0)
	#define TARGET_FPGA_FIR1_WE_BIT (UINT32_C(1) << 4)	/* PG4 - fir1 WE */

	// FPGA PIN_7
	#define TARGET_FPGA_FIR2_WE_PORT_C(v)	do { gpioX_setstate(GPIOG, (v), !! (0) * (v)); } while (0)
	#define TARGET_FPGA_FIR2_WE_PORT_S(v)	do { gpioX_setstate(GPIOG, (v), !! (1) * (v)); } while (0)
	#define TARGET_FPGA_FIR2_WE_BIT (UINT32_C(1) << 5)	/* PG5 - fir2 WE */

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

#if 0
	/* получение состояния переполнения АЦП */
	#define TARGET_FPGA_OVF_INPUT		gpioX_getinputs(GPIOB)
	#define TARGET_FPGA_OVF_BIT			(UINT32_C(1) << 7)	// PB7
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

	#define	USBD_EHCI_INITIALIZE() do { \
		} while (0)

	// todo: add PE9 manipulation for XtremeDx board
	#define TARGET_USBFS_VBUSON_SET(on)	do { \
		} while (0)

	#define	USBD_HS_FS_INITIALIZE() do { \
		} while (0)

	#define	USBD_HS_ULPI_INITIALIZE() do { \
		} while (0)
#endif /* WITHUSBHW */

#if WITHISBOOTLOADER
	#define	HARDWARE_DCDC_INITIALIZE() do { \
	} while (0)
#elif WITHDCDCFREQCTL
	// ST ST1S10 Synchronizable switching frequency from 400 kHz up to 1.2 MHz
	#define WITHHWDCDCFREQMIN 400000L
	#define WITHHWDCDCFREQMAX 1200000L
	#define HARDWARE_DCDC_PWMCH 3	/* PWM3 */

	// PG10 - DC-DC synchro output
	// PWM3 AF6
	#define	HARDWARE_DCDC_INITIALIZE() do { \
		hardware_dcdcfreq_pwm_initialize(HARDWARE_DCDC_PWMCH); \
		arm_hardware_piog_altfn2((UINT32_C(1) << 10), GPIO_CFG_AF2); \
	} while (0)
	#define HARDWARE_DCDC_SETDIV(f) do { \
		hardware_dcdcfreq_pwm_setdiv(HARDWARE_DCDC_PWMCH, f); \
	} while (0)
#else /* WITHDCDCFREQCTL */
	#define	HARDWARE_DCDC_INITIALIZE() do { \
		arm_hardware_piog_outputs((UINT32_C(1) << 10), 0 * (UINT32_C(1) << 10)); /* PG10 */ \
	} while (0)
	#define HARDWARE_DCDC_SETDIV(f) do { \
		(void) (f); \
	} while (0)
#endif /* WITHDCDCFREQCTL */


#if WITHISBOOTLOADER
	#define	HARDWARE_BL_INITIALIZE() do { \
	} while (0)
#elif WITHBLPWMCTL
	/* Управление яркостью подсветки через выход PWN */
	#if LCDMODE_LQ043T3DX02K
		#define WITHLCDBACKLIGHTOFF	1	// Имеется управление включением/выключением подсветки дисплея
		#define WITHLCDBACKLIGHT	1	// Имеется управление яркостью дисплея
		#define WITHLCDBACKLIGHTMIN	1	// Нижний предел регулировки (показываемый на дисплее)
		#define WITHLCDBACKLIGHTMAX	5	// Верхний предел регулировки (показываемый на дисплее)
		#define WITHLCDBACKLIGHTDEF	5	// значение яркости по уиолчанию
		//#define WITHKBDBACKLIGHT	1	// Имеется управление подсветкой клавиатуры
	#elif LCDMODE_AT070TN90 || LCDMODE_AT070TNA2
		#define WITHLCDBACKLIGHTOFF	1	// Имеется управление включением/выключением подсветки дисплея
		#define WITHLCDBACKLIGHT	1	// Имеется управление яркостью дисплея
		#define WITHLCDBACKLIGHTMIN	1	// Нижний предел регулировки (показываемый на дисплее)
		#define WITHLCDBACKLIGHTMAX	5	// Верхний предел регулировки (показываемый на дисплее)
		#define WITHLCDBACKLIGHTDEF	5	// значение яркости по уиолчанию
		//#define WITHKBDBACKLIGHT	1	// Имеется управление подсветкой клавиатуры
	#else
		/* Заглушка для работы без дисплея */
		#define WITHLCDBACKLIGHTMIN	0
		#define WITHLCDBACKLIGHTMAX	2	// Верхний предел регулировки (показываемый на дисплее)
	#endif
	/* установка яркости и включение/выключение преобразователя подсветки */
	/* Яркость Управлятся через 75HC595 или PWM */
	#define HARDWARE_BL_PWMCH 7	/* PWM7 */
	#define HARDWARE_BL_FREQ 	10000	/* Частота PWM управления подсветкой */

	#if 0	/*TODO in revision design - and change the port later*/
	#define	HARDWARE_BL_INITIALIZE() do { \
		const portholder_t ENmask = (UINT32_C(1) << 22); /* PD22 (PWM7, Alt FN 5) */ \
		hardware_dcdcfreq_pwm_initialize(HARDWARE_BL_PWMCH); \
		arm_hardware_piod_altfn2(ENmask, GPIO_CFG_AF5); /* PD22 - PWM7 */ \
	} while (0)
	// en: 0/1, level=WITHLCDBACKLIGHTMIN..WITHLCDBACKLIGHTMAX
	// level=WITHLCDBACKLIGHTMIN не приводит к выключениию подсветки
	#define HARDWARE_BL_SET(en, level) do { \

		hardware_bl_pwm_set_duty(HARDWARE_BL_PWMCH, HARDWARE_BL_FREQ, !! (en) * (level) * 100 / WITHLCDBACKLIGHTMAX); \
	} while (0)
	#endif

/*keep the empty macros - see up*/
#define	HARDWARE_BL_INITIALIZE() do { \
} while (0)

#define HARDWARE_BL_SET(en, level) do { \
} while (0)


#else /* WITHISBOOTLOADER */
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
	#define	HARDWARE_BL_INITIALIZE() do { \
		const portholder_t ENmask = (UINT32_C(1) << 22); /* PD22 (PWM7, Alt FN 5) */ \
        arm_hardware_piod_outputs(ENmask, 1 * ENmask); /* PD22 - PWM7 */ \
	} while (0)
	// en: 0/1, level=WITHLCDBACKLIGHTMIN..WITHLCDBACKLIGHTMAX
	// level=WITHLCDBACKLIGHTMIN не приводит к выключениию подсветки
	#define HARDWARE_BL_SET(en, level) do { \
		const portholder_t ENmask = (UINT32_C(1) << 22); /* PD22 */ \
		gpioX_setstate(GPIOD, ENmask, !! (en) * ENmask); \
	} while (0)
#endif

#if WITHLTDCHW

	// Sochip_VE_S3_Datasheet_V1.0.pdf
	// TCON0_TRM_CTL_REG offset 0x0010
	// User manual:
	// LCD FRM Control Register (Default Value: 0x0000_0000)
	#define TCON_FRM_MODE_VAL 0//((UINT32_C(1) << 31) | (UINT32_C(1) << 6) | (0u << 5) | (UINT32_C(1) << 4))	// 16 bit panel connected
	//#define TCON_FRM_MODE_VAL 0//((UINT32_C(1) << 31) | (0u << 6) | (0u << 5)| (0u << 4))	// 18 bit panel connected

	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_INITIALIZE(demode) do { \
		const portholder_t DITHmask = (UINT32_C(1) << 4); 	/* PE4 LCD_DITH */ \
		const portholder_t VSmask = (UINT32_C(1) << 21); 	/* PD21 LCD_VSYNC */ \
		const portholder_t HSmask = (UINT32_C(1) << 20); 	/* PD20 LCD_HSYNC */ \
		const portholder_t DEmask = (UINT32_C(1) << 19); 	/* PD19 LCD_DE */ \
		/* synchro signals - sync mode */ \
		arm_hardware_piod_outputs(((demode) == 0) * DEmask, 0 * DEmask); /* PD19 LCD_DE */ \
		arm_hardware_piod_altfn20(((demode) == 0) * VSmask, GPIO_CFG_AF2); /* PD21 LCD_VSYNC */ \
		arm_hardware_piod_altfn20(((demode) == 0) * HSmask, GPIO_CFG_AF2); /* PD20 LCD_HSYNC */ \
		/* synchro signals - DE mode */ \
		arm_hardware_piod_altfn20(((demode) != 0) * DEmask, GPIO_CFG_AF2); /* PD19 LCD_DE */ \
		arm_hardware_piod_outputs(((demode) != 0) * VSmask, 1 * VSmask); /* PD21 LCD_VSYNC */ \
		arm_hardware_piod_outputs(((demode) != 0) * HSmask, 1 * HSmask); /* PD20 LCD_HSYNC */ \
		arm_hardware_pioe_outputs(DITHmask, 0 * DITHmask); /* PE4 LCD_DITH */ \
		/* force all sync */ \
		arm_hardware_piod_altfn20(1 * DEmask, GPIO_CFG_AF2); /* PD19 LCD_DE */ \
		arm_hardware_piod_altfn20(1 * VSmask, GPIO_CFG_AF2); /* PD21 LCD_VSYNC */ \
		arm_hardware_piod_altfn20(1 * HSmask, GPIO_CFG_AF2); /* PD20 LCD_HSYNC */ \
		/* pixel clock */ \
		arm_hardware_piod_altfn20(UINT32_C(1) << 18, GPIO_CFG_AF2); /* PD18 LCD_CLK */ \
		/* RED */ \
		arm_hardware_piod_altfn20(UINT32_C(1) << 13, GPIO_CFG_AF2); /* R3 PD13 LCD_D19 */ \
		arm_hardware_piod_altfn20(UINT32_C(1) << 14, GPIO_CFG_AF2); /* R4 PD14 LCD_D20 */ \
		arm_hardware_piod_altfn20(UINT32_C(1) << 15, GPIO_CFG_AF2); /* R5 PD15 LCD_D21 */ \
		arm_hardware_piod_altfn20(UINT32_C(1) << 16, GPIO_CFG_AF2); /* R6 PD16 LCD_D22 */ \
		arm_hardware_piod_altfn20(UINT32_C(1) << 17, GPIO_CFG_AF2); /* R7 PD17 LCD_D23 */ \
		/* GREEN */ \
		arm_hardware_piod_altfn20(UINT32_C(1) << 6, GPIO_CFG_AF2); 	/* G2 PD6 LCD_D10 */ \
		arm_hardware_piod_altfn20(UINT32_C(1) << 7, GPIO_CFG_AF2); 	/* G3 PD7 LCD_D11 */ \
		arm_hardware_piod_altfn20(UINT32_C(1) << 8, GPIO_CFG_AF2); 	/* G4 PD8 LCD_D12 */ \
		arm_hardware_piod_altfn20(UINT32_C(1) << 9, GPIO_CFG_AF2); 	/* G5 PD9 LCD_D13 */ \
		arm_hardware_piod_altfn20(UINT32_C(1) << 10, GPIO_CFG_AF2); /* G6 PD10 LCD_D14 */ \
		arm_hardware_piod_altfn20(UINT32_C(1) << 11, GPIO_CFG_AF2); /* G7 PD11 LCD_D15 */ \
		/* BLUE  */ \
		arm_hardware_piod_altfn20(UINT32_C(1) << 1, GPIO_CFG_AF2); 	/* B3 PD1 LCD_D3 */ \
		arm_hardware_piod_altfn20(UINT32_C(1) << 2, GPIO_CFG_AF2); 	/* B4 PD2 LCD_D4 */ \
		arm_hardware_piod_altfn20(UINT32_C(1) << 3, GPIO_CFG_AF2); 	/* B5 PD3 LCD_D5 */ \
		arm_hardware_piod_altfn20(UINT32_C(1) << 4, GPIO_CFG_AF2); 	/* B6 PD4 LCD_D6 */ \
		arm_hardware_piod_altfn20(UINT32_C(1) << 5, GPIO_CFG_AF2); 	/* B7 PD5 LCD_D7 */ \
	} while (0)

	/* управление состоянием сигнала DISP панели */
	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_SET_DISP(state) do { \
		const portholder_t DITHmask = (UINT32_C(1) << 4); 	/* PE4 LCD_DITH */ \
		const portholder_t VSmask = (UINT32_C(1) << 21); 	/* PD21 LCD_VSYNCC */ \
		const portholder_t HSmask = (UINT32_C(1) << 20); 	/* PD20 LCD_HSYNC */ \
		const portholder_t DEmask = (UINT32_C(1) << 19); 	/* PD19 LCD_DE */ \
		arm_hardware_pioe_outputs(DITHmask, 0 * DITHmask); /* PE4 LCD_DITH */ \
		arm_hardware_piod_outputs(VSmask, 0 * VSmask); /* PD21 LCD_VSYNC */ \
		local_delay_ms(5); \
		/* while ((gpioX_getinputs(GPIOD) & VSmask) != 0) ; */ /* схема синхронизации стоит на плате дисплея. дождаться 0 */ \
		/* while ((gpioX_getinputs(GPIOD) & VSmask) == 0) ; */ /* дождаться 1 */ \
		arm_hardware_piod_outputs(DEmask, ((state) != 0) * DEmask); /* DE=DISP, pin 31 - можно менять только при VSYNC=1 */ \
		local_delay_ms(5); \
		arm_hardware_piod_altfn20(VSmask, GPIO_CFG_AF2); /* PD21 LCD_VSYNC */ \
	} while (0)

	#define LCD_LVDS_IF_REG_VALUE ( \
		(UINT32_C(1) << 31) |	/* LCD_LVDS_EN */ \
		(0u << 30) |	/* LCD_LVDS_LINK: 0: single link */ \
		(! UINT32_C(1) << 27) |	/* LCD_LVDS_MODE 1: JEIDA mode (0 for THC63LVDF84B converter) */ \
		(0u << 26) |	/* LCD_LVDS_BITWIDTH 0: 24-bit */ \
		(UINT32_C(1) << 20) |	/* LCD_LVDS_CLK_SEL 1: LCD CLK */ \
		0 * (UINT32_C(1) << 25) |		/* LCD_LVDS_DEBUG_EN */ \
		0 * (UINT32_C(1) << 24) |		/* LCD_LVDS_DEBUG_MODE */ \
		0 * (UINT32_C(1) << 4) |				/* LCD_LVDS_CLK_POL: 0: reverse, 1: normal */ \
		0 * 0x0F * (UINT32_C(1) << 0) |		/* LCD_LVDS_DATA_POL: 0: reverse, 1: normal */ \
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

	#if 1
		#define	TCONLCD_IX 0	/* 0 - TCON_LCD0, 1: TCON_TV0 */
		#define	TCONLCD_PTR TCON_LCD0	/* 0 - TCON_LCD0, 1: TCON_TV0 */
		#define	TCONLCD_CCU_CLK_REG (CCU->TCONLCD_CLK_REG)	/* 0 - TCON_LCD0, 1: TCON_TV0 */
		#define BOARD_TCONLCDFREQ (allwnr_t113_get_tconlcd_freq())
		#define TCONLCD_IRQ TCON_LCD0_IRQn
		#define TCONLCD_LVDSIX 0	/* 0 -LVDS0 */
	#endif

	#if 0
		#define	TCONTV_IX 0	/* 0 - TCON_TV0, 1: TCON_TV1 */
		#define	TCONTV_PTR TCON_TV0	/* 0 - TCON_TV0, 1: TCON_TV0 */
		#define	TCONTV_CCU_CLK_REG (CCU->TCONTV_CLK_REG)	/* 0 - TCON_TV0, 1: TCON_TV1 */
		#define	TCONTV_CCU_BGR_REG (CCU->TCONTV_BGR_REG)	/* 0 - TCON_TV0, 1: TCON_TV1 */
		#define TCONTV_IRQ TCON_TV0_IRQn
		#define BOARD_TCONTVFREQ (allwnr_t113_get_tcontv_freq())
	#endif

	#if 0
		#define	TVENCODER_IX 0	/* 0 -TVE0 */
		#define	TVENCODER_PTR TVE0	/* 0 - TVE0 */
		#define	TVENCODER_BASE TVE0_BASE	/* 0 - TVE0 */
		#define	TVE_CCU_CLK_REG (CCU->TVE_CLK_REG)	/* 0 - TVE0, 1: TVE1 */
		#define BOARD_TVEFREQ (allwnr_t113_get_tve_freq())
	#endif

#endif /* WITHLTDCHW */

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811)

	//	tsc interrupt XS26, pin 08
	//	tsc/LCD reset, XS26, pin 22
	//	tsc SCL: XS26, pin 01
	//	tsc SDA: XS26, pin 02

	void stmpe811_interrupt_handler(void * ctx);

	#define BOARD_GPIOE_STMPE811_INT_PIN (UINT32_C(1) << 9)		/* PE9 : tsc interrupt XS26, pin 08 */

	#define BOARD_STMPE811_INT_CONNECT() do { \
		static einthandler_t h; \
		arm_hardware_pioe_altfn2(BOARD_GPIOE_STMPE811_INT_PIN, GPIO_CFG_EINT); \
		arm_hardware_pioe_updown(_xMask, BOARD_GPIOE_STMPE811_INT_PIN, 0); \
		einthandler_initialize(& h, 0 * BOARD_GPIOE_STMPE811_INT_PIN, stmpe811_interrupt_handler, NULL); \
		arm_hardware_pioe_onchangeinterrupt(0 * BOARD_GPIOE_STMPE811_INT_PIN, 1 * BOARD_GPIOE_STMPE811_INT_PIN, 0 * BOARD_GPIOE_STMPE811_INT_PIN, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM, & h); \
	} while (0)

#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811) */

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_GT911)

	/* PE9 TS_INT */
	/* PD22 FPLCD_RESET */

	void gt911_interrupt_handler(void * ctx);

	#define BOARD_GPIOE_GT911_INT_PIN (UINT32_C(1) << 9)		/* PE9 : tsc interrupt XS26, pin 08 */
	#define BOARD_GT911_RESET_PIN (UINT32_C(1) << 10)			/* PE10 : tsc/LCD reset, XS26, pin 22 */

	#define BOARD_GT911_RESET_SET(v) do { gpioX_setstate(GPIOE, (v), !! (0) * (v)); } while (0)
	#define BOARD_GT911_INT_SET(v) do { gpioX_setstate(GPIOE, (v), !! (0) * (v)); } while (0)

	#define BOARD_GT911_RESET_INITIO_1() do { \
		arm_hardware_pioe_outputs2m(BOARD_GPIOE_GT911_INT_PIN, 1* BOARD_GPIOE_GT911_INT_PIN); \
		arm_hardware_pioe_outputs2m(BOARD_GT911_RESET_PIN, 1 * BOARD_GT911_RESET_PIN); \
		 local_delay_ms(200);  \
	} while (0)

	#define BOARD_GT911_RESET_INITIO_2() do { \
		arm_hardware_pioe_inputs(BOARD_GPIOE_GT911_INT_PIN); \
		arm_hardware_pioe_updown(_xMask, BOARD_GPIOE_GT911_INT_PIN, 0); \
	} while (0)

	#define BOARD_GT911_INT_CONNECT() do { \
		static einthandler_t h; \
		arm_hardware_pioe_altfn2(BOARD_GPIOE_GT911_INT_PIN, GPIO_CFG_EINT); \
		arm_hardware_pioe_updown(_xMask, BOARD_GPIOE_GT911_INT_PIN, 0); \
		einthandler_initialize(& h, 0 * gt911_interrupt_handler, NULL); \
		arm_hardware_pioe_onchangeinterrupt(0 * BOARD_GPIOE_GT911_INT_PIN, 1 * BOARD_GPIOE_GT911_INT_PIN, 0 * BOARD_GPIOE_GT911_INT_PIN, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM); \
	} while (0)
	//gt911_interrupt_handler

#endif


#if 1
	#define BOARD_BLINK_BIT (UINT32_C(1) << 12)	// PD12 - led on XtremeDx board
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
	#define BOARD_IS_USERBOOT() (0) //(((gpioX_getinputs(GPIOE)) & TARGET_ENC2BTN_BIT) == 0)
	#define BOARD_USERBOOT_INITIALIZE() do { \
			/*arm_hardware_pioe_inputs(TARGET_ENC2BTN_BIT); *//* set as input with pull-up */ \
		} while (0)

	/* макроопределение, которое должно включить в себя все инициализации */
	#define	HARDWARE_INITIALIZE() do { \
		HARDWARE_FPGA_RESET(); \
		BOARD_BLINK_INITIALIZE();\
		HARDWARE_KBD_INITIALIZE(); \
		/*HARDWARE_DAC_INITIALIZE(); */\
		HARDWARE_BL_INITIALIZE(); \
		HARDWARE_DCDC_INITIALIZE(); \
		TXDISABLE_INITIALIZE(); \
		TUNE_INITIALIZE(); \
		BOARD_USERBOOT_INITIALIZE(); \
		USBD_EHCI_INITIALIZE(); \
	} while (0)

	// TUSB parameters
	#define TUP_DCD_ENDPOINT_MAX    6

#endif /* ARM_ALWT113S3_CPUSTYLE_YO6POC_H_INCLUDED */
