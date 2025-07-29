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

//#define WITHSPI16BIT	1	/* возможно использование 16-ти битных слов при обмене по SPI */
//#define WITHSPI32BIT	1	/* возможно использование 32-ти битных слов при обмене по SPI */
//#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
//#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
//#define WITHSPISW 	1	/* Использование программного управления SPI. Нельзя убирать эту строку - требуется явное отключение из-за конфликта с I2C */

//#define WIHSPIDFSW	1	/* программное обслуживание DATA FLASH */
//#define WIHSPIDFOVERSPI 1	/* Для работы используется один из обычных каналов SPI */
//#define WIHSPIDFHW		1	/* аппаратное обслуживание DATA FLASH */
//#define WIHSPIDFHW2BIT	1	/* аппаратное обслуживание DATA FLASH с поддержкой QSPI подключения по 2-м проводам */
//#define WIHSPIDFHW4BIT	1	/* аппаратное обслуживание DATA FLASH с поддержкой QSPI подключения по 4-м проводам */

//#define WITHDMA2DHW		1	/* Использование DMA2D для формирования изображений	- у STM32MP1 его нет */

//#define WITHCAN0HW 1
//#define WITHCAN1HW 1

#define WITHUART1HW	1	/* 485xx2 	TX=PG6 RX=PG7 UART1 */
#define WITHUART2HW	1	/* 485xx3 	TX=PE2 RX=PE3 UART2 */
#define WITHUART3HW	1	/* 485xx1 	TX=PG8 RX=PG9 UART3 */
#define WITHUART4HW	1	/* MODEM_xx TX=PE4 RX=PE5 UART4 */
#define WITHUART5HW	1	/* 485xx4 	TX=PE5 RX=PE6 UART5 */

#define WITHDEBUG_UART4	1

// WITHUART1HW
#define HARDWARE_UART1_INITIALIZE() do { \
		const portholder_t TX485 = (UINT32_C(1) << 12); /* PE12 - 485_TR2 */ \
		const portholder_t TXMASK = (UINT32_C(1) << 6); /* PG6 UART1-TX 485_TX2 */ \
		const portholder_t RXMASK = (UINT32_C(1) << 7); /* PG7 UART1-RX - pull-up RX data 485_RX2 */  \
		arm_hardware_piog_altfn2(TXMASK, GPIO_CFG_AF2); \
		arm_hardware_piog_altfn2(RXMASK, GPIO_CFG_AF2); \
		arm_hardware_piog_updown(RXMASK, RXMASK, 0); \
		arm_hardware_pioe_outputs(TX485, 0 * TX485); /*  */ \
	} while (0)

// WITHUART2HW
#define HARDWARE_UART2_INITIALIZE() do { \
		const portholder_t TX485 = (UINT32_C(1) << 19); /* PE10 - 485_TR3 */ \
		const portholder_t TXMASK = (UINT32_C(1) << 2); /* PE2 UART2-TX 485_TX3 */ \
		const portholder_t RXMASK = (UINT32_C(1) << 3); /* PE3 UART2-RX - pull-up RX data 485_RX3 */  \
		arm_hardware_pioe_altfn2(TXMASK, GPIO_CFG_AF3); \
		arm_hardware_pioe_altfn2(RXMASK, GPIO_CFG_AF3); \
		arm_hardware_pioe_updown(RXMASK, RXMASK, 0); \
		arm_hardware_pioe_outputs(TX485, 0 * TX485); /*  */ \
	} while (0)

// WITHUART4HW
#define HARDWARE_UART4_INITIALIZE() do { \
		const portholder_t TXMASK = (UINT32_C(1) << 4); /* PE4 UART4-TX  */ \
		const portholder_t RXMASK = (UINT32_C(1) << 5); /* PE5 UART4-RX  - pull-up RX data */  \
		arm_hardware_pioe_altfn2(TXMASK, GPIO_CFG_AF3); \
		arm_hardware_pioe_altfn2(RXMASK, GPIO_CFG_AF3); \
		arm_hardware_pioe_updown(RXMASK, RXMASK, 0); \
	} while (0)

