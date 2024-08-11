/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Трансивер с DSP обработкой "Аист" на процессоре Allwinner T507
//
// аппаратная платформа разработки RA4ASN, состоящая из основной платы на модуле
// Allwinner HelperBoard T507 Core Board и ПЛИС Xilinx Artix-7 XC7A35TCSG325
// и дочерней платы с АЦП LTC2209 и ЦАП DAC904E

#ifndef ARM_ALW_T507_CPU_VIDEO2_H_INCLUDED
#define ARM_ALW_T507_CPU_VIDEO2_H_INCLUDED 1

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


//#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
//#define WITHSDHC0HW	1		/* TF CARD */
//#define WITHSDHC1HW	1		/* SDIO */
//#define WITHSDHC2HW	1		/* EMMC */

//#define WITHETHHW 1	/* Hardware Ethernet controller */

//#define FPGA_ARTIX7		1

#if WITHDEBUG
    #define WITHDEBUG_UART0    1
    #define WITHUART0HW    1
	//#define WITHUART0HW_FIFO	1	/* испольование FIFO */
#endif /* WITHDEBUG */

#if WITHISBOOTLOADER

	#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
	#define WITHSDHC0HW	1		/* TF CARD */
	//#define WITHSDHC1HW	1		/* SDIO */
	//#define WITHSDHC2HW	1		/* EMMC */

	#define WITHSDRAMHW	1		/* В процессоре есть внешняя память */
	#define BOARD_CONFIG_DRAM_TYPE SUNXI_DRAM_TYPE_LPDDR4
	#define BOARD_CONFIG_DRAM_CLK 792
	#define CONFIG_SUNXI_DRAM_H616_LPDDR4 1

	//#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
	//#define WITHGPUHW	1	/* Graphic processor unit */

	#define USBPHYC_MISC_SWITHOST_VAL 0		// 0 or 1 - value for USBPHYC_MISC_SWITHOST field. 0: Select OTG controller for 2nd PHY port, 1: Select Host controller for 2nd PHY port
	#define USBPHYC_MISC_PPCKDIS_VAL 0x00

	//#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */

	//#define WITHUSBHW_DEVICE	USB20_OTG_DEVICE	/* на этом устройстве поддерживается функциональность DEVICE	*/
	#define WITHUSBDEV_VBUSSENSE	1		/* используется предопределенный вывод OTG_VBUS */
	//#define WITHUSBDEV_HSDESC	1			/* Требуется формировать дескрипторы как для HIGH SPEED */
	//#define WITHUSBDEV_HIGHSPEEDULPI	1	// ULPI
	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USB0_DP & USB0_DM
	//#define WITHUSBDEV_DMAENABLE 1

	//#define WITHTINYUSB 1
	#define BOARD_TUH_RHPORT 1
	//#define WITHEHCIHW	1	/* USB_EHCI controller */
	//#define WITHUSBHW_EHCI		USB20_HOST1_EHCI
	//#define WITHUSBHW_OHCI		USB20_HOST1_OHCI

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

	#define WITHSDRAM_AXP853	1	/* AXP853T power management chip */
	// AXP853T on HelperBoard T507 Core Board
	#define PMIC_I2C_W 0x6C	// 7bit: 0x36
	#define PMIC_I2C_R (PMIC_I2C_W | 0x01)

	// See WITHSDRAM_AXP308
	int axp853_initialize(void);

	/* Контроллер питания AXP305 */
	#define BOARD_PMIC_INITIALIZE() do { \
		axp853_initialize(); /* Voltages are set here */ \
	} while (0)

#else /* WITHISBOOTLOADER */

	//#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
	//#define WITHSDHC0HW	1		/* TF CARD */
	//#define WITHSDHC1HW	1		/* SDIO */
	//#define WITHSDHC2HW	1		/* EMMC */

	#define WITHDCDCFREQCTL	1		// Имеется управление частотой преобразователей блока питания
	//#define WITHBLPWMCTL	1		// Имеется управление яркостью подсветки дисплея через PWM

	#if WITHINTEGRATEDDSP

		//#define WITHFPGAPIPE_CODEC1 1	/* Интерфейс к FPGA, транзитом в аудио кодек через I2S0 */
		#define WITHFPGAPIPE_RTS96 WITHRTS96	/* в том же фрейме иут квадратуры RTS96 */
		#define WITHFPGAPIPE_RTS192 WITHRTS192	/* в том же фрейме иут квадратуры RTS192 */
		#define WITHFPGAPIPE_NCORX0 1	/* управление частотой приемника 1 */
		#define WITHFPGAPIPE_NCORX1 1	/* управление частотой приемника 2 */
		#define WITHFPGAPIPE_NCORTS 1	/* управление частотой приемника панорамы */

		//#define WITHI2S0HW	1	/* I2S0 - 16-ти канальный канал обмена с FPGA */
		//#define WITHI2S1HW	1	/* Использование I2S1 - аудиокодек на I2S */
		//#define WITHI2S2HW	1	/* Использование I2S2 - FPGA или IF codec	*/

		#define WITHAPBIFMAP_RX 0, 1, 1, 2	// Используемые каналы AHUB_APBIF_RX для I2S0, I2S1, I2S2, I2S3.
		#define WITHAPBIFMAP_TX 0, 1, 1, 2	// Используемые каналы AHUB_APBIF_TX для I2S0, I2S1, I2S2, I2S3.

		#define WITHCODEC1_WHBLOCK_DUPLEX_MASTER	1	/* встороенный в процессор кодек */
		//#define WITHFPGAIF_I2S0_DUPLEX_MASTER	1		/* Обмен с FPGA через I2S0 */
		//#define WITHCODEC1_I2S1_DUPLEX_MASTER	1		/* Обмен с аудиокодеком через I2S1 */
		//#define WITHFPGAIF_I2S2_DUPLEX_MASTER	1		/* Обмен с FPGA через I2S2 */
		//#define WITHFPGAIF_I2S0_DUPLEX_SLAVE	1		/* Обмен с FPGA через I2S0 */
		//#define WITHCODEC1_I2S1_DUPLEX_SLAVE	1		/* Обмен с аудиокодеком через I2S1 */
		//#define WITHFPGAIF_I2S2_DUPLEX_SLAVE	1		/* Обмен с FPGA через I2S2 */
	#endif /* WITHINTEGRATEDDSP */

	//#define WITHCPUDACHW	1	/* использование встроенного в процессор DAC */
	#define WITHCPUADCHW 	1	/* использование встроенного в процессор ADC */

	#if ! LCDMODE_DUMMY
		#define WITHMDMAHW		1	/* Использование G2D для формирования изображений */
		#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
		//#define WITHGPUHW	1	/* Graphic processor unit */
		//#define WITHLTDCHWVBLANKIRQ 1	/* Смена framebuffer по прерыванию */
	#endif
	#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */

	#define WITHUSBHW_DEVICE	USB20_OTG_DEVICE	/* на этом устройстве поддерживается функциональность DEVICE	*/
	#define WITHUSBHW_OTG_IRQ	USB20_OTG_DEVICE_IRQn
	#define WITHUSBDEV_VBUSSENSE	1		/* используется предопределенный вывод OTG_VBUS */
	#define WITHUSBDEV_HSDESC	1			/* Требуется формировать дескрипторы как для HIGH SPEED */
	//#define WITHUSBDEV_HIGHSPEEDULPI	1	// ULPI
	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USB0_DP & USB0_DM
	//#define WITHUSBDEV_DMAENABLE 1

