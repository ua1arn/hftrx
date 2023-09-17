/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Трансивер с DSP обработкой "Аист" на процессоре Allwinner T507
// v5km7_ddc_sv9k_a53_R3.0.pcb Allwinner T507, 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
// HelperBoard T507 Core Board

#ifndef ARM_ALW_T507_CPU_HELPERBOARD_H_INCLUDED
#define ARM_ALW_T507_CPU_HELPERBOARD_H_INCLUDED 1

#define WITHSPI16BIT	1	/* возможно использование 16-ти битных слов при обмене по SPI */
#define WITHSPI32BIT	1	/* возможно использование 32-ти битных слов при обмене по SPI */
#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
//#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
//#define WITHSPISW 	1	/* Использование программного управления SPI. Нельзя убирать эту строку - требуется явное отключение из-за конфликта с I2C */

//#define WIHSPIDFSW	1	/* программное обслуживание DATA FLASH */
//#define WIHSPIDFOVERSPI 1	/* Для работы используется один из обычных каналов SPI */
//#define WIHSPIDFHW		1	/* аппаратное обслуживание DATA FLASH */
//#define WIHSPIDFHW2BIT	1	/* аппаратное обслуживание DATA FLASH с подддержкой QSPI подключения по 2-м проводам */
//#define WIHSPIDFHW4BIT	1	/* аппаратное обслуживание DATA FLASH с подддержкой QSPI подключения по 4-м проводам */

//#define WITHDMA2DHW		1	/* Использование DMA2D для формирования изображений	- у STM32MP1 его нет */

//#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */

#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */
#define WITHETHHW 1	/* Hardware Ethernet controller */

#if WITHDEBUG
	//#define WITHUART0HW	1	/* Используется периферийный контроллер последовательного порта UART0 */
	#define WITHUART2HW	1	/* Используется периферийный контроллер последовательного порта UART2 */
	//#define WITHUARTFIFO	1	/* испольование FIFO */
#endif /* WITHDEBUG */

//#define WITHCAT_USART2		1
#define WITHDEBUG_USART2	1
#define WITHNMEA_USART2		1	/* порт подключения GPS/GLONASS */

// OHCI at USB1HSFSP2_BASE
////#define WITHUSBHW_OHCI ((struct ohci_registers *) USB1HSFSP2_BASE)

#if WITHISBOOTLOADER

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

	#define WITHDCDCFREQCTL	1		// Имеется управление частотой преобразователей блока питания и/или подсветки дисплея

	#if WITHINTEGRATEDDSP

		#define WITHFPGAPIPE_CODEC1 1	/* Интерфейс к FPGA, транзитом в аудио кодек через I2S0 */
		#define WITHFPGAPIPE_RTS96 WITHRTS96	/* в том же фрейме иут квадратуры RTS96 */
		#define WITHFPGAPIPE_RTS192 WITHRTS192	/* в том же фрейме иут квадратуры RTS192 */
		#define WITHFPGAPIPE_NCORX0 1	/* управление частотой приемника 1 */
		#define WITHFPGAPIPE_NCORX1 1	/* управление частотой приемника 2 */
		#define WITHFPGAPIPE_NCORTS 1	/* управление частотой приемника панорамы */

		#define WITHI2S0HW	1	/* I2S0 - 16-ти канальный канал обмена с FPGA */
		//#define WITHI2S1HW	1	/* Использование I2S1 - аудиокодек на I2S */
		//#define WITHI2S2HW	1	/* Использование I2S2 - FPGA или IF codec	*/

		//#define WITHCODEC1_WHBLOCK_DUPLEX_MASTER	1	/* встороенный в процессор кодек */
		//#define WITHFPGAIF_I2S0_DUPLEX_MASTER	1		/* Обмен с FPGA через I2S0 */
		//#define WITHCODEC1_I2S1_DUPLEX_MASTER	1		/* Обмен с аудиокодеком через I2S1 */
		//#define WITHFPGAIF_I2S2_DUPLEX_MASTER	1		/* Обмен с FPGA через I2S2 */
		#define WITHFPGAIF_I2S0_DUPLEX_SLAVE	1		/* Обмен с FPGA через I2S0 */
		//#define WITHCODEC1_I2S1_DUPLEX_SLAVE	1		/* Обмен с аудиокодеком через I2S1 */
		//#define WITHFPGAIF_I2S2_DUPLEX_SLAVE	1		/* Обмен с FPGA через I2S2 */
	#endif /* WITHINTEGRATEDDSP */

	#define WITHMDMAHW		1	/* Использование G2D для формирования изображений */
	//#define WITHCPUDACHW	1	/* использование встроенного в процессор DAC */
	#define WITHCPUADCHW 	1	/* использование встроенного в процессор ADC */

	#define WITHLTDCHW		1	/* TCON + DE Наличие контроллера дисплея с framebuffer-ом */
	//#define WITHGPUHW	1	/* Graphic processor unit */
	#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */

	#define WITHUSBHW_DEVICE	USB20_OTG_DEVICE	/* на этом устройстве поддерживается функциональность DEVICE	*/
	#define WITHUSBDEV_VBUSSENSE	1		/* используется предопределенный вывод OTG_VBUS */
	//#define WITHUSBDEV_HSDESC	1			/* Требуется формировать дескрипторы как для HIGH SPEED */
	//#define WITHUSBDEV_HIGHSPEEDULPI	1	// ULPI
	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USB0_DP & USB0_DM
	//#define WITHUSBDEV_DMAENABLE 1

