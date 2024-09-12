/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Трансивер с DSP обработкой "Аист" на процессоре Allwinner T507
// Rrfboard_v0.pcb Allwinner T507, 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
// HelperBoard T507 Core Board

#ifndef ARM_ALW_T507_CPU_VELOCI_V0_H_INCLUDED
#define ARM_ALW_T507_CPU_VELOCI_V0_H_INCLUDED 1

#define WITHSPI16BIT	1	/* возможно использование 16-ти битных слов при обмене по SPI */
#define WITHSPI32BIT	1	/* возможно использование 32-ти битных слов при обмене по SPI */
#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
//#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
//#define WITHSPISW 	1	/* Использование программного управления SPI. Нельзя убирать эту строку - требуется явное отключение из-за конфликта с I2C */

//#define WIHSPIDFSW	1	/* программное обслуживание DATA FLASH */
//#define WIHSPIDFOVERSPI 1	/* Для работы используется один из обычных каналов SPI */
//#define WIHSPIDFHW		1	/* аппаратное обслуживание DATA FLASH */
//#define WIHSPIDFHW2BIT	1	/* аппаратное обслуживание DATA FLASH с поддержкой QSPI подключения по 2-м проводам */
//#define WIHSPIDFHW4BIT	1	/* аппаратное обслуживание DATA FLASH с поддержкой QSPI подключения по 4-м проводам */

//#define WITHDMA2DHW		1	/* Использование DMA2D для формирования изображений	- у STM32MP1 его нет */

#define WITHETHHW 1	/* Hardware Ethernet controller */


#if WITHDEBUG
	#define WITHDEBUG_UART0	1
	#define WITHUART0HW	1		/* отлдочный порт */
#endif /* WITHDEBUG */

//#define WITHUART5HW_FIFO	1	/* испольование FIFO */
#define WITHUART5HW	1		/* mini dinn8	*/
//#define WITHCAT_UART5 1

#define WITHCAT_MUX 1		/* переключаемый USB UAC или UART канал управления. */


// OHCI at USB1HSFSP2_BASE
////#define WITHUSBHW_OHCI ((struct ohci_registers *) USB1HSFSP2_BASE)

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
	
	//#define WITHUSBHW_EHCI		USB20_HOST1_EHCI
	//#define WITHUSBHW_OHCI		USB20_HOST1_OHCI

	#define WITHEHCIHW_EHCIPORT 0	// 0 - use 1st PHY port
	#define WITHOHCIHW_OHCIPORT 0


//	#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
//	#define WITHMODEM_CDC	1

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
	#define WITHBLPWMCTL	1		// Имеется управление яркостью подсветки дисплея через PWM

	#if WITHINTEGRATEDDSP

		#define WITHFPGAPIPE_CODEC1 1	/* Интерфейс к FPGA, транзитом в аудио кодек через I2S0 */
		#define WITHFPGAPIPE_RTS96 WITHRTS96	/* в том же фрейме идут квадратуры RTS96 */
		#define WITHFPGAPIPE_RTS192 WITHRTS192	/* в том же фрейме идут квадратуры RTS192 */
		#define WITHFPGAPIPE_NCORX0 1	/* управление частотой приемника 1 */
		#define WITHFPGAPIPE_NCORX1 1	/* управление частотой приемника 2 */
		#define WITHFPGAPIPE_NCORTS 1	/* управление частотой приемника панорамы */

		#define WITHI2S0HW	1	/* I2S0 - 16-ти канальный канал обмена с FPGA */
		//#define WITHI2S1HW	1	/* Использование I2S1 - аудиокодек на I2S */
		//#define WITHI2S2HW	1	/* Использование I2S2 - FPGA или IF codec	*/

		#define WITHAPBIFMAP_RX 0, 1, 1, 2	// Используемые каналы AHUB_APBIF_RX для I2S0, I2S1, I2S2, I2S3.
		#define WITHAPBIFMAP_TX 0, 1, 1, 2	// Используемые каналы AHUB_APBIF_TX для I2S0, I2S1, I2S2, I2S3.

		//#define HARDWARE_I2S0HW_USEDMA 0	// test

		//#define WITHCODEC1_WHBLOCK_DUPLEX_MASTER	1	/* встороенный в процессор кодек */
		//#define WITHFPGAIF_I2S0_DUPLEX_MASTER	1		/* Обмен с FPGA через I2S0 */
		//#define WITHCODEC1_I2S1_DUPLEX_MASTER	1		/* Обмен с аудиокодеком через I2S1 */
		//#define WITHFPGAIF_I2S2_DUPLEX_MASTER	1		/* Обмен с FPGA через I2S2 */
		#define WITHFPGAIF_I2S0_DUPLEX_SLAVE	1		/* Обмен с FPGA через I2S0 */
		//#define WITHCODEC1_I2S1_DUPLEX_SLAVE	1		/* Обмен с аудиокодеком через I2S1 */
		//#define WITHFPGAIF_I2S2_DUPLEX_SLAVE	1		/* Обмен с FPGA через I2S2 */
	#endif /* WITHINTEGRATEDDSP */

	//#define WITHCPUDACHW	1	/* использование встроенного в процессор DAC */
	#define WITHCPUADCHW 	1	/* использование встроенного в процессор ADC */

	#if ! LCDMODE_DUMMY
		#define WITHMDMAHW		1	/* Использование G2D для формирования изображений */
		#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
		//#define WITHGPUHW	1	/* Graphic processor unit */
		#define WITHLTDCHWVBLANKIRQ 1	/* Смена framebuffer по прерыванию */
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
	
	
	#if WITHTINYUSB
		#define BOARD_TUH_RHPORT 1
		#define CFG_TUH_ENABLED 1
		//#define TUP_USBIP_OHCI 1
		#define TUP_USBIP_EHCI 1
	#endif /* WITHTINYUSB */

	// USB3-DP & USB3-DM used
	#define WITHUSBHW_EHCI		USB20_HOST3_EHCI
	#define WITHUSBHW_EHCI_IRQ	USB20_HOST3_EHCI_IRQn
	#define WITHUSBHW_EHCI_IX	3

	// USB3-DP & USB3-DM used
	#define WITHUSBHW_OHCI		USB20_HOST3_OHCI
	#define WITHUSBHW_OHCI_IRQ	USB20_HOST3_OHCI_IRQn
	#define WITHUSBHW_OHCI_IX	3

	#define WITHUSBHOST_HIGHSPEEDPHYC	1	// UTMI -> USB1_DP & USB1_DM
	#define WITHEHCIHW_EHCIPORT 0	// 0 - use 1st PHY port
	#define WITHOHCIHW_OHCIPORT 0

//	#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
//	#define WITHMODEM_CDC	1

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