// WITHUART3HW
#define HARDWARE_UART3_INITIALIZE() do { \
		const portholder_t TX485 = (UINT32_C(1) << 13); /* PE13 - 485_TR1 */ \
		const portholder_t TXMASK = (UINT32_C(1) << 8); /* PG8 UART3-TX 485_TX1 */ \
		const portholder_t RXMASK = (UINT32_C(1) << 9); /* PG9 UART3-RX - pull-up RX data 485_RX1 */  \
		arm_hardware_piog_altfn2(TXMASK, GPIO_CFG_AF5); \
		arm_hardware_piog_altfn2(RXMASK, GPIO_CFG_AF5); \
		arm_hardware_piog_updown(RXMASK, RXMASK, 0); \
		arm_hardware_pioe_outputs(TX485, 0 * TX485); /*  */ \
	} while (0)

// WITHUART5HW
#define HARDWARE_UART5_INITIALIZE() do { \
		const portholder_t TX485 = (UINT32_C(1) << 9); /* PE9 - 485_TR4 */ \
		const portholder_t TXMASK = (UINT32_C(1) << 6); /* PE6 UART5-TX 485_TX4 */ \
		const portholder_t RXMASK = (UINT32_C(1) << 7); /* PE7 UART5-RX  - pull-up RX data 485_RX4 */  \
		arm_hardware_pioe_altfn2(TXMASK, GPIO_CFG_AF3); \
		arm_hardware_pioe_altfn2(RXMASK, GPIO_CFG_AF3); \
		arm_hardware_pioe_updown(RXMASK, RXMASK, 0); \
		arm_hardware_pioe_outputs(TX485, 0 * TX485); /*  */ \
	} while (0)

#define WITHCAN0HW 1

// WITHCAN0HW
#define HARDWARE_CAN0_INITIALIZE() do { \
		const portholder_t TXMASK = UINT32_C(1) << 2; /* PB2 CAN0_TX */ \
		const portholder_t RXMASK = UINT32_C(1) << 3; /* PB3 CAN0_RX - pull-up RX data */  \
		arm_hardware_piob_altfn2(TXMASK, GPIO_CFG_AF8); \
		arm_hardware_piob_altfn2(RXMASK, GPIO_CFG_AF8); \
		arm_hardware_piob_updown(RXMASK, RXMASK, 0); \
		arm_hardware_pioe_outputs(UINT32_C(1) << 11, 1 * UINT32_C(1) << 11); /* PE11 CAN_TR */ \
	} while (0)

// OHCI at USB1HSFSP2_BASE
////#define WITHUSBHW_OHCI ((struct ohci_registers *) USB1HSFSP2_BASE)

#if WITHISBOOTLOADER

	#define WITHSDRAMHW	1		/* В процессоре есть внешняя память */
	//#define WITHSDRAM_PMC1	1	/* power management chip */

	//#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
	//#define WITHGPUHW	1	/* Graphic processor unit */
	
	#define WITHSDHCHW	1	/* Hardware SD HOST CONTROLLER */
	#define WITHSDHC0HW	1		/* Hardware SD HOST #0 CONTROLLER */

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
	#define WITHSDHCHW	1	/* Hardware SD HOST CONTROLLER */
	#define WITHSDHC2HW	1		/* SDIO/eMMC */


	#if WITHINTEGRATEDDSP
//		#define WITHI2S1HW	1	/* Использование I2S1 - аудиокодек на I2S */
//		#define WITHI2S2HW	1	/* Использование I2S2 - FPGA или IF codec	*/
	#endif /* WITHINTEGRATEDDSP */
	//#define WITHETHHW 1	/* Hardware Ethernet controller */

	//#define WITHDCDCFREQCTL	1		// Имеется управление частотой преобразователей блока питания
	#define WITHBLPWMCTL	1		// Имеется управление яркостью подсветки дисплея через PWM
	//#define WITHCODEC1_I2S1_DUPLEX_SLAVE	1		/* Обмен с аудиокодеком через I2S1 */
	//#define WITHFPGAIF_I2S2_DUPLEX_SLAVE	1		/* Обмен с FPGA через I2S2 */
	//#define WITHCODEC1_I2S1_DUPLEX_MASTER	1		/* Обмен с аудиокодеком через I2S1 */
	//#define WITHFPGAIF_I2S2_DUPLEX_MASTER	1		/* Обмен с FPGA через I2S2 */
	#define WITHCODEC1_WHBLOCK_DUPLEX_MASTER	1	/* встороенный в процессор кодек */
	#define WITHCODEC1_WHBLOCK_LINEIN 1
	//#define WITHCODEC1_WHBLOCK_FMIN 1

	#if ! LCDMODE_DUMMY
		#define WITHMDMAHW		1	/* Использование G2D для формирования изображений */
		#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
		//#define WITHGPUHW	1	/* Graphic processor unit */
		#define WITHLTDCHWVBLANKIRQ 1	/* Смена framebuffer по прерыванию */
	#endif
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
		#define TUP_USBIP_OHCI 1
		//#define TUP_USBIP_EHCI 1
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
	//#define WITHUSBCDCACM_NOINT	1	/* Не использовать NOTIFY endpoint - под Linux не работает */


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