//	#define WITHUSBHW_HOST		USBOTG0
//	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USB0_DP & USB0_DM
//	#define WITHUSBHOST_DMAENABLE 1


//	#define WITHTINYUSB 1
//	#define BOARD_TUH_RHPORT 1
	#define WITHEHCIHW	1	/* USB_EHCI controller */
	#define WITHUSBHW_EHCI		USB20_HOST1_EHCI
	#define WITHUSBHW_OHCI		USB20_HOST1_OHCI

	#define WITHUSBHOST_HIGHSPEEDPHYC	1	// UTMI -> USB1_DP & USB1_DM
	#define WITHEHCIHW_EHCIPORT 0	// 0 - use 1st PHY port
	#define WITHOHCIHW_OHCIPORT 0

	#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
	#define WITHMODEM_CDC	1

	#if WITHINTEGRATEDDSP

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

#endif /* WITHISBOOTLOADER */

#define LS020_RS				(0 * UINT32_C(1) << 9)			// PA9 signal
#define LS020_RS_SET(v) 		do { gpioX_setstate(GPIOA, LS020_RS, !! (v) * LS020_RS); } while (0)

#define LS020_RS_INITIALIZE() do { \
		arm_hardware_pioa_outputs2m(LS020_RS, LS020_RS); /* PA9 */ \
	} while (0)

#define LS020_RESET				(UINT32_C(1) << 10)			// PA10 signal
#define LS020_RESET_SET(v) 		do { gpioX_setstate(GPIOA, LS020_RESET, !! (v) * LS020_RESET); } while (0)

#define LS020_RESET_INITIALIZE() do { \
		arm_hardware_pioa_outputs2m(LS020_RESET, LS020_RESET); /* PA10 */ \
	} while (0)

#if WITHENCODER

	// Выводы подключения енкодера #1
	#define ENCODER_INPUT_PORT	(gpioX_getinputs(GPIOA))
	#define ENCODER_BITA		(UINT32_C(1) << 5)		// PA5
	#define ENCODER_BITB		(UINT32_C(1) << 4)		// PA4

	// Выводы подключения енкодера #2
	#define ENCODER2_INPUT_PORT	(gpioX_getinputs(GPIOA))
	#define ENCODER2_BITA		(UINT32_C(1) << 7)		// PA7
	#define ENCODER2_BITB		(UINT32_C(1) << 6)		// PA6

	/* Определения масок битов для формирования обработчиков прерываний в нужном GPIO */
	#define BOARA_GPIOE_ENCODER_BITS		(ENCODER_BITA | ENCODER_BITB)
	#define BOARA_GPIOE_ENCODER2_BITS		(ENCODER2_BITA | ENCODER2_BITB)

	#define ENCODER_BITS_GET() (((ENCODER_INPUT_PORT & ENCODER_BITA) != 0) * 2 + ((ENCODER_INPUT_PORT & ENCODER_BITB) != 0))
	#define ENCODER2_BITS_GET() (((ENCODER2_INPUT_PORT & ENCODER2_BITA) != 0) * 2 + ((ENCODER2_INPUT_PORT & ENCODER2_BITB) != 0))

	#define ENCODER_INITIALIZE() do { \
			arm_hardware_pioa_inputs(BOARA_GPIOE_ENCODER_BITS); \
			arm_hardware_pioa_updown(BOARA_GPIOE_ENCODER_BITS, 0); \
			arm_hardware_pioa_onchangeinterrupt(BOARA_GPIOE_ENCODER_BITS, BOARA_GPIOE_ENCODER_BITS, BOARA_GPIOE_ENCODER_BITS, ARM_OVERREALTIME_PRIORITY, TARGETCPU_OVRT); \
			arm_hardware_pioa_inputs(BOARA_GPIOE_ENCODER2_BITS); \
			arm_hardware_pioa_updown(BOARA_GPIOE_ENCODER2_BITS, 0); \
			arm_hardware_pioa_onchangeinterrupt(0 * BOARA_GPIOE_ENCODER2_BITS, BOARA_GPIOE_ENCODER2_BITS, BOARA_GPIOE_ENCODER2_BITS, ARM_OVERREALTIME_PRIORITY, TARGETCPU_OVRT); \
		} while (0)

#endif

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