#if WITHENCODER

	// Выводы подключения енкодера #1
	#define ENCODER_INPUT_PORT	(gpioX_getinputs(GPIOD))
	#define ENCODER_BITA		(UINT32_C(1) << 21)		// PD21
	#define ENCODER_BITB		(UINT32_C(1) << 22)		// PD22

	// Выводы подключения енкодера #2
	#define ENCODER_SUB_INPUT_PORT	(gpioX_getinputs(GPIOD))
	#define ENCODER_SUB_BITA		(UINT32_C(1) << 19)		// PD19
	#define ENCODER_SUB_BITB		(UINT32_C(1) << 20)		// PD20

	// Выводы подключения енкодера ENC1F
	#define ENC1F_INPUT_PORT	(gpioX_getinputs(GPIOD))
	#define ENC1F_BITB_POS 	16
	#define ENC1F_BITA		(UINT32_C(1) << 17)		// PD17
	#define ENC1F_BITB		(UINT32_C(1) << 16)		// PD16

	// Выводы подключения енкодера ENC2F
	#define ENC2F_INPUT_PORT	(gpioX_getinputs(GPIOD))
	#define ENC2F_BITB_POS 	14
	#define ENC2F_BITA		(UINT32_C(1) << 15)		// PD15
	#define ENC2F_BITB		(UINT32_C(1) << 14)		// PD14

	// Выводы подключения енкодера ENC3F
	#define ENC3F_INPUT_PORT	(gpioX_getinputs(GPIOD))
	#define ENC3F_BITB_POS 	12
	#define ENC3F_BITA		(UINT32_C(1) << 13)		// PD13
	#define ENC3F_BITB		(UINT32_C(1) << 12)		// PD12

	// Выводы подключения енкодера ENC4F
	#define ENC4F_INPUT_PORT	(gpioX_getinputs(GPIOD))
	#define ENC4F_BITB_POS 	16
	#define ENC4F_BITA		(UINT32_C(1) << 11)		// PD11
	#define ENC4F_BITB		(UINT32_C(1) << 10)		// PD10

	/* Определения масок битов для формирования обработчиков прерываний в нужном GPIO */
	#define BOARD_ENCODER_BITS		(ENCODER_BITA | ENCODER_BITB)
	#define BOARD_ENCODER_SUB_BITS	(ENCODER_SUB_BITA | ENCODER_SUB_BITB)
	#define BOARD_ENC1F_BITS		(ENC1F_BITA | ENC1F_BITB)
	#define BOARD_ENC2F_BITS		(ENC2F_BITA | ENC2F_BITB)
	#define BOARD_ENC3F_BITS		(ENC3F_BITA | ENC3F_BITB)
	#define BOARD_ENC4F_BITS		(ENC4F_BITA | ENC4F_BITB)

	#define ENCODER_BITS_GET() 		(((ENCODER_INPUT_PORT & ENCODER_BITA) != 0) * GETENCBIT_A + ((ENCODER_INPUT_PORT & ENCODER_BITB) != 0) * GETENCBIT_B)
	#define ENCODER_SUB_BITS_GET() 	(((ENCODER_SUB_INPUT_PORT & ENCODER_SUB_BITA) != 0) * GETENCBIT_A + ((ENCODER_SUB_INPUT_PORT & ENCODER_SUB_BITB) != 0) * GETENCBIT_B)

	#define ENC1F_BITS_GET() 		((ENC1F_INPUT_PORT & BOARD_ENC1F_BITS) >> ENC1F_BITB_POS) //(((ENC1F_INPUT_PORT & ENC1F_BITA) != 0) * GETENCBIT_A + ((ENC1F_INPUT_PORT & ENC1F_BITB) != 0) * GETENCBIT_B)	// ENC1F
	#define ENC2F_BITS_GET() 		((ENC2F_INPUT_PORT & BOARD_ENC2F_BITS) >> ENC2F_BITB_POS) //(((ENC2F_INPUT_PORT & ENC2F_BITA) != 0) * GETENCBIT_A + ((ENC2F_INPUT_PORT & ENC2F_BITB) != 0) * GETENCBIT_B)	// ENC2F
	#define ENC3F_BITS_GET() 		((ENC3F_INPUT_PORT & BOARD_ENC3F_BITS) >> ENC3F_BITB_POS) //(((ENC3F_INPUT_PORT & ENC3F_BITA) != 0) * GETENCBIT_A + ((ENC3F_INPUT_PORT & ENC3F_BITB) != 0) * GETENCBIT_B)	// ENC3F
	#define ENC4F_BITS_GET() 		((ENC4F_INPUT_PORT & BOARD_ENC4F_BITS) >> ENC4F_BITB_POS) //(((ENC4F_INPUT_PORT & ENC4F_BITA) != 0) * GETENCBIT_A + ((ENC4F_INPUT_PORT & ENC4F_BITB) != 0) * GETENCBIT_B)	// ENC4F

	#define ENCODER_INITIALIZE() do { \
		static einthandler_t eh1; \
		static einthandler_t eh2; \
		static einthandler_t enc1fh; \
		static einthandler_t enc2fh; \
		static einthandler_t enc3fh; \
		static einthandler_t enc4fh; \
		/* Разным валкодерам на одном GPIO нельзя назначать разные приоритеты */ \
		/* */ \
		/* Main tuning knob - прерывания на обе фазы */ \
		arm_hardware_piod_altfn20(BOARD_ENCODER_BITS, GPIO_CFG_EINT); \
		arm_hardware_piod_updown(BOARD_ENCODER_BITS, BOARD_ENCODER_BITS, 0); \
		einthandler_initialize(& eh1, BOARD_ENCODER_BITS, spool_encinterrupts, & encoder1); \
		arm_hardware_piod_onchangeinterrupt(BOARD_ENCODER_BITS, BOARD_ENCODER_BITS, BOARD_ENCODER_BITS, ENCODER_PRIORITY, ENCODER_TARGETCPU, & eh1); \
		/* */ \
		/* Second tuning knob - прерывания на обе фазы */ \
		arm_hardware_piod_altfn20(BOARD_ENCODER_SUB_BITS, GPIO_CFG_EINT); \
		arm_hardware_piod_updown(BOARD_ENCODER_SUB_BITS, BOARD_ENCODER_SUB_BITS, 0); \
		einthandler_initialize(& eh2, BOARD_ENCODER_SUB_BITS, spool_encinterrupts, & encoder_sub); \
		arm_hardware_piod_onchangeinterrupt(BOARD_ENCODER_SUB_BITS, BOARD_ENCODER_SUB_BITS, BOARD_ENCODER_SUB_BITS, ENCODER_PRIORITY, ENCODER_TARGETCPU, & eh2); \
		/* */ \
		/* ENC1F - прерывания на обе фазы */ \
		arm_hardware_piod_altfn20(BOARD_ENC1F_BITS, GPIO_CFG_EINT); \
		arm_hardware_piod_updown(BOARD_ENC1F_BITS, BOARD_ENC1F_BITS, 0); \
		einthandler_initialize(& enc1fh, BOARD_ENC1F_BITS, spool_encinterrupts, & encoder_ENC1F); \
		arm_hardware_piod_onchangeinterrupt(BOARD_ENC1F_BITS, BOARD_ENC1F_BITS, BOARD_ENC1F_BITS, ENCODER_PRIORITY, ENCODER_TARGETCPU, & enc1fh); \
		/* */ \
		/* ENC2F - прерывания на обе фазы */ \
		arm_hardware_piod_altfn20(BOARD_ENC2F_BITS, GPIO_CFG_EINT); \
		arm_hardware_piod_updown(BOARD_ENC2F_BITS, BOARD_ENC2F_BITS, 0); \
		einthandler_initialize(& enc2fh, BOARD_ENC2F_BITS, spool_encinterrupts, & encoder_ENC2F); \
		arm_hardware_piod_onchangeinterrupt(BOARD_ENC2F_BITS, BOARD_ENC2F_BITS, BOARD_ENC2F_BITS, ENCODER_PRIORITY, ENCODER_TARGETCPU, & enc2fh); \
		/* */ \
		/* ENC3F - прерывания на обе фазы */ \
		arm_hardware_piod_altfn20(BOARD_ENC3F_BITS, GPIO_CFG_EINT); \
		arm_hardware_piod_updown(BOARD_ENC3F_BITS, BOARD_ENC3F_BITS, 0); \
		einthandler_initialize(& enc3fh, BOARD_ENC3F_BITS, spool_encinterrupts, & encoder_ENC3F); \
		arm_hardware_piod_onchangeinterrupt(BOARD_ENC3F_BITS, BOARD_ENC3F_BITS, BOARD_ENC3F_BITS, ENCODER_PRIORITY, ENCODER_TARGETCPU, & enc3fh); \
		/* */ \
		/* ENC4F - прерывания на обе фазы */ \
		arm_hardware_piod_altfn20(BOARD_ENC4F_BITS, GPIO_CFG_EINT); \
		arm_hardware_piod_updown(BOARD_ENC4F_BITS, BOARD_ENC4F_BITS, 0); \
		einthandler_initialize(& enc4fh, BOARD_ENC4F_BITS, spool_encinterrupts, & encoder_ENC4F); \
		arm_hardware_piod_onchangeinterrupt(BOARD_ENC4F_BITS, BOARD_ENC4F_BITS, BOARD_ENC4F_BITS, ENCODER_PRIORITY, ENCODER_TARGETCPU, & enc4fh); \
	} while (0)