#if WITHSDHCHW && WITHSDHC0HW

	// SMHC0: SDC0 - SD CARD
	// SMHC2: SDC2 - eMMC

	#define	USERFIRSTSBLOCK 0

	#define	SMHCHARD_IX 0	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_PTR SMHC0	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_BASE SMHC0_BASE	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_CCU_CLK_REG (CCU->SMHC0_CLK_REG)	/* 0 - SMHC0, 1: SMHC1... */
	#define SMHCHARD_FREQ (allwnr_t113_get_smhc0_freq())
	#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */

	#define HARDWARE_SDIO_INITIALIZE() do { \
		arm_hardware_piof_altfn50(UINT32_C(1) << 3, GPIO_CFG_AF2);	/* PF3 - SDIO_CMD	*/ \
		arm_hardware_piof_altfn50(UINT32_C(1) << 2, GPIO_CFG_AF2);	/* PF2 - SDIO_CK	*/ \
		arm_hardware_piof_altfn50(UINT32_C(1) << 1, GPIO_CFG_AF2);	/* PF1 - SDIO_D0	*/ \
		arm_hardware_piof_altfn50(UINT32_C(1) << 0, GPIO_CFG_AF2);	/* PF0 - SDIO_D1	*/ \
		arm_hardware_piof_altfn50(UINT32_C(1) << 5, GPIO_CFG_AF2);	/* PF5 - SDIO_D2	*/ \
		arm_hardware_piof_altfn50(UINT32_C(1) << 4, GPIO_CFG_AF2);	/* PF4 - SDIO_D3	*/ \
		arm_hardware_piof_updown(UINT32_C(1) << 3, UINT32_C(1) << 3, 0);	/* PF3 - SDC0_CMD	*/ \
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

	// sd sense - PG10
	// HARDWARE_SDIOSENSE_CD: 1-has disk. 0-no disk
	#define HARDWARE_SDIOSENSE_CD() 1//((GPIOF->DATA & HARDWARE_SDIO_CD_BIT) == 0)	/* == 0: no disk. получить состояние датчика CARD PRESENT */
	#define HARDWARE_SDIOSENSE_WP() 0//((GPIOG->DATA & HARDWARE_SDIO_WP_BIT) != 0)	/* != 0: write protected получить состояние датчика CARD WRITE PROTECT */

	/* если питание SD CARD управляется прямо с процессора */
	#define HARDWARE_SDIOPOWER_INITIALIZE()	do { \
		} while (0)
	/* parameter on not zero for powering SD CARD */
	#define HARDWARE_SDIOPOWER_SET(on) do { \
	} while (0)