//	#define WITHUSBHW_HOST		USBOTG0
//	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USB0_DP & USB0_DM
//	#define WITHUSBHOST_DMAENABLE 1

	#define WITHTINYUSB 1
	#define BOARD_TUH_RHPORT 1
	#define WITHEHCIHW	1	/* USB_EHCI controller */

	#define WITHUSBHW_EHCI		USB20_HOST3_EHCI
	#define WITHUSBHW_EHCI_IRQ	USB20_HOST3_EHCI_IRQn
	#define WITHUSBHW_EHCI_IX	3

	#define WITHUSBHW_OHCI		USB20_HOST3_OHCI
	#define WITHUSBHW_OHCI_IRQ	USB20_HOST3_OHCI_IRQn
	#define WITHUSBHW_OHCI_IX	3

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

	#define I2S0HW_INITIALIZE(master) do { \
		/*arm_hardware_pioi_altfn50(UINT32_C(1) << 0,	GPIO_CFG_AF4); *//* PI0 H_I2S0_MCLK	*/ \
		arm_hardware_pioi_altfn50(UINT32_C(1) << 2,	GPIO_CFG_AF4); /* PI2 H_I2S0_LRCK	*/ \
		arm_hardware_pioi_altfn50(UINT32_C(1) << 1,	GPIO_CFG_AF4); /* PI1 H_I2S0_BCLK	*/ \
		arm_hardware_pioi_altfn50(UINT32_C(1) << 3,	GPIO_CFG_AF4); /* PI3 H_I2S0_DOUT0 to FPGA */ \
		arm_hardware_pioi_altfn50(UINT32_C(1) << 4,	GPIO_CFG_AF4); /* PI4 H_I2S0_DIN0 from FPGA */ \
	} while (0)
	#define HARDWARE_I2S0HW_DIN 0	/* DIN0 used */
	#define HARDWARE_I2S0HW_DOUT 0	/* DOUT0 used */

/* Распределение битов в ARM контроллерах */

#if (WITHCAT && WITHCAT_USART2)
	// CAT data lites
	// RXD at PA10, TXD at PA9

	// CAT control lines
	//#define FROMCAT_TARGET_PIN_RTS		(gpioX_getinputs(GPIOA))
	//#define FROMCAT_BIT_RTS				(UINT32_C(1) << 11)	/* PA11 сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	//#define FROMCAT_TARGET_PIN_DTR		(gpioX_getinputs(GPIOA))
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
	//#define FROMCAT_TARGET_PIN_RTS		(gpioX_getinputs(GPIOA)) // was PINA
	//#define FROMCAT_BIT_RTS				(UINT32_C(1) << 11)	/* сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	//#define FROMCAT_TARGET_PIN_DTR		(gpioX_getinputs(GPIOA)) // was PINA
	//#define FROMCAT_BIT_DTR				(UINT32_C(1) << 12)	/* сигнал DTR от FT232RL	*/

	/* манипуляция от виртуального CDC порта */
	#define FROMCAT_DTR_INITIALIZE() do { \
		} while (0)

	/* переход на передачу от виртуального CDC порта*/
	#define FROMCAT_RTS_INITIALIZE() do { \
		} while (0)

#endif /* (WITHCAT && WITHCAT_CDC) */

#if WITHSDHCHW && WITHSDHC0HW
	// SD CARD
	#define USERFIRSTSBLOCK 0

	// HelerBoard T505 ports:
	// SMHC0: SDC0 - TF CARD
	// SMHC1: SDC1 - SDIO
	// SMHC2: SDC2 - eMMC

	#define	SMHCHARD_IX 0	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_PTR SMHC0	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_BASE SMHC0_BASE	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_CCU_CLK_REG (CCU->SMHC0_CLK_REG)	/* 0 - SMHC0, 1: SMHC1... */
	#define SMHCHARD_FREQ (allwnrt113_get_smhc0_freq())
	#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */

	#define HARDWARE_SDIO_INITIALIZE() do { \
		arm_hardware_piof_altfn50(UINT32_C(1) << 3, GPIO_CFG_AF2);	/* PF3 - SDC0_CMD	*/ \
		arm_hardware_piof_altfn50(UINT32_C(1) << 2, GPIO_CFG_AF2);	/* PF2 - SDC0_CK	*/ \
		arm_hardware_piof_altfn50(UINT32_C(1) << 1, GPIO_CFG_AF2);	/* PF1 - SDC0_D0	*/ \
		arm_hardware_piof_altfn50(UINT32_C(1) << 0, GPIO_CFG_AF2);	/* PF0 - SDC0_D1	*/ \
		arm_hardware_piof_altfn50(UINT32_C(1) << 5, GPIO_CFG_AF2);	/* PF5 - SDC0_D2	*/ \
		arm_hardware_piof_altfn50(UINT32_C(1) << 4, GPIO_CFG_AF2);	/* PF4 - SDC0_D3	*/ \
	} while (0)
	/* отключить процессор от SD карты - чтобы при выполнении power cycle не возникало фантомное питание через сигналы управления. */
	#define HARDWARE_SDIO_HANGOFF()	do { \
		arm_hardware_piof_inputs(UINT32_C(1) << 3);	/* PF3 - SDC0_CMD	*/ \
		arm_hardware_piof_inputs(UINT32_C(1) << 2);	/* PF2 - SDC0_CK	*/ \
		arm_hardware_piof_inputs(UINT32_C(1) << 1);	/* PF1 - SDC0_D0	*/ \
		arm_hardware_piof_inputs(UINT32_C(1) << 0);	/* PF0 - SDC0_D1	*/ \
		arm_hardware_piof_inputs(UINT32_C(1) << 5);	/* PF5 - SDC0_D2	*/ \
		arm_hardware_piof_inputs(UINT32_C(1) << 4);	/* PF4 - SDC0_D3	*/ \
		arm_hardware_piof_updown(UINT32_C(1) << 3, 0, UINT32_C(1) << 3);	/* PF3 - SDC0_CMD	*/ \
		arm_hardware_piof_updown(UINT32_C(1) << 2, 0, UINT32_C(1) << 2);	/* PF2 - SDC0_CK	*/ \
		arm_hardware_piof_updown(UINT32_C(1) << 1, 0, UINT32_C(1) << 1);	/* PF1 - SDC0_D0	*/ \
		arm_hardware_piof_updown(UINT32_C(1) << 0, 0, UINT32_C(1) << 0);	/* PF0 - SDC0_D1	*/ \
		arm_hardware_piof_updown(UINT32_C(1) << 5, 0, UINT32_C(1) << 5);	/* PF5 - SDC0_D2	*/ \
		arm_hardware_piof_updown(UINT32_C(1) << 4, 0, UINT32_C(1) << 4);	/* PF4 - SDC0_D3	*/ \
	} while (0)


	#define HARDWARE_SDIOSENSE_INITIALIZE()	do { \
	} while (0)


	#define HARDWARE_SDIOSENSE_CD() 1//((GPIOF->DATA & HARDWARE_SDIO_CD_BIT) == 0)	/* == 0: no disk. получить состояние датчика CARD PRESENT */
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

	// HelerBoard T505 ports:
	// SMHC0: SDC0 - TF CARD
	// SMHC1: SDC1 - SDIO
	// SMHC2: SDC2 - eMMC

	#define	SMHCHARD_IX 1	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_PTR SMHC1	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_BASE SMHC1_BASE	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_CCU_CLK_REG (CCU->SMHC1_CLK_REG)	/* 0 - SMHC0, 1: SMHC1... */
	#define SMHCHARD_FREQ (allwnrt113_get_smhc1_freq())
	#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */

	// Additional lines to module TL8189FQB2 (Realtek RTL8189FTV based) :
	// PG11 - WL_WAKE_AP 	- pin 13 WL Wake-up host (GPIOB2) - to host
	// PG13 - WL_REG_ON		- pin 12 (tied to GND) Power key (L=OFF, H=ON)
	// PG14 - AP_WAKE_BT 	- pin 06 Host wake (GPIOB5) - from host
	// PG12 - BT_WAKE_AP	- pin 07 WL Wake-up (GPIOB2) - to host
	// PH4 - BT-RST-N		- pin 34 (tied to +3.3) - from host
	// PG10	- AP-CK32KO		- pin 24 (tied to +3.3) (not connected to CPU)

	#define HARDWARE_SDIO_INITIALIZE() do { \
		arm_hardware_piog_outputs(UINT32_C(1) << 13, 1 * UINT32_C(1) << 13); /* PG13 WL_REG_ON */ \
		arm_hardware_piog_altfn50(UINT32_C(1) << 1, GPIO_CFG_AF2);	/* PG1 - SDC1_CMD	*/ \
		arm_hardware_piog_altfn50(UINT32_C(1) << 0, GPIO_CFG_AF2);	/* PG0 - SDC1_CK	*/ \
		arm_hardware_piog_altfn50(UINT32_C(1) << 2, GPIO_CFG_AF2);	/* PG2 - SDC1_D0	*/ \
		arm_hardware_piog_altfn50(UINT32_C(1) << 3, GPIO_CFG_AF2);	/* PG3 - SDC1_D1	*/ \
		arm_hardware_piog_altfn50(UINT32_C(1) << 4, GPIO_CFG_AF2);	/* PG4 - SDC1_D2	*/ \
		arm_hardware_piog_altfn50(UINT32_C(1) << 5, GPIO_CFG_AF2);	/* PG5 - SDC1_D3	*/ \
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