#endif /* WITHENCODER */

#define I2S0HW_INITIALIZE(master) do { \
	/*arm_hardware_pioi_altfn50(UINT32_C(1) << 6,	GPIO_CFG_AF3); *//* PA6 H_I2S0_MCLK	*/ \
	arm_hardware_pioa_altfn50(UINT32_C(1) << 8,	GPIO_CFG_AF3); /* PA8 H_I2S0_LRCK	*/ \
	arm_hardware_pioa_altfn50(UINT32_C(1) << 7,	GPIO_CFG_AF3); /* PA7 H_I2S0_BCLK	*/ \
	arm_hardware_pioa_altfn50(UINT32_C(1) << 5,	GPIO_CFG_AF3); /* PA5 H_I2S0_DOUT0 to FPGA */ \
	arm_hardware_pioa_altfn50(UINT32_C(1) << 9,	GPIO_CFG_AF3); /* PA9 H_I2S0_DIN0 from FPGA */ \
} while (0)
#define HARDWARE_I2S0HW_DIN 0	/* DIN0 used */
#define HARDWARE_I2S0HW_DOUT 0	/* DOUT0 used */

/* Распределение битов в ARM контроллерах */

#if (WITHCAT && WITHCAT_UART5)
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

#endif /* (WITHCAT && WITHCAT_UART5) */

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


#if (WITHCAT && WITHCAT_MUX && WITHUART5HW)

	////////////////////////////////////
	// CAT функции работают через UART5
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_INITIALIZE() do { \
			hardware_uart5_initialize(0, DEBUGSPEED, 8, 0, 0); \
		} while (0)
	// Вызывается из user-mode программы
	#define HARDWARE_CAT_SET_SPEED(baudrate) do { \
			hardware_uart5_set_speed(baudrate); \
		} while (0)

	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния RTS
	#define HARDWARE_CAT_GET_RTS() (board_get_catmux() == BOARD_CATMUX_USB ? usbd_cdc1_getrts() : 0)
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния DTR
	#define HARDWARE_CAT_GET_DTR() (board_get_catmux() == BOARD_CATMUX_USB ? usbd_cdc1_getdtr() : 0)
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния RTS
	#define HARDWARE_CAT2_GET_RTS() (board_get_catmux() == BOARD_CATMUX_USB ? usbd_cdc2_getrts() : 0)
	// вызывается из обработчика перерываний (или из user-mode программы) для получения состояния DTR
	#define HARDWARE_CAT2_GET_DTR() (board_get_catmux() == BOARD_CATMUX_USB ? usbd_cdc2_getdtr() : 0)

	// вызывается из обработчика прерываний UART5
	// с принятым символом
	#define HARDWARE_UART5_ONRXCHAR(c) do { \
			if (board_get_catmux() == BOARD_CATMUX_USB) { \
				hardware_uart5_enablerx(0); \
			} else { \
				cat2_parsechar(c); \
			} \
		} while (0)
	// вызывается из обработчика прерываний UART5
	#define HARDWARE_UART5_ONOVERFLOW() do { \
		if (board_get_catmux() == BOARD_CATMUX_USB) \
			hardware_uart5_enablerx(0); \
		else \
			cat2_rxoverflow(); \
		} while (0)
	// вызывается из обработчика прерываний UART5
	// по готовности передатчика
	#define HARDWARE_UART5_ONTXCHAR(ctx) do { \
		if (board_get_catmux() == BOARD_CATMUX_USB) { \
			hardware_uart5_enabletx(0); \
		} else { \
			cat2_sendchar(ctx); \
		} \
	} while (0)
	// вызывается из обработчика прерываний UART5
	// по окончании передачи (сдвиговый регистр передатчика пуст)
	#define HARDWARE_UART5_ONTXDONE(ctx) do { \
		if (board_get_catmux() == BOARD_CATMUX_USB) \
			; \
		else \
			cat2_txdone(ctx); \
	} while (0)
	////////////////////////////////////
	// CAT функции работают через виртуальный USB последовательный порт
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_CAT_ENABLETX(v) do { \
			if (board_get_catmux() == BOARD_CATMUX_USB) { \
				hardware_uart5_enabletx(0); \
				usbd_cdc_enabletx(v); \
			} else { \
				usbd_cdc_enabletx(0); \
				hardware_uart5_enabletx(v); \
			} \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для управления разрешением последующих вызовов прерывания
	#define HARDWARE_CAT_ENABLERX(v) do { \
			if (board_get_catmux() == BOARD_CATMUX_USB) { \
				hardware_uart5_enablerx(0); \
				usbd_cdc_enablerx(v); \
			} else { \
				usbd_cdc_enablerx(0); \
				hardware_uart5_enablerx(v); \
			} \
		} while (0)
	// вызывается из state machie протокола CAT или NMEA (в прерываниях)
	// для передачи символа
	#define HARDWARE_CAT_TX(ctx, c) do { \
			if (board_get_catmux() == BOARD_CATMUX_USB) { \
				usbd_cdc_tx((ctx), (c)); \
				hardware_uart5_enabletx(0); \
			} else { \
				usbd_cdc_enabletx(0); \
				hardware_uart5_tx((ctx), (c)); \
			} \
		} while (0)

	// вызывается из обработчика прерываний CDC
	// с принятым символом
	#define HARDWARE_CDC_ONRXCHAR(offset, c) do { \
		if (board_get_catmux() == BOARD_CATMUX_USB) \
			cat2_parsechar(c); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// произошёл разрыв связи при работе по USB CDC
	#define HARDWARE_CDC_ONDISCONNECT() do { \
		if (board_get_catmux() == BOARD_CATMUX_USB) \
			cat2_disconnect(); \
		} while (0)
	// вызывается из обработчика прерываний CDC
	// по готовности передатчика
	#define HARDWARE_CDC_ONTXCHAR(offset, ctx) do { \
		if (board_get_catmux() == BOARD_CATMUX_USB) \
			cat2_sendchar(ctx); \
		} while (0)
	////////////////////////////////////
	/* манипуляция от виртуального CDC порта */
	#define FROMCAT_DTR_INITIALIZE() do { \
		} while (0)

	/* переход на передачу от виртуального CDC порта*/
	#define FROMCAT_RTS_INITIALIZE() do { \
		} while (0)

#else /* (WITHCAT && WITHCAT_MUX && WITHUART5HW) */

	void user_uart5_onrxchar(uint_fast8_t c);
	void user_uart5_ontxchar(void * ctx);

	#define HARDWARE_UART5_ONRXCHAR(c) do { user_uart5_onrxchar((c)); } while (0)
	#define HARDWARE_UART5_ONTXCHAR(ctx) do { user_uart5_ontxchar((ctx)); } while (0)