#if WITHSDHCHW
	#define	SMHCHARD_IX 0	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_PTR SMHC0	/* 0 - SMHC0, 1: SMHC1... */
	#define	SMHCHARD_CCU_CLK_REG (CCU->SMHC0_CLK_REG)	/* 0 - SMHC0, 1: SMHC1... */

	#if WITHSDHCHW4BIT
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
			arm_hardware_piof_updown(0, UINT32_C(1) << 3);	/* PF3 - SDIO_CMD	*/ \
			arm_hardware_piof_updown(0, UINT32_C(1) << 2);	/* PF2 - SDIO_CK	*/ \
			arm_hardware_piof_updown(0, UINT32_C(1) << 1);	/* PF1 - SDIO_D0	*/ \
			arm_hardware_piof_updown(0, UINT32_C(1) << 0);	/* PF0 - SDIO_D1	*/ \
			arm_hardware_piof_updown(0, UINT32_C(1) << 5);	/* PF5 - SDIO_D2	*/ \
			arm_hardware_piof_updown(0, UINT32_C(1) << 4);	/* PF4 - SDIO_D3	*/ \
		} while (0)
	#else /* WITHSDHCHW4BIT */
		#define HARDWARE_SDIO_INITIALIZE() do { \
		arm_hardware_piof_altfn50(UINT32_C(1) << 3, GPIO_CFG_AF2);	/* PF3 - SDIO_CMD	*/ \
		arm_hardware_piof_altfn50(UINT32_C(1) << 2, GPIO_CFG_AF2);	/* PF2 - SDIO_CK	*/ \
		arm_hardware_piof_altfn50(UINT32_C(1) << 1, GPIO_CFG_AF2);	/* PF1 - SDIO_D0	*/ \
		} while (0)
		/* отключить процессор от SD карты - чтобы при выполнении power cycle не возникало фантомное питание через сигналы управления. */
		#define HARDWARE_SDIO_HANGOFF()	do { \
		arm_hardware_piof_inputs(UINT32_C(1) << 3);	/* PF3 - SDIO_CMD	*/ \
		arm_hardware_piof_inputs(UINT32_C(1) << 2);	/* PF2 - SDIO_CK	*/ \
		arm_hardware_piof_inputs(UINT32_C(1) << 1);	/* PF1 - SDIO_D0	*/ \
		arm_hardware_piof_inputs(UINT32_C(1) << 6);	/* PF6 - SDC0_DET	*/ \
		arm_hardware_piof_updown(0, UINT32_C(1) << 3);	/* PF3 - SDIO_CMD	*/ \
		arm_hardware_piof_updown(0, UINT32_C(1) << 2);	/* PF2 - SDIO_CK	*/ \
		arm_hardware_piof_updown(0, UINT32_C(1) << 1);	/* PF1 - SDIO_D0	*/ \
		} while (0)
	#endif /* WITHSDHCHW4BIT */

	#define HARDWARE_SDIO_CD_BIT	(UINT32_C(1) << 6)	/* PF6 - SDC0_DET */

	#define HARDWARE_SDIOSENSE_INITIALIZE()	do { \
			arm_hardware_piog_inputs(HARDWARE_SDIO_CD_BIT); /* PF6 - SDC0_DET */ \
			arm_hardware_piog_updown(HARDWARE_SDIO_CD_BIT, 0); \
	} while (0)


	#define HARDWARE_SDIOSENSE_CD() ((GPIOF->DATA & HARDWARE_SDIO_CD_BIT) == 0)	/* == 0: no disk. получить состояние датчика CARD PRESENT */
	#define HARDWARE_SDIOSENSE_WP() 0//((GPIOG->DATA & HARDWARE_SDIO_WP_BIT) != 0)	/* != 0: write protected получить состояние датчика CARD WRITE PROTECT */

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
			arm_hardware_pioe_updown(0, TXDISABLE_BIT_TXDISABLE); \
		} while (0)
	// ---

	// +++
	// PTT input - PD10
	// PTT2 input - PD9
	#define PTT_TARGET_PIN				gpioX_getinputs(GPIOE)
	#define PTT_BIT_PTT					(UINT32_C(1) << 8)		// PE8 - PTT
	#define PTT2_TARGET_PIN				gpioX_getinputs(GPIOE)
	#define PTT2_BIT_PTT				(UINT32_C(1) << 9)		// PE9 - PTT2
	#define PTT3_TARGET_PIN				gpioX_getinputs(GPIOE)
	#define PTT3_BIT_PTT				(UINT32_C(1) << 10)		// PE10 - PTT3
	// получить бит запроса оператором перехода на пердачу
	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0 || (PTT2_TARGET_PIN & PTT2_BIT_PTT) == 0 || (PTT3_TARGET_PIN & PTT3_BIT_PTT) == 0)
	#define PTT_INITIALIZE() do { \
			arm_hardware_pioe_inputs(PTT_BIT_PTT); \
			arm_hardware_pioe_updown(PTT_BIT_PTT, 0); \
			arm_hardware_pioe_inputs(PTT2_BIT_PTT); \
			arm_hardware_pioe_updown(PTT2_BIT_PTT, 0); \
			arm_hardware_pioe_inputs(PTT3_BIT_PTT); \
			arm_hardware_pioe_updown(PTT3_BIT_PTT, 0); \
		} while (0)
	// ---
	// TUNE input - PD11
	#define TUNE_TARGET_PIN				gpioX_getinputs(GPIOE)
	#define TUNE_BIT_TUNE				(UINT32_C(1) << 6)		// PE6
	#define HARDWARE_GET_TUNE() 0//((TUNE_TARGET_PIN & TUNE_BIT_TUNE) == 0)
	#define TUNE_INITIALIZE() do { \
			arm_hardware_pioe_inputs(TUNE_BIT_TUNE); \
			arm_hardware_pioe_updown(TUNE_BIT_TUNE, 0); \
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
	#define ELKEY_BIT_LEFT				(UINT32_C(1) << 4)		// PE4
	#define ELKEY_BIT_RIGHT				(UINT32_C(1) << 5)		// PE5

	#define ELKEY_TARGET_PIN			gpioX_getinputs(GPIOE)

	#define HARDWARE_GET_ELKEY_LEFT() 	((ELKEY_TARGET_PIN & ELKEY_BIT_LEFT) == 0)
	#define HARDWARE_GET_ELKEY_RIGHT() 	((ELKEY_TARGET_PIN & ELKEY_BIT_RIGHT) == 0)


	#define ELKEY_INITIALIZE() do { \
			arm_hardware_pioe_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
			arm_hardware_pioe_updown(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, 0); \
		} while (0)

#endif /* WITHELKEY */