#elif WITHSDHCHW && WITHSDHC2HW
	// eMMC
	#define USERFIRSTSBLOCK 0

	// HelerBoard T505 ports:
	// SMHC0: SDC0 - TF CARD
	// SMHC1: SDC1 - SDIO
	// SMHC2: SDC2 - eMMC

	#define	SMHCHARD_IX 2	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_PTR SMHC2	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_BASE SMHC2_BASE	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_CCU_CLK_REG (CCU->SMHC2_CLK_REG)	/* 0 - SMHC0, 1: SMHC1... */
	#define SMHCHARD_FREQ (allwnrt113_get_smhc2_freq())
	#define WITHSDHCHW8BIT	1	/* Hardware SD HOST CONTROLLER в 8-bit bus width */
	#define WITHSDHCHW1P8V	1	/* 1.8 volt interface */

	#define HARDWARE_SDIO_INITIALIZE() do { \
		arm_hardware_pioc_altfn50(UINT32_C(1) << 6, GPIO_CFG_AF3);	/* PC6 - SDC2_CMD	*/ \
		arm_hardware_pioc_altfn50(UINT32_C(1) << 5, GPIO_CFG_AF3);	/* PC5 - SDC2_CLK	*/ \
		arm_hardware_pioc_altfn50(UINT32_C(1) << 10, GPIO_CFG_AF3);	/* PC10 - SDC2_D0	*/ \
		arm_hardware_pioc_altfn50(UINT32_C(1) << 13, GPIO_CFG_AF3);	/* PC13 - SDC2_D1	*/ \
		arm_hardware_pioc_altfn50(UINT32_C(1) << 15, GPIO_CFG_AF3);	/* PC15 - SDC2_D2	*/ \
		arm_hardware_pioc_altfn50(UINT32_C(1) << 8, GPIO_CFG_AF3);	/* PC8 - SDC2_D3	*/ \
		arm_hardware_pioc_altfn50(UINT32_C(1) << 9, GPIO_CFG_AF3);	/* PC9 - SDC2_D4	*/ \
		arm_hardware_pioc_altfn50(UINT32_C(1) << 11, GPIO_CFG_AF3);	/* PC11 - SDC2_D5	*/ \
		arm_hardware_pioc_altfn50(UINT32_C(1) << 14, GPIO_CFG_AF3);	/* PC14 - SDC2_D6	*/ \
		arm_hardware_pioc_altfn50(UINT32_C(1) << 16, GPIO_CFG_AF3);	/* PC16 - SDC2_D7	*/ \
		arm_hardware_pioc_altfn50(UINT32_C(1) << 0, GPIO_CFG_AF3); /* PC0 - SDC2_DS */ \
		arm_hardware_pioc_altfn50(UINT32_C(1) << 1, GPIO_CFG_AF3); /* PC1 - SDC2_RST */ \
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
	// TXDISABLE input - PE11
	#define TXDISABLE_TARGET_PIN
	#define TXDISABLE_BIT_TXDISABLE				(0)
	// получить бит запрета передачи (от усилителя мощности)
	#define HARDWARE_GET_TXDISABLE() (0) //((TXDISABLE_TARGET_PIN & TXDISABLE_BIT_TXDISABLE) != 0)
	#define TXDISABLE_INITIALIZE() do { \
		} while (0)
	// ---

	// +++
	// PTT input - PD10
	// PTT2 input - PD9
	#define PTT_TARGET_PIN
	#define PTT_BIT_PTT					(0)
	#define PTT2_TARGET_PIN
	#define PTT2_BIT_PTT				(0)
	#define PTT3_TARGET_PIN
	#define PTT3_BIT_PTT				(0)
	// получить бит запроса оператором перехода на пердачу
	#define HARDWARE_GET_PTT() (0)
	#define PTT_INITIALIZE() do { \
		} while (0)
	// ---
	// TUNE input - PD11
	#define TUNE_TARGET_PIN
	#define TUNE_BIT_TUNE				(0)
	#define HARDWARE_GET_TUNE() 0//((TUNE_TARGET_PIN & TUNE_BIT_TUNE) == 0)
	#define TUNE_INITIALIZE() do { \
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
	#define ELKEY_BIT_LEFT				(0)
	#define ELKEY_BIT_RIGHT				(0)

	#define ELKEY_TARGET_PIN

	#define HARDWARE_GET_ELKEY_LEFT() 	(0)
	#define HARDWARE_GET_ELKEY_RIGHT() 	(0)


	#define ELKEY_INITIALIZE() do { \
		} while (0)

#endif /* WITHELKEY */

// IOUPDATE = PA15
//#define SPI_IOUPDATE_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); (void) GPIOA->BSRR; } while (0)
//#define SPI_IOUPDATE_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); (void) GPIOA->BSRR; } while (0)
//#define SPI_IOUPDATE_BIT		(UINT32_C(1) << 15)	// * PA15