#endif /* (WITHCAT && WITHCAT_MUX && WITHUART5HW) */

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
	#define SMHCHARD_FREQ (allwnr_t113_get_smhc0_freq())
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

	#define HARDWARE_SDIO_CD_BIT	(UINT32_C(1) << 6)	/* PF6 - SDC0_DET */

	#define HARDWARE_SDIOSENSE_INITIALIZE()	do { \
			arm_hardware_piof_inputs(HARDWARE_SDIO_CD_BIT); /* PF6 - SDC0_DET */ \
			arm_hardware_piof_updown(HARDWARE_SDIO_CD_BIT, HARDWARE_SDIO_CD_BIT, 0); \
	} while (0)


	#define HARDWARE_SDIOSENSE_CD() ((gpioX_getinputs(GPIOF) & HARDWARE_SDIO_CD_BIT) == 0)	/* == 0: no disk. получить состояние датчика CARD PRESENT */
	#define HARDWARE_SDIOSENSE_WP() 0//((gpioX_getinputs(GPIOF) & HARDWARE_SDIO_WP_BIT) != 0)	/* != 0: write protected получить состояние датчика CARD WRITE PROTECT */

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
	#define SMHCHARD_FREQ (allwnr_t113_get_smhc1_freq())
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
	#define SMHCHARD_FREQ (allwnr_t113_get_smhc2_freq())
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
	#define TXDISABLE_TARGET_PIN				gpioX_getinputs(GPIOE)
	#define TXDISABLE_BIT_TXDISABLE				(UINT32_C(1) << 11)		// PE11 - TX INHIBIT
	// получить бит запрета передачи (от усилителя мощности)
	#define HARDWARE_GET_TXDISABLE() (0) //((TXDISABLE_TARGET_PIN & TXDISABLE_BIT_TXDISABLE) != 0)
	#define TXDISABLE_INITIALIZE() do { \
			arm_hardware_pioe_inputs(TXDISABLE_BIT_TXDISABLE); \
			arm_hardware_pioe_updown(TXDISABLE_BIT_TXDISABLE, 0, TXDISABLE_BIT_TXDISABLE); \
		} while (0)
	// ---

	// +++
	// PTT input - PD10
	// PTT2 input - PD9
	#define PTT_TARGET_PIN				gpioX_getinputs(GPIOE)
	#define PTT_BIT_PTT					(UINT32_C(1) << 10)		// PE10 - PTT_FRONT
	#define PTT2_TARGET_PIN				gpioX_getinputs(GPIOD)
	#define PTT2_BIT_PTT				(UINT32_C(1) << 26)		// PD26 - PTT
	#define PTT3_TARGET_PIN				gpioX_getinputs(GPIOD)
	#define PTT3_BIT_PTT				(UINT32_C(1) << 25)		// PD25 - READ PTT
	// получить бит запроса оператором перехода на пердачу
	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0 || (PTT2_TARGET_PIN & PTT2_BIT_PTT) == 0 || (PTT3_TARGET_PIN & PTT3_BIT_PTT) == 0)
	#define PTT_INITIALIZE() do { \
			arm_hardware_pioe_inputs(PTT_BIT_PTT); \
			arm_hardware_pioe_updown(PTT_BIT_PTT, PTT_BIT_PTT, 0); \
			arm_hardware_piod_inputs(PTT2_BIT_PTT); \
			arm_hardware_piod_updown(PTT2_BIT_PTT, PTT2_BIT_PTT, 0); \
			arm_hardware_piod_inputs(PTT3_BIT_PTT); \
			arm_hardware_piod_updown(PTT3_BIT_PTT, PTT3_BIT_PTT, 0); \
		} while (0)
	// ---
	// TUNE input - PD11
	#define TUNE_TARGET_PIN				gpioX_getinputs(GPIOA)
	#define TUNE_BIT_TUNE				(UINT32_C(1) << 4)		// PA4
	#define HARDWARE_GET_TUNE() 0//((TUNE_TARGET_PIN & TUNE_BIT_TUNE) == 0)
	#define TUNE_INITIALIZE() do { \
			arm_hardware_pioa_inputs(TUNE_BIT_TUNE); \
			arm_hardware_pioa_updown(TUNE_BIT_TUNE, TUNE_BIT_TUNE, 0); \
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
	#define ELKEY_BIT_LEFT				(UINT32_C(1) << 2)		// PA2
	#define ELKEY_BIT_RIGHT				(UINT32_C(1) << 3)		// PA3

	#define ELKEY_LEFT_TARGET_PIN			gpioX_getinputs(GPIOA)
	#define ELKEY_RIGHT_TARGET_PIN			gpioX_getinputs(GPIOA)

	#define HARDWARE_GET_ELKEY_LEFT() 	((ELKEY_LEFT_TARGET_PIN & ELKEY_BIT_LEFT) == 0)
	#define HARDWARE_GET_ELKEY_RIGHT() 	((ELKEY_RIGHT_TARGET_PIN & ELKEY_BIT_RIGHT) == 0)


	#define ELKEY_INITIALIZE() do { \
			arm_hardware_pioa_inputs(ELKEY_BIT_LEFT); \
			arm_hardware_pioa_inputs(ELKEY_BIT_RIGHT); \
			arm_hardware_pioa_updown(ELKEY_BIT_LEFT, ELKEY_BIT_LEFT, 0); \
			arm_hardware_pioa_updown(ELKEY_BIT_RIGHT, ELKEY_BIT_RIGHT, 0); \
		} while (0)

#endif /* WITHELKEY */

// IOUPDATE = PA15
//#define SPI_IOUPDATE_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); (void) GPIOA->BSRR; } while (0)
//#define SPI_IOUPDATE_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); (void) GPIOA->BSRR; } while (0)
//#define SPI_IOUPDATE_BIT		(UINT32_C(1) << 15)	// * PA15