// IOUPDATE = PA15
//#define SPI_IOUPDATE_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); (void) GPIOA->BSRR; } while (0)
//#define SPI_IOUPDATE_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); (void) GPIOA->BSRR; } while (0)
//#define SPI_IOUPDATE_BIT		(UINT32_C(1) << 15)	// * PA15

#if WITHSPIHW || WITHSPISW
	// Набор определений для работы без внешнего дешифратора

	#define OE_CTL1_BIT	(UINT32_C(1) << 13)	/* PI13 */
	//#define targetdataflash 0xFF
	#define targetnone 0x00

	#define targetext1		(UINT32_C(1) << 9)		// PI9 ext1 on front panel CSEXT1
	#define targetnvram		(UINT32_C(1) << 10)		// PI10 nvram FM25W356
	#define targetctl1		(UINT32_C(1) << 15)		// PI15 board control registers chain
	#define targetcodec1	(UINT32_C(1) << 0)		// PI0 on-board codec1 NAU8822L
	#define targetfpga1		(UINT32_C(1) << 12)		// PI12 FPGA control registers CS1
	//#define targetrtc1		(UINT32_C(1) << 10)		// PIx RTC DS1305 RTC_CS

	#define targetadc2		(UINT32_C(1) << 7)	// PI7 on-board ADC MCP3208-BI/SL chip select (potentiometers, swr-meter) ADC1CS
	#define targetadck		(UINT32_C(1) << 8)	// PI8 on-board ADC MCP3208-BI/SL chip select (KEYBOARD) ADC2CS
	#define targetxad2		(UINT32_C(1) << 11)	// PI11 ext2 external SPI device (PA BOARD ADC) CSEXT2

	#define targetlcd	targetext1 	/* LCD over SPI line devices control */ 
	#define targetuc1608 targetext1	/* LCD with positive chip select signal	*/
	#define targettsc1 		targetext1	/* XPT2046 SPI chip select signal */

	/* Select specified chip. */
	#define SPI_CS_ASSERT(target) do { \
		switch (target) { \
		/*case targetdataflash: { gpioX_setstate(GPIOI, SPDIF_NCS_BIT, 0 * (SPDIF_NCS_BIT)); local_delay_us(1); } break; *//* PC3 SPI0_CS */ \
		/*case targetrtc1: { gpioX_setstate(GPIOI, (target), 1 * (target)); local_delay_us(1); } break; */\
		default: { gpioX_setstate(GPIOI, (target), 0 * (target)); local_delay_us(1); } break; \
		case targetnone: break; \
		} \
	} while (0)

	/* Unelect specified chip. */
	#define SPI_CS_DEASSERT(target)	do { \
		switch (target) { \
		/*case targetdataflash: { gpioX_setstate(GPIOI, SPDIF_NCS_BIT, 1 * (SPDIF_NCS_BIT)); local_delay_us(1); } break; *//* PC3 SPI0_CS */ \
		/*case targetrtc1: { gpioX_setstate(GPIOI, (target), 0 * (target)); local_delay_us(1); } break; */\
		case targetctl1: { gpioX_setstate(GPIOI, (target), 1 * (target)); gpioX_setstate(GPIOI, OE_CTL1_BIT, 0 * OE_CTL1_BIT); local_delay_us(1); } break; \
		default: { gpioX_setstate(GPIOI, (target), 1 * (target)); local_delay_us(1); } break; \
		case targetnone: break; \
		} \
	} while (0)

	/* инициализация линий выбора периферийных микросхем */
	#define SPI_ALLCS_INITIALIZE() do { \
		/*arm_hardware_pioc_outputs(SPDIF_NCS_BIT, 1 * SPDIF_NCS_BIT); */	/* PC3 SPI0_CS */ \
		arm_hardware_pioi_outputs(OE_CTL1_BIT, 1 * OE_CTL1_BIT); /*  */ \
		arm_hardware_pioi_outputs(targetext1, 1 * targetext1); /*  */ \
		arm_hardware_pioi_outputs(targetnvram, 1 * targetnvram); /*  */ \
		arm_hardware_pioi_outputs(targetctl1, 1 * targetctl1); /*  */ \
		arm_hardware_pioi_outputs(targetcodec1, 1 * targetcodec1); /*  */ \
		arm_hardware_pioi_outputs(targetfpga1, 1 * targetfpga1); /*  */ \
		/*arm_hardware_pioi_outputs(targetrtc1, 0 * targetrtc1);*/ /*  */ \
		arm_hardware_pioi_outputs(targetadc2, 1 * targetadc2); /*  */ \
		arm_hardware_pioi_outputs(targetadck, 1 * targetadck); /*  */ \
		arm_hardware_pioi_outputs(targetxad2, 1 * targetxad2); /*  */ \
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

	#if WITHSPIHW
		#define SPIIO_INITIALIZE() do { \
			arm_hardware_pioh_altfn2(SPI_SCLK_BIT, GPIO_CFG_AF4); 	/* PH6 SPI1_CLK */ \
			arm_hardware_pioh_altfn2(SPI_MOSI_BIT, GPIO_CFG_AF4); 	/* PH7 SPI1_MOSI */ \
			arm_hardware_pioh_altfn2(SPI_MISO_BIT, GPIO_CFG_AF4); 	/* PH8 SPI1_MISO */ \
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
		arm_hardware_pioh_updown(RXMASK, 0); \
	} while (0)