#if WITHSPIHW || WITHSPISW
	// Набор определений для работы без внешнего дешифратора

	//#define OE_CTL1_BIT	(UINT32_C(1) << 13)	/* PI13 */
	//#define targetdataflash 0xFF
	#define targetnone 0x00

	#define targetextctl	(UINT32_C(1) << 25)		// PD25
	#define targetnvram		(UINT32_C(1) << 19)		// PD19 nvram FM25W356
	#define targetfpga1		(UINT32_C(1) << 12)		// PI12 FPGA control registers

	/* Select specified chip. */
	#define SPI_CS_ASSERT(target) do { \
		switch (target) { \
		/*case targetdataflash: { gpioX_setstate(GPIOI, SPDIF_NCS_BIT, 0 * (SPDIF_NCS_BIT)); local_delay_us(1); } break; *//* PC3 SPI0_CS */ \
		case targetfpga1: { gpioX_setstate(GPIOI, (target), 0 * (target)); local_delay_us(1); } break; \
		default: { gpioX_setstate(GPIOD, (target), 0 * (target)); local_delay_us(1); } break; \
		case targetnone: break; \
		} \
	} while (0)

	/* Unelect specified chip. */
	#define SPI_CS_DEASSERT(target)	do { \
		switch (target) { \
		/*case targetdataflash: { gpioX_setstate(GPIOI, SPDIF_NCS_BIT, 1 * (SPDIF_NCS_BIT)); local_delay_us(1); } break; *//* PC3 SPI0_CS */ \
		/*case targetrtc1: { gpioX_setstate(GPIOI, (target), 0 * (target)); local_delay_us(1); } break; */\
		case targetfpga1: { gpioX_setstate(GPIOI, (target), 1 * (target)); local_delay_us(1); } break; \
		default: { gpioX_setstate(GPIOD, (target), 1 * (target)); local_delay_us(1); } break; \
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
	#define SPI_ALLCS_INITIALIZE() do { \
		/*arm_hardware_pioc_outputs(SPDIF_NCS_BIT, 1 * SPDIF_NCS_BIT); */	/* PC3 SPI0_CS */ \
		arm_hardware_piod_outputs(targetextctl, 1 * targetextctl); /*  */ \
		arm_hardware_piod_outputs(targetnvram, 1 * targetnvram); /*  */ \
		arm_hardware_pioi_outputs(targetfpga1, 1 * targetfpga1); /*  */ \
	} while (0)

	// MOSI & SCK port
	#define	SPI_SCLK_BIT			(UINT32_C(1) << 6)	// PH6 SPI1_CLK
	#define	SPI_MOSI_BIT			(UINT32_C(1) << 7)	// PH7 SPI1_MOSI
	#define	SPI_MISO_BIT			(UINT32_C(1) << 8)	// PH8 SPI1_MISO

	/* Выводы соединения с QSPI BOOT NOR FLASH */
	//#define SPDIF_SCLK_BIT (UINT32_C(1) << 2)	// PC2 SPI0_CLK
	//#define SPDIF_NCS_BIT (UINT32_C(1) << 3)	// PC3 SPI0_CS
	//#define SPDIF_MOSI_BIT (UINT32_C(1) << 4)	// PC4 SPI0_MOSI
	//#define SPDIF_MISO_BIT (UINT32_C(1) << 5)	// PC5 SPI0_MISO
	//#define SPDIF_D2_BIT (UINT32_C(1) << 6)		// PC6 SPI0_WP/D2
	//#define SPDIF_D3_BIT (UINT32_C(1) << 7)		// PC7 SPI0_HOLD/D3

	#define SPI_TARGET_SCLK_PORT_C(v)	do { gpioX_setstate(GPIOH, (v), !! (0) * (v)); local_delay_us(1); } while (0)
	#define SPI_TARGET_SCLK_PORT_S(v)	do { gpioX_setstate(GPIOH, (v), !! (1) * (v)); local_delay_us(1); } while (0)

	#define SPI_TARGET_MOSI_PORT_C(v)	do { gpioX_setstate(GPIOH, (v), !! (0) * (v)); local_delay_us(1); } while (0)
	#define SPI_TARGET_MOSI_PORT_S(v)	do { gpioX_setstate(GPIOH, (v), !! (1) * (v)); local_delay_us(1); } while (0)

	#define SPI_TARGET_MISO_PIN		gpioX_getinputs(GPIOH)

	#define	SPIHARD_IX 1	/* 0 - SPI0, 1: SPI1... */
	#define	SPIHARD_PTR SPI1	/* 0 - SPI0, 1: SPI1... */
	#define	SPIHARD_CCU_CLK_REG (CCU->SPI1_CLK_REG)	/* 0 - SPI0, 1: SPI1... */
	#define BOARD_SPI_FREQ (allwnrt113_get_spi1_freq())

	#if WITHSPIHW
		#define SPIIO_INITIALIZE() do { \
			arm_hardware_pioh_altfn50(SPI_SCLK_BIT, GPIO_CFG_AF4); 	/* PH6 SPI1_CLK */ \
			arm_hardware_pioh_altfn50(SPI_MOSI_BIT, GPIO_CFG_AF4); 	/* PH7 SPI1_MOSI */ \
			arm_hardware_pioh_altfn50(SPI_MISO_BIT, GPIO_CFG_AF4); 	/* PH8 SPI1_MISO */ \
		} while (0)

	#elif WITHSPISW

		#define SPIIO_INITIALIZE() do { \
			arm_hardware_pioh_outputs(SPI_SCLK_BIT, 1 * SPI_SCLK_BIT); 	/* PH6 SPI1_CLK */ \
			arm_hardware_pioh_outputs(SPI_MOSI_BIT, 1 * SPI_MOSI_BIT); 	/* PH7 SPI1_MOSI */ \
			arm_hardware_pioh_inputs(SPI_MISO_BIT); 	/* PH8 SPI1_MISO */ \
		} while (0)
	#endif
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
// Используется периферийный контроллер последовательного порта UART0 */
#define HARDWARE_UART0_INITIALIZE() do { \
		const portholder_t TXMASK = UINT32_C(1) << 0; /* PH0 UART0-TX */ \
		const portholder_t RXMASK = UINT32_C(1) << 1; /* PH1 UART0-RX - pull-up RX data */  \
		arm_hardware_pioh_altfn2(TXMASK, GPIO_CFG_AF2); \
		arm_hardware_pioh_altfn2(RXMASK, GPIO_CFG_AF2); \
		arm_hardware_pioh_updown(RXMASK, RXMASK, 0); \
	} while (0)

// WITHUART1HW
// Используется периферийный контроллер последовательного порта UART1 (BT BOX) */
#define HARDWARE_UART1_INITIALIZE() do { \
		const portholder_t TXMASK = UINT32_C(1) << 6; /* PG6 UART1-TX */ \
		const portholder_t RXMASK = UINT32_C(1) << 7; /* PG7 UART1-RX - pull-up RX data */  \
		/*arm_hardware_pioh_outputs(UINT32_C(1) << 4, 0); *//* PH4 BT_RESETN */ \
		/*arm_hardware_piog_outputs(UINT32_C(1) << 14, 0); *//* PG14 AP_WKE */ \
		/*arm_hardware_piog_outputs(UINT32_C(1) << 12, 0); *//* PG12 BT_WAKE */ \
		arm_hardware_piog_altfn2(TXMASK, GPIO_CFG_AF2); \
		arm_hardware_piog_altfn2(RXMASK, GPIO_CFG_AF2); \
		arm_hardware_piog_updown(RXMASK, RXMASK, 0); \
	} while (0)

// WITHUART2HW
// Используется периферийный контроллер последовательного порта UART2 */
#define HARDWARE_UART2_INITIALIZE() do { \
		const portholder_t TXMASK = UINT32_C(1) << 15; /* PG15 UART2-TX */ \
		const portholder_t RXMASK = UINT32_C(1) << 16; /* PG16 UART2-RX - pull-up RX data */  \
		arm_hardware_piog_altfn2(TXMASK, GPIO_CFG_AF2); \
		arm_hardware_piog_altfn2(RXMASK, GPIO_CFG_AF2); \
		arm_hardware_piog_updown(RXMASK, RXMASK, 1); \
	} while (0)


#define BOARD_GPIOD_ENC2BTN_BIT (UINT32_C(1) << 21)	// PD21 - second encoder button with pull-up

#if WITHKEYBOARD
	#define TARGET_POWERBTN_BIT 0//(UINT32_C(1) << 8)	// PAxx - ~CPU_POWER_SW signal

#if WITHENCODER2
	#define TARGET_ENC2BTN_GET	(((gpioX_getinputs(GPIOD)) & BOARD_GPIOD_ENC2BTN_BIT) == 0)
#endif /* WITHENCODER2 */

#if WITHPWBUTTON
	// P5_3 - ~CPU_POWER_SW signal
	#define TARGET_POWERBTN_GET	0//(((GPIOx->DATA) & TARGET_POWERBTN_BIT) == 0)
#endif /* WITHPWBUTTON */

	#define HARDWARE_KBD_INITIALIZE() do { \
			arm_hardware_piod_inputs(BOARD_GPIOD_ENC2BTN_BIT); \
			arm_hardware_piod_updown(BOARD_GPIOD_ENC2BTN_BIT, BOARD_GPIOD_ENC2BTN_BIT, 0); /* PD21: pull-up second encoder button */ \
			/*arm_hardware_pioa_inputs(TARGET_POWERBTN_BIT); */ \
			/*arm_hardware_pioa_updown(TARGET_POWERBTN_BIT, TARGET_POWERBTN_BIT, 0);	*//* PAxx: pull-up second encoder button */ \
		} while (0)