#if WITHSPIHW || WITHSPISW
	// Набор определений для работы без внешнего дешифратора

	#define OE_CTL1_BIT	(UINT32_C(1) << 16)	/* PI16 */
	//#define targetdataflash 0xFF
	#define targetnone 0x00
	#define FPGALOADER_SPISPEED SPIC_SPEEDUFAST
	#define SPIDF_SPEEDC SPIC_SPEEDFAST

	#define targetctl1		(UINT32_C(1) << 22)		// PE22 board control registers chain
	#define targettsc1		(UINT32_C(1) << 19)		// PE19 XPT2046 SPI chip select signal - CSEXT1
	#define targetnvram		(UINT32_C(1) << 18)		// PE18 nvram FM25W356
	#define targetcodec1	(UINT32_C(1) << 20)		// PE20 on-board codec1 NAU8822L
	#define targetfpga1		(UINT32_C(1) << 17)		// PE17 FPGA control registers CS1
	#define targetadck		(UINT32_C(1) << 21)		// PE21 on-board ADC MCP3208-BI/SL chip select (KEYBOARD) ADC2CS
	#define targetxad2		(UINT32_C(1) << 11)		// PI11 ext2 external SPI device (PA BOARD ADC) CSEXT2

	/* Select specified chip. */
	#define SPI_CS_ASSERT(target) do { \
		switch (target) { \
		/*case targetdataflash: { gpioX_setstate(GPIOI, SPDIF_NCS_BIT, 0 * (SPDIF_NCS_BIT)); } break; *//* PC3 SPI0_CS */ \
		/*case targetrtc1: { gpioX_setstate(GPIOI, (target), 1 * (target)); } break; */\
		default: { gpioX_setstate(GPIOE, (target), 0 * (target)); } break; \
		case targetnone: break; \
		} \
	} while (0)

	/* Unelect specified chip. */
	#define SPI_CS_DEASSERT(target)	do { \
		switch (target) { \
		/*case targetdataflash: { gpioX_setstate(GPIOI, SPDIF_NCS_BIT, 1 * (SPDIF_NCS_BIT)); } break; *//* PC3 SPI0_CS */ \
		/*case targetrtc1: { gpioX_setstate(GPIOI, (target), 0 * (target)); } break; */\
		case targetctl1: { gpioX_setstate(GPIOE, (target), 1 * (target)); gpioX_setstate(GPIOI, OE_CTL1_BIT, 0 * OE_CTL1_BIT); } break; \
		default: { gpioX_setstate(GPIOE, (target), 1 * (target)); } break; \
		case targetnone: break; \
		} \
	} while (0)

	/* Perform delay after assert or de-assert specific CS line */
	#define SPI_CS_DELAY(target) do { \
		switch (target) { \
		case targetxad2: local_delay_us(5); break; /* external SPI device (PA BOARD ADC) */ \
		case targetctl1: local_delay_us(5); break; /* board control registers chain */ \
		case targettsc1: local_delay_us(1); break; /* XPT2046 SPI chip select signal */ \
		case targetfpga1: local_delay_us(1); break; /* FPGA control registers CS1 */ \
		default: local_delay_us(1); break; \
		} \
	} while (0)

	/* инициализация линий выбора периферийных микросхем */
	#define SPI_ALLCS_INITIALIZE() do { \
		/*arm_hardware_pioc_outputs(SPDIF_NCS_BIT, 1 * SPDIF_NCS_BIT); */	/* PC3 SPI0_CS */ \
		arm_hardware_pioi_outputs(OE_CTL1_BIT, 1 * OE_CTL1_BIT); /*  */ \
		arm_hardware_pioe_outputs(targettsc1, 1 * targettsc1); /*  */ \
		arm_hardware_pioe_outputs(targetnvram, 1 * targetnvram); /*  */ \
		arm_hardware_pioe_outputs(targetctl1, 1 * targetctl1); /*  */ \
		arm_hardware_pioe_outputs(targetcodec1, 1 * targetcodec1); /*  */ \
		arm_hardware_pioe_outputs(targetfpga1, 1 * targetfpga1); /*  */ \
		arm_hardware_pioe_outputs(targetadck, 1 * targetadck); /*  */ \
		arm_hardware_pioe_outputs(targetxad2, 1 * targetxad2); /*  */ \
		local_delay_us(1); \
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
	#define BOARD_SPI_FREQ (allwnr_t113_get_spi1_freq())

	#if WITHSPIHW
		#define SPIIO_INITIALIZE() do { \
			arm_hardware_pioh_altfn50(SPI_SCLK_BIT, GPIO_CFG_AF4); 	/* PH6 SPI1_CLK */ \
			arm_hardware_pioh_altfn50(SPI_MOSI_BIT, GPIO_CFG_AF4); 	/* PH7 SPI1_MOSI */ \
			arm_hardware_pioh_altfn50(SPI_MISO_BIT, GPIO_CFG_AF4); 	/* PH8 SPI1_MISO */ \
			local_delay_us(1); \
		} while (0)

	#elif WITHSPISW

		#define SPIIO_INITIALIZE() do { \
			arm_hardware_pioh_outputs(SPI_SCLK_BIT, 1 * SPI_SCLK_BIT); 	/* PH6 SPI1_CLK */ \
			arm_hardware_pioh_outputs(SPI_MOSI_BIT, 1 * SPI_MOSI_BIT); 	/* PH7 SPI1_MOSI */ \
			arm_hardware_pioh_inputs(SPI_MISO_BIT); 	/* PH8 SPI1_MISO */ \
			local_delay_us(1); \
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
		arm_hardware_pioh_updown(RXMASK | TXMASK, RXMASK, 0); \
	} while (0)

// WITHUART5HW
// Используется периферийный контроллер последовательного порта UART5 (rear panel) */
#define HARDWARE_UART5_INITIALIZE() do { \
		const portholder_t TXMASK = UINT32_C(1) << 2; /* PH2 UART5-TX */ \
		const portholder_t RXMASK = UINT32_C(1) << 3; /* PH3 UART5-RX - pull-up RX data */  \
		arm_hardware_pioh_altfn2(TXMASK, GPIO_CFG_AF2); \
		arm_hardware_pioh_altfn2(RXMASK, GPIO_CFG_AF2); \
		arm_hardware_pioh_updown(RXMASK | TXMASK, RXMASK, 0); \
	} while (0)

#if WITHKEYBOARD

	#define TARGET_POWERBTN_BIT (UINT32_C(1) << 18)	// PD18 - ~CPU_POWER_SW signal

#if WITHPWBUTTON
	// P5_3 - ~CPU_POWER_SW signal
	#define TARGET_POWERBTN_GET	((gpioX_getinputs(GPIOD) & TARGET_POWERBTN_BIT) == 0)
#endif /* WITHPWBUTTON */

	#define HARDWARE_KBD_INITIALIZE() do { \
			arm_hardware_piod_inputs(TARGET_POWERBTN_BIT); \
			arm_hardware_piod_updown(TARGET_POWERBTN_BIT, TARGET_POWERBTN_BIT, 0);	/* PD18: pull-up button */ \
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
	#define	TWIHARD_FREQ (allwnr_t113_get_s_twi_freq()) // APBS2_CLK allwnr_t507_get_apb2_freq() or allwnr_t507_get_apbs2_freq()

#else /* WITHISBOOTLOADER */
	// I2C/TWI
	/* Not WITHISBOOTLOADER */
	#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
	//#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */
	// PA0 - TWI0_SCL
	// PA1 - TWI0_SDA
	#define TARGET_TWI_TWCK		(UINT32_C(1) << 0)
	#define TARGET_TWI_TWCK_PIN		(gpioX_getinputs(GPIOA))
	#define TARGET_TWI_TWCK_PORT_C(v) do { gpioX_setopendrain(GPIOA, (v), 0 * (v)); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do { gpioX_setopendrain(GPIOA, (v), 1 * (v)); } while (0)

	#define TARGET_TWI_TWD		(UINT32_C(1) << 1)
	#define TARGET_TWI_TWD_PIN		(gpioX_getinputs(GPIOA))
	#define TARGET_TWI_TWD_PORT_C(v) do { gpioX_setopendrain(GPIOA, (v), 0 * (v)); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v) do { gpioX_setopendrain(GPIOA, (v), 1 * (v)); } while (0)

	// Инициализация битов портов ввода-вывода для программной реализации I2C
	#define	TWISOFT_INITIALIZE() do { \
			arm_hardware_pioa_opendrain(TARGET_TWI_TWCK, TARGET_TWI_TWCK); /* SCL */ \
			arm_hardware_pioa_opendrain(TARGET_TWI_TWD, TARGET_TWI_TWD);  	/* SDA */ \
		} while (0) 
	#define	TWISOFT_DEINITIALIZE() do { \
			arm_hardware_pioa_inputs(TARGET_TWI_TWCK); 	/* SCL */ \
			arm_hardware_pioa_inputs(TARGET_TWI_TWD);	/* SDA */ \
		} while (0)
	// Инициализация битов портов ввода-вывода для аппаратной реализации I2C
	// присоединение выводов к периферийному устройству
	#define	TWIHARD_INITIALIZE() do { \
			arm_hardware_pioa_altfn2(TARGET_TWI_TWCK, GPIO_CFG_AF4);	/* PA0 - TWI0_SCL */ \
			arm_hardware_pioa_altfn2(TARGET_TWI_TWD, GPIO_CFG_AF4);		/* PA1 - TWI0_SDA */ \
			arm_hardware_pioa_updown(TARGET_TWI_TWCK, TARGET_TWI_TWCK, 0); \
			arm_hardware_pioa_updown(TARGET_TWI_TWD, TARGET_TWI_TWD, 0); \
		} while (0) 
	#define	TWIHARD_IX 0	/* 0 - TWI0, 1: TWI1... */
	#define	TWIHARD_PTR TWI0	/* 0 - TWI0, 1: TWI1... */
	#define	TWIHARD_FREQ (allwnr_t113_get_twi_freq()) // APBS2_CLK allwnr_t507_get_apb2_freq() or allwnr_t507_get_apbs2_freq()