#elif WITHSDHCHW && WITHSDHC2HW
	// SDIO
	#define USERFIRSTSBLOCK 0

	// SMHC0: SDC0 - SD CARD
	// SMHC2: SDC2 - eMMC

	#define	SMHCHARD_IX 2	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_PTR SMHC2	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_BASE SMHC2_BASE	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_CCU_CLK_REG (CCU->SMHC2_CLK_REG)	/* 0 - SMHC0, 1: SMHC1... */
	#define SMHCHARD_FREQ (allwnr_t113_get_smhc2_freq())
	#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */

	#define HARDWARE_SDIO_INITIALIZE() do { \
		arm_hardware_pioc_altfn50(UINT32_C(1) << 3, GPIO_CFG_AF3);	/* PC3 - SDC2_CMD	*/ \
		arm_hardware_pioc_altfn50(UINT32_C(1) << 2, GPIO_CFG_AF3);	/* PC2 - SDC2_CK	*/ \
		arm_hardware_pioc_altfn50(UINT32_C(1) << 4, GPIO_CFG_AF3);	/* PC4 - SDC2_D0	*/ \
		arm_hardware_pioc_altfn50(UINT32_C(1) << 5, GPIO_CFG_AF3);	/* PC5 - SDC2_D1	*/ \
		arm_hardware_pioc_altfn50(UINT32_C(1) << 6, GPIO_CFG_AF3);	/* PC6 - SDC2_D2	*/ \
		arm_hardware_pioc_altfn50(UINT32_C(1) << 7, GPIO_CFG_AF3);	/* PC7 - SDC2_D3	*/ \
		arm_hardware_pioc_updown(UINT32_C(1) << 3, 1*UINT32_C(1) << 3, 0*UINT32_C(1) << 3);	/* PC3 - SDC2_CMD	*/ \
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
	#define SPIHARD_CCU_CLK_SRC_SEL_VAL 0x03	/* t113: 000: HOSC 001: PLL_PERI(1X) 010: PLL_PERI(2X) 011: PLL_AUDIO1(DIV2) 100: PLL_AUDIO1(DIV5) */
	#define HARDWARE_SPI_FREQ (allwnr_t113_get_spi0_freq())
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
	#define	TWIHARD_FREQ (allwnr_t113_get_twi_freq()) // APBS2_CLK allwnr_t507_get_apb2_freq() or allwnr_t507_get_apbs2_freq()


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
	#define	TWIHARD_FREQ (allwnr_t113_get_twi_freq()) // APBS2_CLK allwnr_t507_get_apb2_freq() or allwnr_t507_get_apbs2_freq()

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

	#define	USBD_USBVDD_INITIALIZE() do { \
		arm_hardware_piob_outputs(UINT32_C(1) << 4, 0 * (UINT32_C(1) << 4)); /* PB4 USB_EN SY6280AAC EN input */ \
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
#else

	#define	USBD_USBVDD_INITIALIZE() do { \
		arm_hardware_piob_outputs(UINT32_C(1) << 4, 0 * (UINT32_C(1) << 4)); /* PB4 USB_EN SY6280AAC EN input */ \
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

#if WITHBLPWMCTL
	/* установка яркости и включение/выключение преобразователя подсветки */
	/* Яркость Управлятся через PWM */
	#define HARDWARE_BL_PWMCH 0	/* PWM0 */
	#define HARDWARE_BL_FREQ 	10000	/* Частота PWM управления подсветкой */
	#define	HARDWARE_BL_INITIALIZE() do { \
		const portholder_t ENmask = (UINT32_C(1) << 5); /* PB5 (PWM0, Alt FN 5) */ \
		hardware_dcdcfreq_pwm_initialize(HARDWARE_BL_PWMCH); \
		arm_hardware_piob_altfn2(ENmask, GPIO_CFG_AF5); /* PB5 - PWM7 */ \
	} while (0)
	// en: 0/1, level=WITHLCDBACKLIGHTMIN..WITHLCDBACKLIGHTMAX
	// level=WITHLCDBACKLIGHTMIN не приводит к выключениию подсветки
	#define HARDWARE_BL_SET(en, level) do { \
		hardware_bl_pwm_set_duty(HARDWARE_BL_PWMCH, HARDWARE_BL_FREQ, !! en * ((level) - WITHLCDBACKLIGHTMIN + 1) * 100 / (WITHLCDBACKLIGHTMAX - WITHLCDBACKLIGHTMIN + 1)); \
	} while (0)
#else
	#define	HARDWARE_BL_INITIALIZE() do { \
	} while (0)
	// en: 0/1, level=WITHLCDBACKLIGHTMIN..WITHLCDBACKLIGHTMAX
	// level=WITHLCDBACKLIGHTMIN не приводит к выключениию подсветки
	#define HARDWARE_BL_SET(en, level) do { \
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

	/* управление состоянием сигнала DISP панели */
	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_SET_DISP(state) do { \
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
		arm_hardware_piod_outputs(UINT32_C(1) << 16, 1 * (UINT32_C(1) << 16)); /* LVDS_EN SY6280AAC EN input */ \
	} while (0)