#else /* WITHKEYBOARD */

	#define HARDWARE_KBD_INITIALIZE() do { \
		} while (0)

#endif /* WITHKEYBOARD */

#if WITHISBOOTLOADER
	// I2C/TWI
	// BOOTLOASER version
	#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
	//#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */
	// PL0 S-TWI0-SCK - На плате нет pull-up резисторов
	// PL1 S-TWI0-SDA - На плате нет pull-up резисторов
	#define TARGET_TWI_TWCK		(UINT32_C(1) << 0)
	#define TARGET_TWI_TWCK_PIN		(gpioX_getinputs(GPIOL))
	#define TARGET_TWI_TWCK_PORT_C(v) do { gpioX_setopendrain(GPIOL, (v), 0 * (v)); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do { gpioX_setopendrain(GPIOL, (v), 1 * (v)); } while (0)

	#define TARGET_TWI_TWD		(UINT32_C(1) << 1)
	#define TARGET_TWI_TWD_PIN		(gpioX_getinputs(GPIOL))
	#define TARGET_TWI_TWD_PORT_C(v) do { gpioX_setopendrain(GPIOL, (v), 0 * (v)); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v) do { gpioX_setopendrain(GPIOL, (v), 1 * (v)); } while (0)

	// Инициализация битов портов ввода-вывода для программной реализации I2C
	#define	TWISOFT_INITIALIZE() do { \
		arm_hardware_piol_opendrain(TARGET_TWI_TWCK, TARGET_TWI_TWCK); /* SCL */ \
		arm_hardware_piol_opendrain(TARGET_TWI_TWD, TARGET_TWI_TWD);  	/* SDA */ \
		arm_hardware_piol_updown(TARGET_TWI_TWCK, TARGET_TWI_TWCK, 0); \
		arm_hardware_piol_updown(TARGET_TWI_TWD, TARGET_TWI_TWD, 0); \
	} while (0)
	#define	TWISOFT_DEINITIALIZE() do { \
		arm_hardware_piol_inputs(TARGET_TWI_TWCK); 	/* SCL */ \
		arm_hardware_piol_inputs(TARGET_TWI_TWD);	/* SDA */ \
	} while (0)
	// Инициализация битов портов ввода-вывода для аппаратной реализации I2C
	// присоединение выводов к периферийному устройству
	#define	TWIHARD_INITIALIZE() do { \
		arm_hardware_piol_altfn2(TARGET_TWI_TWCK, GPIO_CFG_AF3);	/* PL0 - S_TWI0_SCK */ \
		arm_hardware_piol_altfn2(TARGET_TWI_TWD, GPIO_CFG_AF3);		/* PL1 - S_TWI0_SDA */ \
		arm_hardware_piol_updown(TARGET_TWI_TWCK, TARGET_TWI_TWCK, 0); \
		arm_hardware_piol_updown(TARGET_TWI_TWD, TARGET_TWI_TWD, 0); \
	} while (0)
	#define	TWIHARD_IX 0	/* 0 - TWI0, 1: TWI1... */
	#define	TWIHARD_PTR S_TWI0	/* 0 - TWI0, 1: TWI1... */
	#define	TWIHARD_FREQ (allwnrt113_get_s_twi_freq()) // APBS2_CLK allwnr_t507_get_apb2_freq() or allwnr_t507_get_apbs2_freq()

#else /* WITHISBOOTLOADER */


	#if 1
		// Toshiba, TP
		#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
		//#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */
		// PE20 - TWI2-SCK
		// PE21 - TWI2_SDA
		#define TARGET_TWI_TWCK		(UINT32_C(1) << 20)
		#define TARGET_TWI_TWCK_PIN		(gpioX_getinputs(GPIOE))
		#define TARGET_TWI_TWCK_PORT_C(v) do { gpioX_setopendrain(GPIOE, (v), 0 * (v)); } while (0)
		#define TARGET_TWI_TWCK_PORT_S(v) do { gpioX_setopendrain(GPIOE, (v), 1 * (v)); } while (0)

		#define TARGET_TWI_TWD		(UINT32_C(1) << 21)
		#define TARGET_TWI_TWD_PIN		(gpioX_getinputs(GPIOE))
		#define TARGET_TWI_TWD_PORT_C(v) do { gpioX_setopendrain(GPIOE, (v), 0 * (v)); } while (0)
		#define TARGET_TWI_TWD_PORT_S(v) do { gpioX_setopendrain(GPIOE, (v), 1 * (v)); } while (0)

		// Инициализация битов портов ввода-вывода для программной реализации I2C
		#define	TWISOFT_INITIALIZE() do { \
				arm_hardware_pioe_opendrain(TARGET_TWI_TWCK, TARGET_TWI_TWCK);  /* SCL */ \
				arm_hardware_pioe_opendrain(TARGET_TWI_TWD, TARGET_TWI_TWD);  	/* SDA */ \
			} while (0)
		#define	TWISOFT_DEINITIALIZE() do { \
				arm_hardware_pioe_inputs(TARGET_TWI_TWCK); 	/* SCL */ \
				arm_hardware_pioe_inputs(TARGET_TWI_TWD);	/* SDA */ \
			} while (0)
		// Инициализация битов портов ввода-вывода для аппаратной реализации I2C
		// присоединение выводов к периферийному устройству
		#define	TWIHARD_INITIALIZE() do { \
			arm_hardware_pioe_altfn2(TARGET_TWI_TWCK, GPIO_CFG_AF5);	/* PH2 - TWI2_SCL */ \
			arm_hardware_pioe_altfn2(TARGET_TWI_TWD, GPIO_CFG_AF5);		/* PH3 - TWI2_SDA */ \
			arm_hardware_pioe_updown(TARGET_TWI_TWCK, TARGET_TWI_TWCK, 0); \
			arm_hardware_pioe_updown(TARGET_TWI_TWD, TARGET_TWI_TWD, 0); \
			} while (0)
		#define	TWIHARD_IX 2	/* 0 - TWI0, 1: TWI1... */
		#define	TWIHARD_PTR TWI2	/* 0 - TWI0, 1: TWI1... */
		#define	TWIHARD_FREQ (allwnrt113_get_twi_freq()) // APBS2_CLK allwnr_t507_get_apb2_freq() or allwnr_t507_get_apbs2_freq()

	#endif

	#if 1
		// MEMS addr8bit=0xD0, addr7bit=0x68
		#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
		//#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */
		// PG15 - TWI4-SCK
		// PG16 - TWI4_SDA
		#define TARGET_TWI2_TWCK		(UINT32_C(1) << 15)
		#define TARGET_TWI2_TWCK_PIN		(gpioX_getinputs(GPIOG))
		#define TARGET_TWI2_TWCK_PORT_C(v) do { gpioX_setopendrain(GPIOG, (v), 0 * (v)); } while (0)
		#define TARGET_TWI2_TWCK_PORT_S(v) do { gpioX_setopendrain(GPIOG, (v), 1 * (v)); } while (0)

		#define TARGET_TWI2_TWD		(UINT32_C(1) << 16)
		#define TARGET_TWI2_TWD_PIN		(gpioX_getinputs(GPIOG))
		#define TARGET_TWI2_TWD_PORT_C(v) do { gpioX_setopendrain(GPIOE, (v), 0 * (v)); } while (0)
		#define TARGET_TWI2_TWD_PORT_S(v) do { gpioX_setopendrain(GPIOE, (v), 1 * (v)); } while (0)

		// Инициализация битов портов ввода-вывода для программной реализации I2C
		#define	TWISOFT2_INITIALIZE() do { \
				arm_hardware_piog_opendrain(TARGET_TWI2_TWCK, TARGET_TWI_TWCK);  /* SCL */ \
				arm_hardware_piog_opendrain(TARGET_TWI2_TWD, TARGET_TWI_TWD);  	/* SDA */ \
			} while (0)
		#define	TWISOFT2_DEINITIALIZE() do { \
				arm_hardware_piog_inputs(TARGET_TWI2_TWCK); 	/* SCL */ \
				arm_hardware_piog_inputs(TARGET_TWI2_TWD);	/* SDA */ \
			} while (0)
		// Инициализация битов портов ввода-вывода для аппаратной реализации I2C
		// присоединение выводов к периферийному устройству
		#define	TWIHARD2_INITIALIZE() do { \
			arm_hardware_piog_altfn2(TARGET_TWI2_TWCK, GPIO_CFG_AF5);	/* PG15 - TWI4-SCK */ \
			arm_hardware_piog_altfn2(TARGET_TWI2_TWD, GPIO_CFG_AF5);		/* PG16 - TWI4_SDA */ \
			} while (0)
		#define	TWIHARD2_IX 4	/* 0 - TWI0, 1: TWI1... */
		#define	TWIHARD2_PTR TWI4	/* 0 - TWI0, 1: TWI1... */
		#define	TWIHARD2_FREQ (allwnrt113_get_twi_freq()) // APBS2_CLK allwnr_t507_get_apb2_freq() or allwnr_t507_get_apbs2_freq()

	#endif