// WITHUART2HW
// Используется периферийный контроллер последовательного порта UART2 */
#define HARDWARE_UART2_INITIALIZE() do { \
		const portholder_t TXMASK = UINT32_C(1) << 5; /* PI5 UART2-TX */ \
		const portholder_t RXMASK = UINT32_C(1) << 6; /* PI6 UART2-RX - pull-up RX data */  \
		arm_hardware_pioi_altfn2(TXMASK, GPIO_CFG_AF3); \
		arm_hardware_pioi_altfn2(RXMASK, GPIO_CFG_AF3); \
		arm_hardware_pioi_updown(RXMASK, 0); \
	} while (0)


#define BOARD_GPIOA_ENC2BTN_BIT (UINT32_C(1) << 8)	// PA8 - second encoder button with pull-up

#if WITHKEYBOARD
	/* PE15: pull-up second encoder button */

	//#define BOARD_GPIOA_ENC2BTN_BIT (UINT32_C(1) << 15)	// PE15 - second encoder button with pull-up
	#define TARGET_POWERBTN_BIT 0//(UINT32_C(1) << 8)	// PAxx - ~CPU_POWER_SW signal

#if WITHENCODER2
	// P7_8
	#define TARGET_ENC2BTN_GET	(((gpioX_getinputs(GPIOA)) & BOARD_GPIOA_ENC2BTN_BIT) == 0)
#endif /* WITHENCODER2 */

#if WITHPWBUTTON
	// P5_3 - ~CPU_POWER_SW signal
	#define TARGET_POWERBTN_GET	0//(((GPIOx->DATA) & TARGET_POWERBTN_BIT) == 0)
#endif /* WITHPWBUTTON */

	#define HARDWARE_KBD_INITIALIZE() do { \
			arm_hardware_pioa_inputs(BOARD_GPIOA_ENC2BTN_BIT); \
			arm_hardware_pioa_updown(BOARD_GPIOA_ENC2BTN_BIT, 0); /* PE15: pull-up second encoder button */ \
			/*arm_hardware_pioa_inputs(TARGET_POWERBTN_BIT); */ \
			/*arm_hardware_pioa_updown(TARGET_POWERBTN_BIT, 0);	*//* PAxx: pull-up second encoder button */ \
		} while (0)

#else /* WITHKEYBOARD */

	#define HARDWARE_KBD_INITIALIZE() do { \
		} while (0)

#endif /* WITHKEYBOARD */

#if 1
	// PL0 S-TWI0-SCK
	// PL1 S-TWI0-SDA
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
	} while (0)
	#define	TWISOFT_DEINITIALIZE() do { \
		arm_hardware_piol_inputs(TARGET_TWI_TWCK); 	/* SCL */ \
		arm_hardware_piol_inputs(TARGET_TWI_TWD);	/* SDA */ \
	} while (0)
	// Инициализация битов портов ввода-вывода для аппаратной реализации I2C
	// присоединение выводов к периферийному устройству
	#define	TWIHARD_INITIALIZE() do { \
		arm_hardware_piol_altfn2(TARGET_TWI_TWCK, GPIO_CFG_AF4x);	/* PL0 - S_TWI0_SCK */ \
		arm_hardware_piol_altfn2(TARGET_TWI_TWD, GPIO_CFG_AF4x);		/* PL1 - S_TWI0_SDA */ \
	} while (0)
	#define	TWIHARD_IX 0x	/* 0 - TWI0, 1: TWI1... */
	#define	TWIHARD_PTR TWI0x	/* 0 - TWI0, 1: TWI1... */



#elif WITHTWISW || WITHTWIHW
	// PA0 - TWI0_SCL
	// PA1 - TWI0_SDA
	#define TARGET_TWI_TWCK		(UINT32_C(1) << 0)
	#define TARGET_TWI_TWCK_PIN		(gpioX_getinputs(GPIOA))
	#define TARGET_TWI_TWCK_PORT_C(v) do { arm_hardware_pioa_outputs((v), 0); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do { arm_hardware_pioa_inputs(v); } while (0)

	#define TARGET_TWI_TWD		(UINT32_C(1) << 1)
	#define TARGET_TWI_TWD_PIN		(gpioX_getinputs(GPIOA))
	#define TARGET_TWI_TWD_PORT_C(v) do { arm_hardware_pioa_outputs((v), 0); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v) do { arm_hardware_pioa_inputs(v); } while (0)

	// Инициализация битов портов ввода-вывода для программной реализации I2C
	#define	TWISOFT_INITIALIZE() do { \
			arm_hardware_pioa_inputs(TARGET_TWI_TWCK); /* SCL */ \
			arm_hardware_pioa_inputs(TARGET_TWI_TWD);  	/* SDA */ \
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
		} while (0) 
	#define	TWIHARD_IX 0	/* 0 - TWI0, 1: TWI1... */
	#define	TWIHARD_PTR TWI0	/* 0 - TWI0, 1: TWI1... */


#endif /* WITHTWISW || WITHTWIHW */