#endif /* WITHISBOOTLOADER */

#if WITHFPGAWAIT_AS || WITHFPGALOAD_PS

	/* outputs */
	#define FPGA_NCONFIG_PORT_S(v)	do { gpioX_setstate(GPIOE, (v), !! (1) * (v)); local_delay_us(5); } while (0)
	#define FPGA_NCONFIG_PORT_C(v)	do { gpioX_setstate(GPIOE, (v), !! (0) * (v)); local_delay_us(5); } while (0)
	#define FPGA_NCONFIG_BIT		(UINT32_C(1) << 12)	/* PE12 bit connected to nCONFIG pin ALTERA FPGA */

	/* inputs */
	#define FPGA_CONF_DONE_INPUT	gpioX_getinputs(GPIOE)
	#define FPGA_CONF_DONE_BIT		(UINT32_C(1) << 14)	/* PE14 bit connected to CONF_DONE pin ALTERA FPGA */

	#define FPGA_NSTATUS_INPUT		gpioX_getinputs(GPIOE)
	#define FPGA_NSTATUS_BIT		(UINT32_C(1) << 13)	/* PE13 bit connected to NSTATUS pin ALTERA FPGA */

	#define FPGA_INIT_DONE_INPUT	gpioX_getinputs(GPIOE)
	#define FPGA_INIT_DONE_BIT		(UINT32_C(1) << 15)	/* PE15 bit connected to INIT_DONE pin ALTERA FPGA */

	/* Инициадизация выводов GPIO процессора для получения состояния и управлением загрузкой FPGA */
	#define HARDWARE_FPGA_LOADER_INITIALIZE() do { \
			arm_hardware_pioe_outputs(FPGA_NCONFIG_BIT, FPGA_NCONFIG_BIT); \
			arm_hardware_pioe_inputs(FPGA_NSTATUS_BIT); \
			arm_hardware_pioe_inputs(FPGA_CONF_DONE_BIT); \
			arm_hardware_pioe_inputs(FPGA_INIT_DONE_BIT); \
			local_delay_us(5); \
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
	#define TARGET_FPGA_FIR_CS_PORT_C(v)	do { gpioX_setstate(GPIOE, (v), !! (0) * (v)); local_delay_us(1); } while (0) // do { GPIOC->BSRR = BSRR_C(v); (void) GPIOC->BSRR; } while (0)
	#define TARGET_FPGA_FIR_CS_PORT_S(v)	do { gpioX_setstate(GPIOE, (v), !! (1) * (v)); local_delay_us(1); } while (0) // do { GPIOC->BSRR = BSRR_S(v); (void) GPIOC->BSRR; } while (0)
	#define TARGET_FPGA_FIR_CS_BIT (UINT32_C(1) << 2)	/* PE2 - fir CS ~FPGA_FIR_CLK */

	// FPGA PIN_8
	#define TARGET_FPGA_FIR1_WE_PORT_C(v)	do { gpioX_setstate(GPIOE, (v), !! (0) * (v)); local_delay_us(1); } while (0) // do { GPIOD->BSRR = BSRR_C(v); (void) GPIOD->BSRR; } while (0)
	#define TARGET_FPGA_FIR1_WE_PORT_S(v)	do { gpioX_setstate(GPIOE, (v), !! (1) * (v)); local_delay_us(1); } while (0) // do { GPIOD->BSRR = BSRR_S(v); (void) GPIOD->BSRR; } while (0)
	#define TARGET_FPGA_FIR1_WE_BIT (UINT32_C(1) << 3)	/* PE3 - fir1 WE */

	// FPGA PIN_7
	#define TARGET_FPGA_FIR2_WE_PORT_C(v)	do { gpioX_setstate(GPIOE, (v), !! (0) * (v)); local_delay_us(1); } while (0) // do { GPIOD->BSRR = BSRR_C(v); (void) GPIOD->BSRR; } while (0)
	#define TARGET_FPGA_FIR2_WE_PORT_S(v)	do { gpioX_setstate(GPIOE, (v), !! (1) * (v)); local_delay_us(1); } while (0) // do { GPIOD->BSRR = BSRR_S(v); (void) GPIOD->BSRR; } while (0)
	#define TARGET_FPGA_FIR2_WE_BIT (UINT32_C(1) << 0)	/* PE0 - fir2 WE */

	#define TARGET_FPGA_FIR_INITIALIZE() do { \
			arm_hardware_pioe_outputs2m(TARGET_FPGA_FIR1_WE_BIT, TARGET_FPGA_FIR1_WE_BIT); \
			arm_hardware_pioe_outputs2m(TARGET_FPGA_FIR2_WE_BIT, TARGET_FPGA_FIR2_WE_BIT); \
			arm_hardware_pioe_outputs2m(TARGET_FPGA_FIR_CS_BIT, TARGET_FPGA_FIR_CS_BIT); \
		} while (0)
#endif /* WITHDSPEXTFIR */

#if 1
	/* получение состояния переполнения АЦП */
	#define TARGET_FPGA_OVF_INPUT		gpioX_getinputs(GPIOE)
	#define TARGET_FPGA_OVF_BIT			(UINT32_C(1) << 1)	// PE1
	#define TARGET_FPGA_OVF_GET			((TARGET_FPGA_OVF_INPUT & TARGET_FPGA_OVF_BIT) == 0)	// 1 - overflow active
	#define TARGET_FPGA_OVF_INITIALIZE() do { \
				arm_hardware_pioe_inputs(TARGET_FPGA_OVF_BIT); \
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

#if WITHUSBHW

	#define TARGET_GPIOE_VBUSON_BIT 0//(UINT32_C(1) << 18)	// PE18 - единицей включение питания для device
	#define	USBD_EHCI_INITIALIZE() do { \
		arm_hardware_pioe_outputs(TARGET_GPIOE_VBUSON_BIT, 0 * TARGET_GPIOE_VBUSON_BIT); \
	} while (0)

	#define TARGET_USBFS_VBUSON_SET(on)	do { \
		gpioX_setstate(GPIOE, TARGET_GPIOE_VBUSON_BIT, !! (on) * TARGET_GPIOE_VBUSON_BIT); \
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

#if WITHISBOOTLOADER
	#define	HARDWARE_DCDC_INITIALIZE() do { \
	} while (0)
#elif WITHDCDCFREQCTL
	// ST ST1S10 Synchronizable switching frequency from 400 kHz up to 1.2 MHz
	#define WITHHWDCDCFREQMIN 400000L
	#define WITHHWDCDCFREQMAX 1200000L
	#define HARDWARE_DCDC_PWMCH 5	/* PWM5 */

	// PA12 - DC-DC synchro output
	// PWM5 AF2
	#define	HARDWARE_DCDC_INITIALIZE() do { \
		hardware_dcdcfreq_pwm_initialize(HARDWARE_DCDC_PWMCH); \
		arm_hardware_pioa_altfn2((UINT32_C(1) << 12), GPIO_CFG_AF2); /* PA12 - PWM5 */ \
	} while (0)
	#define HARDWARE_DCDC_SETDIV(f) do { \
		hardware_dcdcfreq_pwm_setdiv(HARDWARE_DCDC_PWMCH, f); \
	} while (0)