#endif /* WITHISBOOTLOADER */

#if WITHCPUDACHW
#else /* WITHCPUDACHW */
	#define HARDWARE_DAC_INITIALIZE() do { \
		} while (0)

#endif /* WITHCPUDACHW */

#if WITHCPUADCHW
	#define HARDWARE_ADC_INITIALIZE(ainmask) do { \
		} while (0)
#endif /* WITHCPUADCHW */

#if WITHUSBHW

	#define TARGET_GPIOE_VBUSON_BIT (0)	0
	#define	USBD_EHCI_INITIALIZE() do { \
	} while (0)

	#define TARGET_USBFS_VBUSON_SET(on)	do { \
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

#else /* WITHUSBHW */

	#define	USBD_EHCI_INITIALIZE() do { \
	} while (0)

#endif /* WITHUSBHW */

#if WITHDCDCFREQCTL
	// ST ST1S10 Synchronizable switching frequency from 400 kHz up to 1.2 MHz
	#define WITHHWDCDCFREQMIN 400000L
	#define WITHHWDCDCFREQMAX 1200000L
	#define HARDWARE_DCDC_PWMCH 0	/* PWM0 */
	#define HARDWARE_BL_PWMCH 0	/* PWM0 */
	#define HARDWARE_BL_FREQ 	10000	/* Частота PWM управления подсветкой */

	// PD28 - DC-DC synchro output
	// PWM0 AF2
	#define	HARDWARE_BACKLIGHT_INITIALIZE() do { \
		hardware_dcdcfreq_pwm_initialize(HARDWARE_DCDC_PWMCH); \
		arm_hardware_piod_altfn2((0*UINT32_C(1) << 28), GPIO_CFG_AF2); /* PD28 - PWM0 */ \
		arm_hardware_piod_outputs(1*(UINT32_C(1) << 28), 1 * (UINT32_C(1) << 28)); /* PD28 - PWM0 - TE - TE_18 */ \
	} while (0)
	#define HARDWARE_DCDC_SETDIV(f) do { \
		hardware_dcdcfreq_pwm_setdiv(HARDWARE_DCDC_PWMCH, f); \
	} while (0)
#else /* WITHDCDCFREQCTL */
	#define	HARDWARE_BACKLIGHT_INITIALIZE() do { \
		arm_hardware_piod_outputs(0*(UINT32_C(1) << 28), 1 * (UINT32_C(1) << 28)); /* PD28 - PWM0 */ \
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

	#define	HARDWARE_BL_INITIALIZE() do { \
	} while (0)

	/* установка яркости и включение/выключение преобразователя подсветки */
	#define HARDWARE_BL_SET(en, level) do { \
	} while (0)


#define HARDWARE_TP_INIT() do { \
		const portholder_t TE = (UINT32_C(1) << 7);	/* PC7 (TE) - panel pin 29 Sync signal from driver IC */ \
		const portholder_t OTP_PWR = (UINT32_C(1) << 7);	/* PD7 (CTRL - OTP_PWR) - panel pin 30 */ \
		arm_hardware_pioc_inputs(TE); \
		arm_hardware_piod_outputs(OTP_PWR, 1 * OTP_PWR); \
		const portholder_t RESET = (UINT32_C(1) << 1);	/* PD1 = RESX_18 - pin  28 */ \
		arm_hardware_piod_outputs(RESET, 0 * RESET); \
		local_delay_ms(5); \
		arm_hardware_piod_outputs(RESET, 1 * RESET); \
	} while (0)

// TC358778XBG conrol
//	x-gpios = <&gpioa 10 GPIO_ACTIVE_HIGH>; /* Video_RST */
//	x-gpios = <&gpiof 14 GPIO_ACTIVE_HIGH>; /* Video_MODE: 0: test, 1: normal */
#define HARDWARE_VIDEO_INIT() do { \
		const portholder_t Video_RST = (UINT32_C(1) << 13);	/* PE13 */ \
		const portholder_t Video_MODE = (UINT32_C(1) << 14);	/* PE14: Video_MODE: 0: test, 1: normal */ \
		arm_hardware_pioe_outputs(UINT32_C(1) << 19, 0 * (UINT32_C(1) << 19)); /* PE19 - V_3V_EN enable Toshiba power */ \
		arm_hardware_piod_outputs(UINT32_C(1) << 28, 1 * (UINT32_C(1) << 28)); /* PD28 - PWM0 - TE - TE_18 */ \
		local_delay_ms(100); \
		arm_hardware_pioe_outputs(Video_MODE, Video_MODE); /* Video_MODE: 0: test, 1: normal */ \
		\
		arm_hardware_pioe_outputs(UINT32_C(1) << 16, 0 * (UINT32_C(1) << 16)); /* PE16 CTRL TP_RESX_18 */ \
		arm_hardware_pioe_outputs(UINT32_C(1) << 17, 0 * (UINT32_C(1) << 17)); /* PE17 RESET_Disp - RESX_18 */ \
		arm_hardware_pioe_outputs(Video_RST, 0 * Video_RST); \
		local_delay_ms(5); \
		arm_hardware_pioe_outputs(UINT32_C(1) << 16, 1 * (UINT32_C(1) << 16)); /* PE16 CTRL TP_RESX_18 */ \
		arm_hardware_pioe_outputs(UINT32_C(1) << 17, 1 * (UINT32_C(1) << 17)); /* PE17 RESET_Disp - RESX_18 */ \
		arm_hardware_pioe_outputs(Video_RST, 1 * Video_RST); \
		local_delay_ms(200); \
	} while (0)

#define HARDWARE_VODEO_DEINIT() do { \
		const portholder_t Video_RST = (UINT32_C(1) << 13);	/* PE13 */ \
		const portholder_t Video_MODE = (UINT32_C(1) << 14);	/* PE14: Video_MODE: 0: test, 1: normal */ \
		arm_hardware_pioe_outputs(Video_MODE, Video_MODE); \
		arm_hardware_pioe_outputs(Video_RST, 0 * Video_RST);	/* RESET active */ \
		local_delay_ms(5); \
	} while (0)

// PMIC control
#define HARDWARE_TC358768_POWERON(state) do { \
		if ((state) != 0) { \
		} else { \
		} \
	} while (0)