#if WITHFPGAWAIT_AS || WITHFPGALOAD_PS

	/* outputs */
	#define FPGA_NCONFIG_PORT_S(v)	do { gpioX_setstate(GPIOE, (v), !! (1) * (v)); } while (0)
	#define FPGA_NCONFIG_PORT_C(v)	do { gpioX_setstate(GPIOE, (v), !! (0) * (v)); } while (0)
	#define FPGA_NCONFIG_BIT		(UINT32_C(1) << 14)	/* PE14 bit conneced to nCONFIG pin ALTERA FPGA */

	/* inputs */
	#define FPGA_CONF_DONE_INPUT	gpioX_getinputs(GPIOE)
	#define FPGA_CONF_DONE_BIT		(UINT32_C(1) << 16)	/* PE16 bit conneced to CONF_DONE pin ALTERA FPGA */

	#define FPGA_NSTATUS_INPUT		gpioX_getinputs(GPIOE)
	#define FPGA_NSTATUS_BIT		(UINT32_C(1) << 15)	/* PE15 bit conneced to NSTATUS pin ALTERA FPGA */

	#define FPGA_INIT_DONE_INPUT	gpioX_getinputs(GPIOE)
	#define FPGA_INIT_DONE_BIT		(UINT32_C(1) << 17)	/* PE17 bit conneced to INIT_DONE pin ALTERA FPGA */

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
	#define HARDWARE_FPGA_IS_USER_MODE() (local_delay_ms(100), (FPGA_INIT_DONE_INPUT & FPGA_INIT_DONE_BIT) != 0)

#endif /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS */

#if WITHDSPEXTFIR
	// Биты доступа к массиву коэффициентов FIR фильтра в FPGA

	// FPGA PIN_23
	#define TARGET_FPGA_FIR_CS_PORT_C(v)	do { gpioX_setstate(GPIOE, (v), !! (0) * (v)); } while (0) // do { GPIOC->BSRR = BSRR_C(v); (void) GPIOC->BSRR; } while (0)
	#define TARGET_FPGA_FIR_CS_PORT_S(v)	do { gpioX_setstate(GPIOE, (v), !! (1) * (v)); } while (0) // do { GPIOC->BSRR = BSRR_S(v); (void) GPIOC->BSRR; } while (0)
	#define TARGET_FPGA_FIR_CS_BIT (UINT32_C(1) << 2)	/* PE2 - fir CS ~FPGA_FIR_CLK */

	// FPGA PIN_8
	#define TARGET_FPGA_FIR1_WE_PORT_C(v)	do { gpioX_setstate(GPIOE, (v), !! (0) * (v)); } while (0) // do { GPIOD->BSRR = BSRR_C(v); (void) GPIOD->BSRR; } while (0)
	#define TARGET_FPGA_FIR1_WE_PORT_S(v)	do { gpioX_setstate(GPIOE, (v), !! (1) * (v)); } while (0) // do { GPIOD->BSRR = BSRR_S(v); (void) GPIOD->BSRR; } while (0)
	#define TARGET_FPGA_FIR1_WE_BIT (UINT32_C(1) << 3)	/* PE3 - fir1 WE */

	// FPGA PIN_7
	#define TARGET_FPGA_FIR2_WE_PORT_C(v)	do { gpioX_setstate(GPIOE, (v), !! (0) * (v)); } while (0) // do { GPIOD->BSRR = BSRR_C(v); (void) GPIOD->BSRR; } while (0)
	#define TARGET_FPGA_FIR2_WE_PORT_S(v)	do { gpioX_setstate(GPIOE, (v), !! (1) * (v)); } while (0) // do { GPIOD->BSRR = BSRR_S(v); (void) GPIOD->BSRR; } while (0)
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

	#define TARGET_GPIOE_VBUSON_BIT (UINT32_C(1) << 18)	// PE18 - единицей включение питания для device
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

#if WITHDCDCFREQCTL
	// ST ST1S10 Synchronizable switching frequency from 400 kHz up to 1.2 MHz
	#define WITHHWDCDCFREQMIN 400000L
	#define WITHHWDCDCFREQMAX 1200000L
	#define HARDWARE_DCDC_PWMCH 4	/* PWM4 */

	// PI14 - DC-DC synchro output
	// PWM5 AF6
	#define	HARDWARE_DCDC_INITIALIZE() do { \
		hardware_dcdcfreq_pwm_initialize(HARDWARE_DCDC_PWMCH); \
		arm_hardware_pioi_altfn2((UINT32_C(1) << 14), GPIO_CFG_AF5); /* PI14 - PWM4 */ \
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

	/* BL0: PA12. BL1: PA11, EN: PD28  */
	#define	HARDWARE_BL_INITIALIZE() do { \
		const portholder_t ENmask = (UINT32_C(1) << 28); /* PD28 */ \
		const portholder_t BLPinMSB = UINT32_C(1) << 11; /* PA11 - MSB open drain */ \
		const portholder_t BLPinLSB = UINT32_C(1) << 12; /* PA12 - LSB open drain */ \
		arm_hardware_pioa_opendrain(BLPinMSB, BLPinMSB); /* минимальный ток */ \
		arm_hardware_pioa_opendrain(BLPinLSB, BLPinLSB); /* минимальный ток */ \
		arm_hardware_piod_outputs(ENmask, 0 * ENmask); \
	} while (0)

	/* установка яркости и включение/выключение преобразователя подсветки */
	/* LCD_BL_ADJ0: PA12, LCD_BL_ADJ1: PA11, LCD_BL_ENABLE:PD28 */
	#define HARDWARE_BL_SET(en, level) do { \
		const portholder_t ENmask = UINT32_C(1) << 28; /* PD28 */ \
		const portholder_t BLPinMSB = UINT32_C(1) << 11; /* PA11 - MSB open drain */ \
		const portholder_t BLPinLSB = UINT32_C(1) << 12; /* PA12 - LSB open drain */ \
		gpioX_setopendrain(GPIOA, BLPinMSB, BLPinMSB * ! (level & 0x02)); /* Больший ток - нулём */ \
		gpioX_setopendrain(GPIOA, BLPinLSB, BLPinLSB * ! (level & 0x01)); /* Больший ток - нулём */ \
		gpioX_setstate(GPIOD, ENmask, !! (en) * ENmask); \
	} while (0)