#else /* WITHDCDCFREQCTL */
	#define	HARDWARE_DCDC_INITIALIZE() do { \
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
	/* Яркость Управлятся через PWM */
	#define HARDWARE_BL_PWMCH 0	/* PWM0 */
	#define HARDWARE_BL_FREQ 	10000	/* Частота PWM управления подсветкой */
	#define	HARDWARE_BL_INITIALIZE() do { \
		const portholder_t ENmask = (UINT32_C(1) << 28); /* PD28 */ \
		hardware_dcdcfreq_pwm_initialize(HARDWARE_BL_PWMCH); \
		arm_hardware_piod_altfn2(ENmask, GPIO_CFG_AF2); /* PD28 - PWM0 */ \
	} while (0)
	// en: 0/1, level=WITHLCDBACKLIGHTMIN..WITHLCDBACKLIGHTMAX
	// level=WITHLCDBACKLIGHTMIN не приводит к выключениию подсветки
	#define HARDWARE_BL_SET(en, level) do { \
		hardware_bl_pwm_set_duty(HARDWARE_BL_PWMCH, HARDWARE_BL_FREQ, !! (en) * (level) * 100 / WITHLCDBACKLIGHTMAX); \
	} while (0)

#else /* WITHBLPWMCTL */

	/* Аналоговое управление яркостью подсветки */

	#if LCDMODE_LQ043T3DX02K
		#define WITHLCDBACKLIGHTOFF	1	// Имеется управление включением/выключением подсветки дисплея
		#define WITHLCDBACKLIGHT	1	// Имеется управление яркостью дисплея
		#define WITHLCDBACKLIGHTMIN	1	// Нижний предел регулировки (показываемый на дисплее)
		#define WITHLCDBACKLIGHTMAX	3	// Верхний предел регулировки (показываемый на дисплее)
		#define WITHLCDBACKLIGHTDEF	3	// значение яркости по уиолчанию
		//#define WITHKBDBACKLIGHT	1	// Имеется управление подсветкой клавиатуры
	#elif LCDMODE_AT070TN90 || LCDMODE_AT070TNA2
		#define WITHLCDBACKLIGHTOFF	1	// Имеется управление включением/выключением подсветки дисплея
		#define WITHLCDBACKLIGHT	1	// Имеется управление яркостью дисплея
		#define WITHLCDBACKLIGHTMIN	1	// Нижний предел регулировки (показываемый на дисплее)
		#define WITHLCDBACKLIGHTMAX	3	// Верхний предел регулировки (показываемый на дисплее)
		#define WITHLCDBACKLIGHTDEF	3	// значение яркости по уиолчанию
		//#define WITHKBDBACKLIGHT	1	// Имеется управление подсветкой клавиатуры
	#else
		/* Заглушка для работы без дисплея */
		#define WITHLCDBACKLIGHTMIN	0
		#define WITHLCDBACKLIGHTMAX	1	// Верхний предел регулировки (показываемый на дисплее)
	#endif
	/* BL0: PA12. BL1: PA11, EN: PD28  */
	#define	HARDWARE_BL_INITIALIZE() do { \
	} while (0)

	/* установка яркости и включение/выключение преобразователя подсветки */
	/* LCD_BL_ADJ0: PA12, LCD_BL_ADJ1: PA11, LCD_BL_ENABLE:PD28 */
	/* level: начиная от WITHLCDBACKLIGHTMIN */
	#define HARDWARE_BL_SET(en, level) do { \
	} while (0)

#endif /* WITHBLPWMCTL */

#if WITHLTDCHW

	// LCD FRM Control Register (Default Value: 0x0000_0000)
	//#define TCON_FRM_MODE_VAL 0//((UINT32_C(1) << 31) | (UINT32_C(1) << 6) | (UINT32_C(0) << 5) | (UINT32_C(1) << 4))	// 16 bit panel connected
	#define TCON_FRM_MODE_VAL 0//((UINT32_C(1) << 31) | (UINT32_C(0) << 6) | (UINT32_C(0) << 5)| (UINT32_C(0) << 4))	// 18 bit panel connected
	//#define TCON_FRM_MODE_VAL 0	// 24 bit panel

	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_INITIALIZE(demode) do { \
	} while (0)

	/* управление состоянием сигнала DISP панели */
	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_SET_DISP(state) do { \
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

	#if 1
		#define	TCONLCD_IX 0	/* 0 - TCON_LCD0, 1: TCON_LCD1 */
		#define	TCONLCD_PTR TCON_LCD0	/* 0 - TCON_LCD0, 1: TCON_LCD1 */
		#define	TCONLCD_CCU_CLK_REG (CCU->TCON_LCD0_CLK_REG)	/* 0 - TCON_LCD0, 1: TCON_LCD1 */
		#define	TCONLCD_CCU_BGR_REG (CCU->TCON_LCD_BGR_REG)	/* 0 - TCON_LCD0, 1: TCON_LCD1 */
		#define BOARD_TCONLCDFREQ (allwnr_t507_get_tcon_lcd0_freq())
		#define TCONLCD_IRQ TCON_LCD0_IRQn
		#define TCONLCD_LVDSIX 0	/* 0 -LVDS0 */
	#endif

	#if 0
		#define	TCONTV_IX 0	/* 0 - TCON_TV0, 1: TCON_TV1 */
		#define	TCONTV_PTR TCON_TV0	/* 0 - TCON_TV0, 1: TCON_TV0 */
		#define	TCONTV_CCU_CLK_REG (CCU->TCON_TV0_CLK_REG)	/* 0 - TCON_LCD0, 1: TCON_LCD1, 2: TCON_TV0, 3: TCON_TV1 */
		#define	TCONTV_CCU_BGR_REG (CCU->TCON_TV_BGR_REG)	/* 0 - TCON_TV0, 1: TCON_TV1 */
		#define TCONTV_IRQ TCON_TV0_IRQn
		#define BOARD_TCONTVFREQ (allwnr_t507_get_tcon_tv0_freq())
	#endif

	#if 0
		#define	TVENCODER_IX 0	/* 0 -TVE0 */
		#define	TVENCODER_PTR TVE0	/* 0 - TVE0 */
		#define	TVENCODER_BASE TVE0_BASE	/* 0 - TVE0 */
		#define	TVE_CCU_CLK_REG (CCU->TVE0_CLK_REG)	/* 0 - TVE0, 1: TVE1 */
		#define	TVE_CCU_BGR_REG (CCU->TVE_BGR_REG)	/* 0 - TVE0, 1: TVE1 */
		#define BOARD_TVEFREQ (allwnr_t507_get_tve0_freq())
	#endif

#endif /* WITHLTDCHW */


#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811)

	//	tsc interrupt XS26, pin 08
	//	tsc/LCD reset, XS26, pin 22
	//	tsc SCL: XS26, pin 01
	//	tsc SDA: XS26, pin 02

	void stmpe811_interrupt_handler(void * ctx);

	#define BOARD_GPIOA_STMPE811_INT_PIN (UINT32_C(1) << 3)		/* PA3 : tsc interrupt XS26, pin 08 */

	#define BOARD_STMPE811_INT_CONNECT() do { \
		static einthandler_t h; \
		arm_hardware_pioa_inputs(BOARD_GPIOA_STMPE811_INT_PIN); \
		arm_hardware_pioa_updown(BOARD_GPIOA_STMPE811_INT_PIN, BOARD_GPIOA_STMPE811_INT_PIN, 0); \
		arm_hardware_pioa_onchangeinterrupt(BOARD_GPIOA_STMPE811_INT_PIN, 1 * BOARD_GPIOA_STMPE811_INT_PIN, 0 * BOARD_GPIOA_STMPE811_INT_PIN, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM, & h, stmpe811_interrupt_handler); \
	} while (0)