#if WITHLTDCHW

	// LCD FRM Control Register (Default Value: 0x0000_0000)
	//#define TCON_FRM_MODE_VAL 0//((UINT32_C(1) << 31) | (UINT32_C(1) << 6) | (UINT32_C(0) << 5) | (UINT32_C(1) << 4))	// 16 bit panel connected
	#define TCON_FRM_MODE_VAL 0//((UINT32_C(1) << 31) | (UINT32_C(0) << 6) | (UINT32_C(0) << 5)| (UINT32_C(0) << 4))	// 18 bit panel connected
	//#define TCON_FRM_MODE_VAL 0	// 24 bit panel

	#if LCDMODETX_TC358778XBG

		/* demode values: 0: static signal, 1: DE controlled */
		#define HARDWARE_LTDC_INITIALIZE(demode) do { \
			const portholder_t VSmask = (UINT32_C(1) << 27); 	/* PD27 LCD_VSYNC */ \
			const portholder_t HSmask = (UINT32_C(1) << 26); 	/* PD26 LCD_HSYNC */ \
			const portholder_t DEmask = (UINT32_C(1) << 25); 	/* PD25 LCD_DE */ \
			/* synchro signals */ \
			arm_hardware_piod_altfn50(DEmask, GPIO_CFG_AF2); /* PD25 LCD_DE */ \
			arm_hardware_piod_altfn50(VSmask, GPIO_CFG_AF2); /* PD27 LCD_VSYNC */ \
			arm_hardware_piod_altfn50(HSmask, GPIO_CFG_AF2); /* PD26 LCD_HSYNC */ \
			/* pixel clock */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 24, GPIO_CFG_AF2); /* PD24 LCD_CLK */ \
			/* RED */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 16, GPIO_CFG_AF2); /* R0 PD16 LCD_D16 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 17, GPIO_CFG_AF2); /* R1 PD17 LCD_D17 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 18, GPIO_CFG_AF2); /* R2 PD18 LCD_D18 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 19, GPIO_CFG_AF2); /* R3 PD19 LCD_D19 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 20, GPIO_CFG_AF2); /* R4 PD20 LCD_D20 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 21, GPIO_CFG_AF2); /* R5 PD21 LCD_D21 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 22, GPIO_CFG_AF2); /* R6 PD22 LCD_D22 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 23, GPIO_CFG_AF2); /* R7 PD23 LCD_D23 */ \
			/* GREEN */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 8, GPIO_CFG_AF2); 	/* G0 PD8 LCD_D8 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 9, GPIO_CFG_AF2); 	/* G1 PD9 LCD_D9 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 10, GPIO_CFG_AF2); /* G2 PD10 LCD_D10 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 11, GPIO_CFG_AF2); /* G3 PD11 LCD_D11 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 12, GPIO_CFG_AF2); /* G4 PD12 LCD_D12 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 13, GPIO_CFG_AF2); /* G5 PD13 LCD_D13 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 14, GPIO_CFG_AF2); /* G6 PD14 LCD_D14 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 15, GPIO_CFG_AF2); /* G7 PD15 LCD_D15 */ \
			/* BLUE  */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 0, GPIO_CFG_AF2); 	/* B0 PD3 LCD_D0 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 1, GPIO_CFG_AF2); 	/* B1 PD3 LCD_D1 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 2, GPIO_CFG_AF2); 	/* B2 PD3 LCD_D2 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 3, GPIO_CFG_AF2); 	/* B3 PD3 LCD_D3 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 4, GPIO_CFG_AF2); 	/* B4 PD4 LCD_D4 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 5, GPIO_CFG_AF2); 	/* B5 PD5 LCD_D5 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 6, GPIO_CFG_AF2); 	/* B6 PD6 LCD_D6 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 7, GPIO_CFG_AF2); 	/* B7 PD7 LCD_D7 */ \
		} while (0)

	#else /* LCDMODETX_TC358778XBG */

		/* demode values: 0: static signal, 1: DE controlled */
		#define HARDWARE_LTDC_INITIALIZE(demode) do { \
			const portholder_t VSmask = (UINT32_C(1) << 27); 	/* PD27 LCD_VSYNC */ \
			const portholder_t HSmask = (UINT32_C(1) << 26); 	/* PD26 LCD_HSYNC */ \
			const portholder_t DEmask = (UINT32_C(1) << 25); 	/* PD25 LCD_DE */ \
			/* synchro signals - sync mode */ \
			arm_hardware_piod_outputs(((demode) == 0) * DEmask, 0 * DEmask); /* PD25 LCD_DE */ \
			arm_hardware_piod_altfn50(((demode) == 0) * VSmask, GPIO_CFG_AF2); /* PD27 LCD_VSYNC */ \
			arm_hardware_piod_altfn50(((demode) == 0) * HSmask, GPIO_CFG_AF2); /* PD26 LCD_HSYNC */ \
			/* synchro signals - DE mode */ \
			arm_hardware_piod_altfn50(((demode) != 0) * DEmask, GPIO_CFG_AF2); /* PD25 LCD_DE */ \
			arm_hardware_piod_outputs(((demode) != 0) * VSmask, 1 * VSmask); /* PD27 LCD_VSYNC */ \
			arm_hardware_piod_outputs(((demode) != 0) * HSmask, 1 * HSmask); /* PD25 LCD_HSYNC */ \
			/* pixel clock */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 24, GPIO_CFG_AF2); /* PD24 LCD_CLK */ \
			/* RED */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 16, GPIO_CFG_AF2); /* R0 PD16 LCD_D16 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 17, GPIO_CFG_AF2); /* R1 PD17 LCD_D17 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 18, GPIO_CFG_AF2); /* R2 PD18 LCD_D18 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 19, GPIO_CFG_AF2); /* R3 PD19 LCD_D19 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 20, GPIO_CFG_AF2); /* R4 PD20 LCD_D20 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 21, GPIO_CFG_AF2); /* R5 PD21 LCD_D21 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 22, GPIO_CFG_AF2); /* R6 PD22 LCD_D22 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 23, GPIO_CFG_AF2); /* R7 PD23 LCD_D23 */ \
			/* GREEN */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 8, GPIO_CFG_AF2); 	/* G0 PD8 LCD_D8 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 9, GPIO_CFG_AF2); 	/* G1 PD9 LCD_D9 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 10, GPIO_CFG_AF2); /* G2 PD10 LCD_D10 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 11, GPIO_CFG_AF2); /* G3 PD11 LCD_D11 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 12, GPIO_CFG_AF2); /* G4 PD12 LCD_D12 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 13, GPIO_CFG_AF2); /* G5 PD13 LCD_D13 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 14, GPIO_CFG_AF2); /* G6 PD14 LCD_D14 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 15, GPIO_CFG_AF2); /* G7 PD15 LCD_D15 */ \
			/* BLUE  */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 0, GPIO_CFG_AF2); 	/* B0 PD3 LCD_D0 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 1, GPIO_CFG_AF2); 	/* B1 PD3 LCD_D1 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 2, GPIO_CFG_AF2); 	/* B2 PD3 LCD_D2 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 3, GPIO_CFG_AF2); 	/* B3 PD3 LCD_D3 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 4, GPIO_CFG_AF2); 	/* B4 PD4 LCD_D4 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 5, GPIO_CFG_AF2); 	/* B5 PD5 LCD_D5 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 6, GPIO_CFG_AF2); 	/* B6 PD6 LCD_D6 */ \
			arm_hardware_piod_altfn50(UINT32_C(1) << 7, GPIO_CFG_AF2); 	/* B7 PD7 LCD_D7 */ \
		} while (0)

	#endif /* LCDMODETX_TC358778XBG */

	/* управление состоянием сигнала DISP панели */
	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_SET_DISP(state) do { \
		const portholder_t VSmask = (UINT32_C(1) << 27); 	/* PD27 LCD_VSYNC */ \
		const portholder_t HSmask = (UINT32_C(1) << 26); 	/* PD26 LCD_HSYNC */ \
		const portholder_t DEmask = (UINT32_C(1) << 25); 	/* PD25 LCD_DE */ \
		const portholder_t MODEmask = (UINT32_C(1) << 9); 	/* PA9 mode */ \
		arm_hardware_piod_outputs(VSmask, 0 * VSmask); /* PD27 LCD_VSYNC */ \
		local_delay_ms(5); \
		/* while ((gpioX_getinputs(GPIOD) & VSmask) != 0) ; */ /* схема синхронизации стоит на плате дисплея. дождаться 0 */ \
		/* while ((gpioX_getinputs(GPIOD) & VSmask) == 0) ; */ /* дождаться 1 */ \
		arm_hardware_piod_outputs(DEmask, ((state) != 0) * DEmask); /* DE=DISP, pin 31 - можно менять только при VSYNC=1 */ \
		local_delay_ms(5); \
		arm_hardware_piod_altfn20(VSmask, GPIO_CFG_AF2); /* PD27 LCD_VSYNC */ \
	} while (0)

	#define LCD_LVDS_IF_REG_VALUE ( \
		(UINT32_C(1) << 31) |	/* LCD_LVDS_EN */ \
		(UINT32_C(0) << 30) |	/* LCD_LVDS_LINK: 0: single link */ \
		(! UINT32_C(1) << 27) |	/* LCD_LVDS_MODE 1: JEIDA mode (0 for THC63LVDF84B converter) */ \
		(UINT32_C(0) << 26) |	/* LCD_LVDS_BITWIDTH 0: 24-bit */ \
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

	#define	TCONLCD_IX 0	/* 0 - TCON_LCD0, 1: TCON_LCD1 */
	#define	TCONLCD_PTR TCON_LCD0	/* 0 - TCON_LCD0, 1: TCON_LCD1 */
	#define	TCONLCD_CCU_CLK_REG (CCU->TCON_LCD0_CLK_REG)	/* 0 - TCON_LCD0, 1: TCON_LCD1 */
	#define BOARD_TCONLCDFREQ (allwnr_t507_get_tcon_lcd0_freq())
	#define TCONLCD_LVDSIX 0	/* 0 -LVDS0 */