#if WITHLTDCHW

	// Sochip_VE_S3_Datasheet_V1.0.pdf
	// TCON0_TRM_CTL_REG offset 0x0010
	// User manual:
	// LCD FRM Control Register (Default Value: 0x0000_0000)
	//#define TCON_FRM_MODE_VAL ((UINT32_C(1) << 31) | (UINT32_C(1) << 6) | (UINT32_C(0) << 5) | (UINT32_C(1) << 4))	// 16 bit panel connected
	#define TCON_FRM_MODE_VAL ((UINT32_C(1) << 31) | (UINT32_C(0) << 6) | (UINT32_C(0) << 5)| (UINT32_C(0) << 4))	// 18 bit panel connected
	//#define TCON_FRM_MODE_VAL 0	// 24 bit panel

	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_INITIALIZE(demode) do { \
		const portholder_t VSmask = (UINT32_C(1) << 27); 	/* PD27 LCD_VSYNC */ \
		const portholder_t HSmask = (UINT32_C(1) << 26); 	/* PD26 LCD_HSYNC */ \
		const portholder_t DEmask = (UINT32_C(1) << 25); 	/* PD25 LCD_DE */ \
		const portholder_t MODEmask = (UINT32_C(1) << 9); 	/* PA9 mode */ \
		/* set LCD DE/SYNC mode */ \
		arm_hardware_pioa_outputs(MODEmask, ((demode) != 0) * MODEmask);	/* PA9 = state */ \
		/* synchro signals - sync mode */ \
		arm_hardware_piod_outputs(((demode) == 0) * DEmask, 0 * DEmask); /* PD25 LCD_DE */ \
		arm_hardware_piod_altfn50(((demode) == 0) * VSmask, GPIO_CFG_AF2); /* PD27 LCD_VSYNC */ \
		arm_hardware_piod_altfn50(((demode) == 0) * HSmask, GPIO_CFG_AF2); /* PD26 LCD_HSYNC */ \
		/* synchro signals - DE mode */ \
		arm_hardware_piod_altfn50(((demode) != 0) * DEmask, GPIO_CFG_AF2); /* PD19 LCD_DE */ \
		arm_hardware_piod_outputs(((demode) != 0) * VSmask, 1 * VSmask); /* PD27 LCD_VSYNC */ \
		arm_hardware_piod_outputs(((demode) != 0) * HSmask, 1 * HSmask); /* PD25 LCD_HSYNC */ \
		/* pixel clock */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 24, GPIO_CFG_AF2); /* PD24 LCD_CLK */ \
		/* RED */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 19, GPIO_CFG_AF2); /* R3 PD19 LCD_D19 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 20, GPIO_CFG_AF2); /* R4 PD20 LCD_D20 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 21, GPIO_CFG_AF2); /* R5 PD21 LCD_D21 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 22, GPIO_CFG_AF2); /* R6 PD22 LCD_D22 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 23, GPIO_CFG_AF2); /* R7 PD23 LCD_D23 */ \
		/* GREEN */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 10, GPIO_CFG_AF2); 	/* G2 PD10 LCD_D10 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 11, GPIO_CFG_AF2); 	/* G3 PD11 LCD_D11 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 12, GPIO_CFG_AF2); 	/* G4 PD12 LCD_D12 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 13, GPIO_CFG_AF2); 	/* G5 PD13 LCD_D13 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 14, GPIO_CFG_AF2); /* G6 PD14 LCD_D14 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 15, GPIO_CFG_AF2); /* G7 PD15 LCD_D15 */ \
		/* BLUE  */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 3, GPIO_CFG_AF2); 	/* B3 PD3 LCD_D3 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 4, GPIO_CFG_AF2); 	/* B4 PD4 LCD_D4 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 5, GPIO_CFG_AF2); 	/* B5 PD5 LCD_D5 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 6, GPIO_CFG_AF2); 	/* B6 PD6 LCD_D6 */ \
		arm_hardware_piod_altfn50(UINT32_C(1) << 7, GPIO_CFG_AF2); 	/* B7 PD7 LCD_D7 */ \
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
	} while (0)

	#define	TCONLCD_IX 0	/* 0 - TCON_LCD0, 1: TCON_LCD1 */
	#define	TCONLCD_PTR TCON_LCD0	/* 0 - TCON_LCD0, 1: TCON_LCD1 */
	#define	TCONLCD_CCU_CLK_REG (CCU->TCON_LCD0_CLK_REG)	/* 0 - TCON_LCD0, 1: TCON_LCD1 */
	#define BOARD_TCONLCDFREQ (allwnr_t507_get_tcon_lcd0_freq())

#endif /* WITHLTDCHW */


	#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811)

		//	tsc interrupt XS26, pin 08
		//	tsc/LCD reset, XS26, pin 22
		//	tsc SCL: XS26, pin 01
		//	tsc SDA: XS26, pin 02

		void stmpe811_interrupt_handler(void);

		#define BOARD_GPIOA_STMPE811_INT_PIN (UINT32_C(1) << 3)		/* PA3 : tsc interrupt XS26, pin 08 */
		//#define BOARD_STMPE811_RESET_PIN (UINT32_C(1) << 4)	/* PA10 : tsc/LCD reset, XS26, pin 22 */

		#define BOARD_STMPE811_INT_CONNECT() do { \
			arm_hardware_pioa_inputs(BOARD_GPIOA_STMPE811_INT_PIN); \
			arm_hardware_pioa_updown(BOARD_GPIOA_STMPE811_INT_PIN, 0); \
			arm_hardware_pioa_onchangeinterrupt(BOARD_GPIOA_STMPE811_INT_PIN, 1 * BOARD_GPIOA_STMPE811_INT_PIN, 0 * BOARD_GPIOA_STMPE811_INT_PIN, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM); \
		} while (0)