#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811) */

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_GT911)

	//	tsc interrupt XS26, pin 08
	//	tsc/LCD reset, XS26, pin 22
	//	tsc SCL: XS26, pin 01
	//	tsc SDA: XS26, pin 02

	void gt911_interrupt_handler(void * ctx);

	#define BOARD_GPIOA_GT911_INT_PIN (UINT32_C(1) << 3)		/* PA3 : tsc interrupt XS26, pin 08 */
	#define BOARD_GT911_RESET_PIN (UINT32_C(1) << 4)			/* PA10 : tsc/LCD reset, XS26, pin 22 */

	#define BOARD_GT911_RESET_SET(v) do { gpioX_setstate(GPIOA, BOARD_GT911_RESET_PIN, !! (v)); local_delay_us(1); } while (0)
	#define BOARD_GT911_INT_SET(v) do { gpioX_setstate(GPIOA, BOARD_GPIOA_GT911_INT_PIN, !! (v)); local_delay_us(1); } while (0)

	#define BOARD_GT911_RESET_INITIO_1() do { \
		arm_hardware_pioa_outputs2m(BOARD_GPIOA_GT911_INT_PIN, 1 * BOARD_GPIOA_GT911_INT_PIN); \
		arm_hardware_pioa_outputs2m(BOARD_GT911_RESET_PIN, 1 * BOARD_GT911_RESET_PIN); \
		 local_delay_ms(200);  \
	} while (0)

	#define BOARD_GT911_RESET_INITIO_2() do { \
		arm_hardware_pioa_inputs(BOARD_GPIOA_GT911_INT_PIN); \
		arm_hardware_pioa_updown(_xMask, BOARD_GPIOA_GT911_INT_PIN, 0); \
	} while (0)

	#define BOARD_GT911_INT_CONNECT() do { \
		static einthandler_t h; \
		arm_hardware_pioa_inputs(BOARD_GPIOA_GT911_INT_PIN); \
		arm_hardware_pioa_updown(_xMask, BOARD_GPIOA_GT911_INT_PIN, 0); \
		arm_hardware_pioa_onchangeinterrupt(BOARD_GPIOA_GT911_INT_PIN, 1 * BOARD_GPIOA_GT911_INT_PIN, 0 * BOARD_GPIOA_GT911_INT_PIN, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM, & h, gt911_interrupt_handler); \
	} while (0)
	//gt911_interrupt_handler

#endif

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_XPT2046)

	#define BOARD_XPT2046_INT_PIN (UINT32_C(1) << 7)		/* PE7 : tsc interrupt */
	#define BOARD_XPT2046_INT_GET() (!! (gpioX_getinputs(GPIOE) & BOARD_XPT2046_INT_PIN))
	#define BOARD_XPT2046_INT_CONNECT() do { \
		arm_hardware_pioe_inputs(BOARD_XPT2046_INT_PIN); \
	} while (0)

#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_XPT2046) */

#define BOARD_BLINK_BIT0 (UINT32_C(1) << 4)	// PE4 BOOT_LED
#define BOARD_BLINK_BIT1 (UINT32_C(1) << 5)	// PE5 FRONTLED_RED
#define BOARD_BLINK_BIT2 (UINT32_C(1) << 6)	// PE6 FRONTLED_GREEN

#define BOARD_BLINK_INITIALIZE() do { \
	arm_hardware_pioe_outputs(BOARD_BLINK_BIT0, 1 * BOARD_BLINK_BIT0); \
	arm_hardware_pioe_outputs(BOARD_BLINK_BIT1, 1 * BOARD_BLINK_BIT1); \
	arm_hardware_pioe_outputs(BOARD_BLINK_BIT2, 1 * BOARD_BLINK_BIT2); \
} while (0)
#define BOARD_BLINK_SETSTATE(state) do { \
	gpioX_setstate(GPIOE, BOARD_BLINK_BIT0, !! (state) * BOARD_BLINK_BIT0); \
	gpioX_setstate(GPIOE, BOARD_BLINK_BIT1, !! (state) * BOARD_BLINK_BIT1); \
	gpioX_setstate(GPIOE, BOARD_BLINK_BIT2, !! (state) * BOARD_BLINK_BIT2); \
} while (0)

/* запрос на вход в режим загрузчика */
#define BOARD_IS_USERBOOT() (board_getadc_unfiltered_u8(KI5, 0, 15) == 0)	/* проверка нажатия кнопки дополнительного валкодера */

#if WITHETHHW
	#define ETHERNET_INITIALIZE() do { \
		arm_hardware_pioi_outputs(UINT32_C(1) << 6, 0 * UINT32_C(1) << 6); /* PI6 PHYRSTB */ \
		\
		arm_hardware_pioi_altfn50(UINT32_C(1) << 0, GPIO_CFG_AF2); 	/* PI0 RGMII_RXD3 */ \
		arm_hardware_pioi_altfn50(UINT32_C(1) << 1, GPIO_CFG_AF2); 	/* PI1 RGMII_RXD2 */ \
		arm_hardware_pioi_altfn50(UINT32_C(1) << 2, GPIO_CFG_AF2); 	/* PI2 RGMII_RXD1 */ \
		arm_hardware_pioi_altfn50(UINT32_C(1) << 3, GPIO_CFG_AF2); 	/* PI3 RGMII_RXD0 */ \
		arm_hardware_pioi_altfn50(UINT32_C(1) << 4, GPIO_CFG_AF2); 	/* PI4 RGMII_RXCK */ \
		arm_hardware_pioi_altfn50(UINT32_C(1) << 5, GPIO_CFG_AF2); 	/* PI5 RGMII_RXCTL */ \
		arm_hardware_pioi_altfn50(UINT32_C(1) << 7, GPIO_CFG_AF2); 	/* PI7 RGMII_TXD3 */ \
		arm_hardware_pioi_altfn50(UINT32_C(1) << 8, GPIO_CFG_AF2); 	/* PI8 RGMII_TXD2 */ \
		arm_hardware_pioi_altfn50(UINT32_C(1) << 9, GPIO_CFG_AF2); 	/* PI9 RGMII_TXD1 */ \
		arm_hardware_pioi_altfn50(UINT32_C(1) << 10, GPIO_CFG_AF2); /* PI10 RGMII_TXD0 */ \
		arm_hardware_pioi_altfn50(UINT32_C(1) << 11, GPIO_CFG_AF2); /* PI11 RGMII_TXCK */ \
		arm_hardware_pioi_altfn50(UINT32_C(1) << 12, GPIO_CFG_AF2); /* PI12 RGMII_TXCTL */ \
		arm_hardware_pioi_altfn50(UINT32_C(1) << 13, GPIO_CFG_AF2); /* PI13 RGMII_CLKIN */ \
		arm_hardware_pioi_altfn50(UINT32_C(1) << 14, GPIO_CFG_AF2); /* PI14 MDC */ \
		arm_hardware_pioi_altfn50(UINT32_C(1) << 15, GPIO_CFG_AF2); /* PI15 MDIO */ \
		\
		arm_hardware_pioi_outputs(UINT32_C(1) << 6, 1 * UINT32_C(1) << 6); /* PI6 PHYRSTB */ \
	} while (0)

#else /* WITHETHHW */

	#define ETHERNET_INITIALIZE() do { \
		arm_hardware_pioi_outputs(UINT32_C(1) << 6, 0 * UINT32_C(1) << 6); /* PI6 PHYRSTB */ \
	} while (0)

#endif /* WITHETHHW */

/* макроопределение, которое должно включить в себя все инициализации */
#define	HARDWARE_INITIALIZE() do { \
	BOARD_BLINK_INITIALIZE(); \
	HARDWARE_KBD_INITIALIZE(); \
	/*HARDWARE_DAC_INITIALIZE(); */\
	HARDWARE_BL_INITIALIZE(); \
	HARDWARE_DCDC_INITIALIZE(); \
	TXDISABLE_INITIALIZE(); \
	TUNE_INITIALIZE(); \
	USBD_EHCI_INITIALIZE(); \
	ETHERNET_INITIALIZE(); \
} while (0)

// TUSB parameters
#define TUP_DCD_ENDPOINT_MAX    6

#endif /* ARM_ALW_T507_CPU_VELOCI_V0_H_INCLUDED */