#endif /* WITHLTDCHW */

	#if 1
		#define	TCONLCD_IX 0	/* 0 - TCON_LCD0, 1: TCON_TV0 */
		#define	TCONLCD_PTR TCON_LCD0	/* 0 - TCON_LCD0, 1: TCON_TV0 */
		#define	TCONLCD_CCU_CLK_REG (CCU->TCONLCD_CLK_REG)	/* 0 - TCON_LCD0, 1: TCON_TV0 */
		#define BOARD_TCONLCDFREQ (allwnr_t113_get_tconlcd_freq())
		#define TCONLCD_GINT0_REG (TCON_LCD0->LCD_GINT0_REG)
		#define TCONLCD_IRQ TCON_LCD0_IRQn
		#define TCONLCD_LVDSIX 0	/* 0 -LVDS0 */
		#define RTMIXIDLCD 1	/* 1 or 2 for RTMIX0 or RTMIX1 */
	#endif

	#if 0
		#define	TCONTV_IX 0	/* 0 - TCON_TV0, 1: TCON_TV1 */
		#define	TCONTV_PTR TCON_TV0	/* 0 - TCON_TV0, 1: TCON_TV0 */
		#define	TCONTV_CCU_CLK_REG (CCU->TCONTV_CLK_REG)	/* 0 - TCON_TV0, 1: TCON_TV1 */
		#define	TCONTV_CCU_BGR_REG (CCU->TCONTV_BGR_REG)	/* 0 - TCON_TV0, 1: TCON_TV1 */
		#define TCONTV_IRQ TCON_TV0_IRQn
		#define TCONTV_GINT0_REG (TCON_TV0->TV_GINT0_REG)
		#define BOARD_TCONTVFREQ (allwnr_t113_get_tcontv_freq())
		#define RTMIXIDTV 2	/* 1 or 2 for RTMIX0 or RTMIX1 */
	#endif

	#if 0
		#define	TVENCODER_IX 0	/* 0 -TVE0 */
		#define	TVENCODER_PTR TVE0	/* 0 - TVE0 */
		#define	TVENCODER_BASE TVE0_BASE	/* 0 - TVE0 */
		#define	TVE_CCU_CLK_REG (CCU->TVE_CLK_REG)	/* 0 - TVE0, 1: TVE1 */
		#define	TVE_CCU_BGR_REG (CCU->TVE_BGR_REG)	/* 0 - TVE0, 1: TVE1 */
		#define BOARD_TVEFREQ (allwnr_t113_get_tve_freq())
	#endif

	#if 0
		#define WITHTVDHW 1	/* Use TV Decoder hardware (capture CVBS signal) */
		#define	TVDECODER_IX 0	/* 0 -TVD0 */
		#define	TVDECODER_PTR TVD0	/* 0 - TVD0 */
		#define	TVDECODER_TOP_BASE TVD_TOP_BASE
		#define	TVDECODER_BASE TVD0_BASE	/* 0 - TVD0 */
		#define	TVD_CCU_BGR_REG (CCU->TVD_BGR_REG)	/* 0 - TVE0, 1: TVE1 */
		#define	TVD_CCU_CLK_REG (CCU->TVD_CLK_REG)	/* 0 - TVE0, 1: TVE1 */
		#define WITHTVDHW_INPUT 0
	#endif


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
			arm_hardware_piod_outputs(BOARD_BLINK_BIT, 1 * BOARD_BLINK_BIT); \
		} while (0)
	#define BOARD_BLINK_SETSTATE(state) do { \
			if (state) \
				gpioX_setstate(GPIOD, BOARD_BLINK_BIT, 0 * BOARD_BLINK_BIT); \
			else \
				gpioX_setstate(GPIOD, BOARD_BLINK_BIT, 1 * BOARD_BLINK_BIT); \
		} while (0)
#endif

	/* запрос на вход в режим загрузчика */
	//#define BOARD_USERBOOT_BIT	(UINT32_C(1) << 1)	/* PB1: ~USER_BOOT */
	#define BOARD_IS_USERBOOT() 0//(((GPIOB->DATA) & BOARD_USERBOOT_BIT) == 0 || ((GPIOE->DATA) & TARGET_ENC2BTN_BIT) == 0)

	/* макроопределение, которое должно включить в себя все инициализации */
	#define	HARDWARE_INITIALIZE() do { \
		BOARD_BLINK_INITIALIZE(); \
		/*HARDWARE_KBD_INITIALIZE(); */\
		/*HARDWARE_DAC_INITIALIZE(); */\
		HARDWARE_BL_INITIALIZE(); \
		HARDWARE_DCDC_INITIALIZE(); \
		USBD_USBVDD_INITIALIZE(); \
	} while (0)

	// TUSB parameters
	#define TUP_DCD_ENDPOINT_MAX    6

#endif /* ARM_ALLW_F1333_CPUSTYLE_MANGO_PI_H_INCLUDED */