#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811) */

	#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_GT911)

		//	tsc interrupt XS26, pin 08
		//	tsc/LCD reset, XS26, pin 22
		//	tsc SCL: XS26, pin 01
		//	tsc SDA: XS26, pin 02

		void gt911_interrupt_handler(void);

		#define BOARD_GPIOA_GT911_INT_PIN (UINT32_C(1) << 3)		/* PA3 : tsc interrupt XS26, pin 08 */
		#define BOARD_GT911_RESET_PIN (UINT32_C(1) << 4)			/* PA10 : tsc/LCD reset, XS26, pin 22 */

		#define BOARD_GT911_RESET_SET(v) do { gpioX_setstate(GPIOA, BOARD_GT911_RESET_PIN, !! (v)); } while (0)
		#define BOARD_GT911_INT_SET(v) do { gpioX_setstate(GPIOA, BOARD_GPIOA_GT911_INT_PIN, !! (v)); } while (0)

		#define BOARD_GT911_RESET_INITIO_1() do { \
			arm_hardware_pioa_outputs2m(BOARD_GPIOA_GT911_INT_PIN, 1 * BOARD_GPIOA_GT911_INT_PIN); \
			arm_hardware_pioa_outputs2m(BOARD_GT911_RESET_PIN, 1 * BOARD_GT911_RESET_PIN); \
			 local_delay_ms(200);  \
		} while (0)

		#define BOARD_GT911_RESET_INITIO_2() do { \
			arm_hardware_pioa_inputs(BOARD_GPIOA_GT911_INT_PIN); \
			arm_hardware_pioa_updown(BOARD_GPIOA_GT911_INT_PIN, 0); \
		} while (0)

		#define BOARD_GT911_INT_CONNECT() do { \
			arm_hardware_pioa_inputs(BOARD_GPIOA_GT911_INT_PIN); \
			arm_hardware_pioa_updown(BOARD_GPIOA_GT911_INT_PIN, 0); \
			arm_hardware_pioa_onchangeinterrupt(BOARD_GPIOA_GT911_INT_PIN, 1 * BOARD_GPIOA_GT911_INT_PIN, 0 * BOARD_GPIOA_GT911_INT_PIN, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM); \
		} while (0)
		//gt911_interrupt_handler

	#endif

	#define BOARD_BLINK_BIT0 (UINT32_C(1) << 24)	// PD24 - Banana Pi M64 led0 RED - active "1" (default has pull-up)
	#define BOARD_BLINK_BIT1 (UINT32_C(1) << 14)	// PE14 - Banana Pi M64 led1 GREEN - active "1"
	#define BOARD_BLINK_BIT2 (UINT32_C(1) << 15)	// PE15 - Banana Pi M64 led2 BLUE - active "1"

#if 0
	#define BOARD_BLINK_INITIALIZE() do { \
		arm_hardware_piod_outputs(BOARD_BLINK_BIT0, 1 * BOARD_BLINK_BIT0); \
		arm_hardware_pioe_outputs(BOARD_BLINK_BIT1, 1 * BOARD_BLINK_BIT1); \
		arm_hardware_pioe_outputs(BOARD_BLINK_BIT2, 1 * BOARD_BLINK_BIT2); \
	} while (0)
	#define BOARD_BLINK_SETSTATE(state) do { \
		gpioX_setstate(GPIOD, BOARD_BLINK_BIT0, !! (state) * BOARD_BLINK_BIT0); \
		gpioX_setstate(GPIOE, BOARD_BLINK_BIT1, !! (state) * BOARD_BLINK_BIT1); \
		gpioX_setstate(GPIOE, BOARD_BLINK_BIT2, !! (state) * BOARD_BLINK_BIT2); \
	} while (0)
#endif

	/* запрос на вход в режим загрузчика */
	#define BOARD_GPIOA_USERBOOT_BIT	(UINT32_C(1) << 8)	/* PA8: ~USER_BOOT - same as BOARD_GPIOA_ENC2BTN_BIT */
	#define BOARD_IS_USERBOOT() (((gpioX_getinputs(GPIOA)) & BOARD_GPIOA_USERBOOT_BIT) == 0)
	#define BOARD_USERBOOT_INITIALIZE() do { \
			arm_hardware_pioa_inputs(BOARD_GPIOA_USERBOOT_BIT); /* set as input with pull-up */ \
		} while (0)

	/* макроопределение, которое должно включить в себя все инициализации */
	#define	HARDWARE_INITIALIZE() do { \
			/*BOARD_BLINK_INITIALIZE(); */\
			HARDWARE_KBD_INITIALIZE(); \
			/*HARDWARE_DAC_INITIALIZE(); */\
			HARDWARE_BL_INITIALIZE(); \
			HARDWARE_DCDC_INITIALIZE(); \
			TXDISABLE_INITIALIZE(); \
			TUNE_INITIALIZE(); \
			BOARD_USERBOOT_INITIALIZE(); \
			USBD_EHCI_INITIALIZE(); \
		} while (0)

#endif /* ARM_ALW_T507_CPU_HELPERBOARD_H_INCLUDED */