//	#define	TCONTV_IX 0	/* 0: TCON_TV0, 2: TCON_TV1 */
//	#define TCONTV_PTR TCON_TV0
//	#define	TCONTV_CCU_CLK_REG (CCU->TCON_TV0_CLK_REG)	/* 0 - TCON_LCD0, 1: TCON_LCD1, 2: TCON_TV0, 3: TCON_TV1 */
//	#define BOARD_TCONTVFREQ (allwnr_t507_get_tcon_tvd0_freq())

#endif /* WITHLTDCHW */


	#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_GT911)

		//	tsc interrupt PD27
		//	tsc reset PD26

		void gt911_interrupt_handler(void * ctx);

		#define BOARD_GT911_INT_PIN 	(UINT32_C(1) << 27)
		#define BOARD_GT911_RESET_PIN 	(UINT32_C(1) << 26)

		#define BOARD_GT911_RESET_SET(v) do { gpioX_setstate(GPIOD, BOARD_GT911_RESET_PIN, !! (v) * BOARD_GT911_RESET_PIN); } while (0)
		#define BOARD_GT911_INT_SET(v) do { gpioX_setstate(GPIOD, BOARD_GT911_INT_PIN, !! (v) * BOARD_GT911_INT_PIN); } while (0)

		#define BOARD_GT911_RESET_INITIO_1() do { \
			arm_hardware_piod_outputs2m(BOARD_GT911_INT_PIN, 1 * BOARD_GT911_INT_PIN); \
			arm_hardware_piod_outputs2m(BOARD_GT911_RESET_PIN, 1 * BOARD_GT911_RESET_PIN); \
		} while (0)

		#define BOARD_GT911_RESET_INITIO_2() do { \
			arm_hardware_piod_inputs(BOARD_GT911_INT_PIN); \
			arm_hardware_piod_updown(BOARD_GT911_INT_PIN, BOARD_GT911_INT_PIN, 0); \
		} while (0)

		#define BOARD_GT911_INT_CONNECT() do { \
			static einthandler_t h; \
			arm_hardware_piod_inputs(BOARD_GT911_INT_PIN); \
			arm_hardware_piod_updown(BOARD_GT911_INT_PIN, BOARD_GT911_INT_PIN, 0); \
			einthandler_initialize(& h, 1*BOARD_PPSIN_BIT, gt911_interrupt_handler, NULL); \
			arm_hardware_piod_onchangeinterrupt(BOARD_GT911_INT_PIN, 1*BOARD_GT911_INT_PIN, 0 * BOARD_GT911_INT_PIN, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM, & h); \
		} while (0)
		//gt911_interrupt_handler

	#endif

	#define BOARD_BLINK_BIT0 (UINT32_C(1) << 10) // PH10
	#define BOARD_BLINK_BIT1 (UINT32_C(1) << 10)	// PA10 - VD24 - active "1"

	#define BOARD_BLINK_INITIALIZE() do { \
		arm_hardware_pioh_outputs(BOARD_BLINK_BIT0, 1 * BOARD_BLINK_BIT0); \
		arm_hardware_pioa_outputs(BOARD_BLINK_BIT1, 1 * BOARD_BLINK_BIT1); \
	} while (0)
	#define BOARD_BLINK_SETSTATE(state) do { \
		gpioX_setstate(GPIOH, BOARD_BLINK_BIT0, !! (state) * BOARD_BLINK_BIT0); \
		gpioX_setstate(GPIOA, BOARD_BLINK_BIT1, !! (state) * BOARD_BLINK_BIT1); \
	} while (0)

	/* запрос на вход в режим загрузчика */
	#define BOARD_GPIOD_USERBOOT_BIT	BOARD_GPIOD_ENC2BTN_BIT	/* PD21: ~USER_BOOT - same as BOARD_GPIOA_ENC2BTN_BIT */
	#define BOARD_IS_USERBOOT() (((gpioX_getinputs(GPIOD)) & BOARD_GPIOD_USERBOOT_BIT) == 0)
	#define BOARD_USERBOOT_INITIALIZE() do { \
			arm_hardware_piod_inputs(BOARD_GPIOD_USERBOOT_BIT); /* set as input with pull-up */ \
		} while (0)

#if WITHETHHW && 0

	//PA0 - PA9 EMAC RMII
	#define ETHERNET_INITIALIZE() do { \
		arm_hardware_pioa_altfn50(1 << 0, GPIO_CFG_AF2);	\
		arm_hardware_pioa_altfn50(1 << 1, GPIO_CFG_AF2);	\
		arm_hardware_pioa_altfn50(1 << 2, GPIO_CFG_AF2);	\
		arm_hardware_pioa_altfn50(1 << 3, GPIO_CFG_AF2);	\
		arm_hardware_pioa_altfn50(1 << 4, GPIO_CFG_AF2);	\
		arm_hardware_pioa_altfn50(1 << 5, GPIO_CFG_AF2);	\
		arm_hardware_pioa_altfn50(1 << 6, GPIO_CFG_AF2);	\
		arm_hardware_pioa_altfn50(1 << 7, GPIO_CFG_AF2);	\
		arm_hardware_pioa_altfn50(1 << 8, GPIO_CFG_AF2);	\
		arm_hardware_pioa_altfn50(1 << 9, GPIO_CFG_AF2);	\
	} while (0)

#else
	#define ETHERNET_INITIALIZE() do { } while (0)
#endif /* WITHETHHW */

	/* макроопределение, которое должно включить в себя все инициализации */
	#define	HARDWARE_INITIALIZE() do { \
		BOARD_BLINK_INITIALIZE(); \
		HARDWARE_KBD_INITIALIZE(); \
		/*HARDWARE_DAC_INITIALIZE(); */\
		HARDWARE_BACKLIGHT_INITIALIZE(); \
		ETHERNET_INITIALIZE(); \
		USBD_EHCI_INITIALIZE(); \
	} while (0)

	// TUSB parameters
	#define TUP_DCD_ENDPOINT_MAX    6

#endif /* ARM_ALW_T507_CPU_VIDEO2_H_INCLUDED */
